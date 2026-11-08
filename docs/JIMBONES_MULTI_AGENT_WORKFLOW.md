# JIMBONES multi-agent engineering workflow

This is a role-based coordination system, not a claim that autonomous agents are currently running. Each agent owns a GitHub issue and proposes changes through a dedicated branch/PR. GitHub Actions is the build/test gate. No agent may merge, flash hardware, or overwrite the known-working v1.8.02.

## Roles and handoff
- **Coordinator**: triage, priority, dependencies, acceptance criteria and release gate.
- **DSP**: algorithms, audio quality, CPU budgets, voice isolation.
- **Embedded**: STM32F765, real-time safety, memory, MIDI, UI and persistence.
- **Verification**: host fixtures, stress tests, ARM compile, checksums, reproducibility.
- **Research**: reference implementations, licensing, hardware limitations; record source links and avoid copying incompatible code.
- **Integration**: resolves conflicts, checks six-voice behavior, prepares experimental image.

## Definition of done for each task
1. Issue has scope, baseline, test strategy and CPU/memory risk.
2. Research agent records references and license compatibility.
3. Implementing agent submits focused PR with code and tests.
4. Verification agent runs host regression, multi-voice stress, ARM compilation and image validation.
5. Integration agent reviews memory report, regression evidence and rollback path.
6. Hardware-dependent behavior stays UNVERIFIED until user tests on LXR-02.
7. No feature enters release unless coordinator explicitly accepts it.

## Priorities
P0: ARM/release parity, CPU monitoring, multi-voice modulation crunch.
P1: morph A/B/C and modulation; tape/tube drive; USB multitrack feasibility; step performance FX.
P2: metallic FM; wavetable and interpolation; hi-hat expression; velocity response; per-hit randomisation; dual/comb filters; UI navigation.
All tasks retain six original voices and protect kick from performance effects when requested.

## GitHub implementation
- Track every task in an issue tagged with a virtual agent role in the title/body.
- One feature branch per task, branched from a verified integration baseline.
- Pull requests must run `JIMBONES v1.8.03 ARM test build`.
- Build success is NOT hardware success. Do not describe a firmware image as bug-free.
- An agent is a responsibility/automation contract; running parallel AI workers requires an external runner with credentials, permissions and execution limits.
