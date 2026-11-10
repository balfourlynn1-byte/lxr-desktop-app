"""Unit tests for host-only PCM diagnostics (synthetic fixtures)."""
import tempfile
import unittest
from array import array
from pathlib import Path
from analyze_pcm import analyze

class PCMTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
    def tearDown(self):
        self.tmp.cleanup()
    def pcm(self, values):
        import sys
        a = array("h", values)
        if sys.byteorder != "little":
            a.byteswap()
        p = Path(self.tmp.name) / "test.pcm"
        p.write_bytes(a.tobytes())
        return p
    def test_silence(self):
        result = analyze(self.pcm([0]*4))
        self.assertEqual((result["samples"], result["peak_absolute"], result["near_rail_samples"]), (4, 0, 0))
        self.assertIsNone(result["peak_dbfs"])
    def test_rails(self):
        result = analyze(self.pcm([0,32760,32767,-32768,-32760,1]))
        self.assertEqual(result["near_rail_samples"], 4)
        self.assertEqual(result["exact_endpoint_samples"], 2)
        self.assertEqual(result["near_rail_longest_run"], 4)
    def test_odd_length(self):
        p = Path(self.tmp.name) / "bad.pcm"
        p.write_bytes(b"123")
        with self.assertRaises(ValueError):
            analyze(p)
    def test_empty(self):
        with self.assertRaises(ValueError):
            analyze(self.pcm([]))

if __name__ == "__main__":
    unittest.main()
