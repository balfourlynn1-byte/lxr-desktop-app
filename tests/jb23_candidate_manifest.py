#!/usr/bin/env python3
"""JB-23: produce a provenance/checksum manifest for a test-only LXR-02 image.

The manifest verifies the built updater; it does NOT validate realtime timing,
physical outputs, installation safety or the user's device/kit backup.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import sys
from datetime import datetime, timezone


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def run_checked(*command: str) -> str:
    result = subprocess.run(command, check=True, capture_output=True, text=True)
    return result.stdout.strip()


def make_manifest(
    image: Path, binary: Path, log: Path, output: Path,
    sha: str, workflow_run: str, branch: str, version: str,
) -> dict:
    if version not in ("1.8.04","1.8.05"):
        raise ValueError("Unexpected version for JB-23 candidate: " + version)
    if image.name != f"LXR_V2_V{version}.img":
        raise ValueError("Unexpected image name: " + image.name)
    if not re.fullmatch(r"[0-9a-f]{40}", sha):
        raise ValueError("Invalid Git SHA")
    if not image.is_file() or not binary.is_file() or not log.is_file():
        raise FileNotFoundError("Image, .bin or ARM build log missing")
    build_text = log.read_text(encoding="utf-8", errors="replace")

    # Reuse project-owned bootloader-format and linker memory validators.
    run_checked(sys.executable, "tests/validate_image.py", str(image),
                "--binary", str(binary))
    memory_output = run_checked(sys.executable, "tests/check_jb22_memory.py", str(log))
    match = re.search(r"ITCM_RAM:\s*(\d+)/16384 B; (\d+) B headroom", memory_output)
    if not match:
        raise ValueError("Missing verified ITCM evidence")
    used, free = map(int, match.groups())
    if used + free != 16384 or free < 512:
        raise ValueError("Failed ITCM budget")
    if "region `ITCM_RAM` overflowed" in build_text:
        raise ValueError("Build shows ITCM overflow")

    data = image.read_bytes()
    magic, payload_size, checksum = struct.unpack("<8sIB3x", data[:16])
    if magic != b"LXRV2IMG" or payload_size != len(data) - 16:
        raise ValueError("Invalid desktop updater")
    if sha256(binary) != hashlib.sha256(data[16:]).hexdigest():
        raise ValueError("Firmware binary differs from image payload")

    manifest = {
        "artifact_type": "JB23_EXPERIMENTAL_HARDWARE_UNVERIFIED",
        "purpose": "PERSONAL_LXR02_TESTING_ONLY_NOT_A_STABLE_RELEASE",
        "hardware_tested": False,
        "safe_for_live_performance": False,
        "firmware_version": version,
        "source_commit_sha": sha,
        "source_branch": branch,
        "github_actions_run_url": workflow_run,
        "target_hardware": "Erica Synths LXR-02 desktop STM32F765",
        "image_filename": image.name,
        "image_size_bytes": len(data),
        "image_sha256": sha256(image),
        "binary_sha256": sha256(binary),
        "image_payload_bytes": payload_size,
        "image_header_checksum": checksum,
        "itcm_used_bytes": used,
        "itcm_free_bytes": free,
        "minimum_itcm_free_required_bytes": 512,
        "previous_hardware_dsp_readings_are_not_new_build_results": True,
        "known_hardware_tests_outstanding": [
            "boot/rollback", "original_six_voices", "saved_kit_compatibility",
            "two_overlapping_toms", "four_pad_stress",
            "CPU_DSP_PK_LATE_RAIL", "step1_LED", "steps2_to16_toggle",
            "original_mono_stereo_routing", "dub_delay_send_and_tail",
            "external_sync_dropout", "wavetable_OSC_UI",
        ],
        "created_at_utc": datetime.now(timezone.utc).isoformat(),
    }
    output.mkdir(parents=True, exist_ok=True)
    (output / "MANIFEST.json").write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    (output / "SHA256SUMS.txt").write_text(
        manifest["image_sha256"] + "  " + image.name + "\n",
        encoding="utf-8")
    (output / "HARDWARE_UNVERIFIED.txt").write_text(
        f"JIMBONES v{version} EXPERIMENTAL -- NOT HARDWARE VERIFIED\n"
        "Do not flash without a verified working rollback image, backed-up SD card "
        "and independent review of all stacked draft PRs.\n"
        "CI verifies the built update format and memory. It cannot prove audio timing, "
        "voice safety, or absence of clipping/crunch under pad performance.\n",
        encoding="utf-8")
    return manifest


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--image", type=Path, required=True)
    p.add_argument("--binary", type=Path, required=True)
    p.add_argument("--build-log", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--sha", required=True)
    p.add_argument("--workflow-run", required=True)
    p.add_argument("--branch", required=True)
    p.add_argument("--version", default="1.8.04")
    a = p.parse_args()
    m = make_manifest(a.image, a.binary, a.build_log, a.output,
                      a.sha, a.workflow_run, a.branch, a.version)
    print("PASS JB-23 candidate preflight")
    print("Image: " + m["image_filename"])
    print("Commit: " + m["source_commit_sha"])
    print("SHA256: " + m["image_sha256"])
    print(f"ITCM: {m['itcm_free_bytes']} bytes free")
    print("HARDWARE TEST STATUS: UNVERIFIED")


if __name__ == "__main__":
    main()
