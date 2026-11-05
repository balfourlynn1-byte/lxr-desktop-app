# PROJECT JIMBONES
## Complete Project Vision, Layered Architecture and Development Roadmap

**Platform:** Erica Synths LXR-02 Desktop Drum Synthesizer  
**Firmware source:** https://github.com/erica-synths/lxr-desktop-app  
**Project name:** JIMBONES  
**Working experimental firmware:** v1.7.90  
**Next target:** v1.7.91  
**Development approach:** Incremental, modular and performance-first

---

# 1. The Overall Vision

JIMBONES is a long-term custom firmware project designed to transform the Erica Synths LXR-02 into a much more expressive, experimental and performance-oriented electronic drum synthesizer.

It is being developed primarily for **live drumming with physical drum pads**, rather than conventional step sequencing alone.

The intention is to combine:

- The existing LXR-02 synthesis engines
- Additional metallic FM synthesis
- Additional wavetable synthesis
- Per-voice analogue-style saturation
- Advanced velocity modulation
- Morphable sound states
- Controlled sound randomisation
- Tempo-synchronised performance effects
- Improved expressive MIDI control
- Expanded filtering
- Potential multichannel USB audio

These features should operate together as a coherent instrument.

**The fundamental principle is that new capabilities must be layered on top of the original firmware, not replace or compromise it.**

The LXR's existing synthesis, interface, sequencing, routing, presets and MIDI functions should remain available unless a specific change has been deliberately designed, implemented and tested.

# 2. The Layered Architecture

JIMBONES should be developed around several distinct functional layers.

Each layer must have clearly defined responsibilities, independent enable/disable states where appropriate, and predictable interactions with the others.

## LAYER 0 — Original LXR-02 Sound Engine

This is the foundation of the entire project.

Preserve the six original voices:

1. Kick
2. Snare
3. Tom 1
4. Tom 2
5. Clap
6. Hi-hat

Preserve their existing synthesis parameters, envelopes, modulation, filters, routing, MIDI assignments and preset behaviour.

JIMBONES should be capable of behaving like a completely standard LXR-02 when all additional features are disabled.

**This layer must never be removed.**

The user must be able to return any voice to its original sound without loading different firmware.

## LAYER 1 — Optional Additional Synthesis Engines

The first major development milestone.

Each of the six voices should support two new optional synthesis layers:

**A. Metallic FM Engine**

A dedicated synthesis engine for metallic, inharmonic, resonant, noisy and aggressive percussion sounds.

Examples include:

- Metallic snares
- Bells and tuned percussion
- Industrial kicks
- Distorted toms
- Metallic claps
- Synthetic cymbals and hi-hats
- Complex evolving percussion

The engine should support frequency modulation, selectable frequency relationships, feedback, transient shaping, noise, and timbral evolution.

**B. Wavetable Engine**

A complementary synthesis engine providing alternative harmonic structures, evolving timbres and more complex oscillator textures.

Potential features:

- Selectable wavetable
- Wavetable position
- Scanning and interpolation
- Modulation of wavetable position
- Velocity-dependent wavetable movement
- Controlled randomisation
- Interaction with envelopes and LFOs

Reuse existing waveform resources wherever technically appropriate.

### Per-voice engine combinations

All six voices must offer the following choices:

| Mode | Original | Metallic FM | Wavetable |
|---|---|---|---|
| Original | ON | OFF | OFF |
| Metallic | ON | ON | OFF |
| Wavetable | ON | OFF | ON |
| Combined | ON | ON | ON |

This includes the kick.

Each new engine needs independent level control and activation.

For example:

- Kick: Original + subtle Wavetable
- Snare: Original + Metallic FM
- Tom 1: Original + Wavetable
- Tom 2: Original + Metallic FM + Wavetable
- Clap: Original + Metallic FM
- Hi-hat: Original only

All additional engines must be **OFF by default**.

Each voice must retain its own independent settings.

### Metallic macro interface

The initial main control page should expose four high-level macro parameters:

**METAL**  
Controls metallic character through FM ratios, modulation intensity, resonance and inharmonicity.

**NOISE**  
Controls noise generation, noise colour, transient texture and noise-envelope behaviour.

**AGGRESSION**  
Controls drive, feedback, saturation, digital distortion and other aggressive characteristics.

**TEXTURE**  
Controls timbral movement, wavetable activity, instability and evolving sound complexity.

Additional pages should expose the deeper parameters behind these macros.

Macro controls should remain musically useful throughout their ranges and avoid unintentionally modifying unrelated properties.

### Interface

Pressing OSC twice should provide access to the additional synthesis pages, subject to the existing button and menu architecture.

Preserve the original oscillator page and existing user-interface conventions.

## LAYER 2 — Expressive Velocity and Modulation

JIMBONES is intended to respond like a playable percussion instrument.

Velocity should do more than control volume.

Every voice should have configurable velocity routing to parameters including:

- Pitch
- Envelope decay
- Metallic FM amount
- Wavetable position
- Noise amount
- Distortion/saturation
- Filter cutoff
- Morph position
- Transient character

Each of the four main Metallic macros should have its own velocity modulation amount.

For example:

**Soft snare hit:** clean, short and relatively dry.  
**Medium hit:** additional metallic harmonics.  
**Hard hit:** pronounced FM, noisier transient and more saturation.

These transitions should feel natural.

Support selectable velocity curves, including more sensitive response at low velocities and stronger expression at high velocities.

Avoid abrupt timbral discontinuities unless specifically selected.

Maintain reliable tracking during rapid rolls and repeated hits of different velocities.

## LAYER 3 — Advanced Morphing and Sound States

One of the most important long-term features.

Every voice should support three sound states:

**A — Original / Protected State**  
The primary sound snapshot.

**B — Alternative Sound State**  
A second independently editable version.

**C — Experimental Sound State**  
A third state that can contain more extreme synthesis or randomised parameters.

The system should allow smooth movement between sound states.

### A/B/C behaviour

- Store independent synthesis parameters for each state.
- Allow state A to be protected from mutation.
- Allow B and C to be edited without modifying A.
- Provide clear visual feedback indicating which state is being edited.
- Editing at an intermediate morph position must modify the intended target state, not silently overwrite another state.
- Allow selected voices to be excluded from morphing.
- Provide controllable interpolation between compatible parameters.
- Define predictable switching behaviour for discrete engine selections.

### Velocity-to-morph

Velocity should be capable of moving a voice through its sound states.

Example:

- Soft snare hit: primarily A
- Medium hit: blend between A and B
- Hard hit: move towards B or C

This should enable genuinely expressive acoustic-style performance dynamics using electronic synthesis.

### Voice Mutator

Add controlled randomisation of B and C states.

The user should be able to mutate sounds while keeping A protected.

The mutation system should support strength, selected parameter groups, and voice inclusion/exclusion.

The goal is to generate new sound variations without losing the original sound.

### Morph modulation

Morph position should also be controllable through:

- LFO
- Velocity
- MIDI
- Manual controls
- Performance steps
- Potential future modulation sources

Different voices should be able to morph independently.

## LAYER 4 — Per-voice Tape, Tube and Drive Processing

Introduce a new saturation engine inspired by the musical behaviour of analogue tape and tube circuits.

The reference concept is the kind of warmth and harmonic enhancement associated with devices such as Elektron Analog Heat +FX.

This should not simply be a hard clipper.

Desired characteristics:

- Smooth harmonic saturation
- Warmth and density
- Soft transient compression
- Controlled high-frequency fuzz
- Preserved low-frequency weight
- Adjustable drive intensity
- Musical response to velocity

The effect should be usable **per voice**, not solely as a global bus effect.

A kick should be able to remain clean while the snare and toms become saturated, or vice versa.

Ideally, velocity should control drive independently of overall volume.

This feature should integrate with the A/B/C morph system, allowing saturation characteristics to change between snapshots.

Research numerical stability, aliasing and CPU efficiency before selecting the final DSP implementation.

## LAYER 5 — Expanded Filters

Extend the existing voice-filtering possibilities.

### Secondary bass filter

Investigate an additional independently controllable low-pass or high-pass filter, allowing more flexible shaping of low-frequency content.

Example: remove unwanted high-frequency harshness from a kick without losing the deeper bass.

### Comb filter

Introduce an optional comb-filter mode for resonant, metallic and physical-model-style effects.

Potential uses:

- Resonant metallic percussion
- Short tuned delays
- Hollow snare textures
- Synthetic cymbals
- Karplus–Strong-style sounds

Investigate integrating this into the existing FILTER pages, with additional presses accessing secondary filter controls.

Maintain the original filter modes.

## LAYER 6 — Step-based Performance Effects

Create an additional live performance system accessible from the LXR's step buttons.

This system is intended for expressive, momentary or latched effects during live drumming.

Examples:

- Trigger delay
- Repeated hits
- Randomised retriggering
- Rhythmic stutters
- Tempo-synchronised glitch patterns
- Morph modulation
- Temporary timbral mutation
- Rhythmic sound variation

The system should support synchronisation with incoming MIDI clock and the internal tempo.

Proposed pattern lengths include:

- 2 bars
- 4 bars
- 8 bars

The exact timing resolution should be flexible, potentially including 1/2-beat and 1/4-beat intervals.

### Performance controls

The step buttons should be capable of enabling or disabling selected performance effects.

A preliminary concept is:

- Step 1: all performance effects off
- Step 16: morph LFO effect

The remaining mappings should be established during interface design.

These are design ideas, not fixed button assignments that override the existing sequencer.

### Voice inclusion/exclusion

Every performance effect should be able to include or exclude specific voices.

A common live setup might exclude the kick from stutters and randomisation so the underlying rhythm remains stable.

However, the kick should not be permanently restricted. The user should retain control over inclusion.

The system must preserve timing accuracy and avoid stuck or duplicate notes.

## LAYER 7 — Controlled Randomisation

Develop a performance-oriented randomisation system.

This should allow individual drum hits to vary in character without destroying the underlying musical groove.

Potential options:

- Randomly select between defined sound states
- Randomly vary specified synthesis parameters
- Adjust probability of applying variation
- Restrict the amount of randomisation
- Keep certain voices or parameters protected

Example:

A snare retains a reliable backbeat, but selected hits occasionally introduce more metallic character, altered noise or another transient texture.

Randomisation should be deterministic when seeded, if practical, to support reproducible testing.

It should also be possible to disable it at kit level.

## LAYER 8 — Improved Hi-hat Articulation

The hi-hat should become more expressive for live playing.

The current hardware setup uses a Roland FD-8 pedal with Ableton MIDI routing for open/closed articulation.

Potential improvements:

- Better open/closed transitions
- Predictable choking
- Adjustable choking response
- More natural decay control
- Velocity-dependent timbral changes
- Metallic FM and Wavetable layers following choke behaviour
- Better MIDI compatibility with pedal-derived articulation

New synthesis layers must respect the existing hi-hat voice and release logic.

Do not create uncontrolled overlapping hi-hat sounds.

## LAYER 9 — MIDI Expression and Drum-pad Integration

The instrument is played with:

- Alesis Performance Pad
- Roland KT-10 kick pedal
- Roland FD-8 hi-hat controller
- Ableton Live 12
- An Intel Mac

The user frequently plays multiple voices simultaneously and performs rapid rolls with drumsticks.

Important goals:

- Reliable note-on and note-off handling
- Accurate velocity response
- No unintended retrigger gaps
- Low and consistent latency
- Reliable MIDI clock synchronisation
- Configurable velocity curves
- Assignable expressive MIDI modulation
- Improved control of hi-hat articulation

Investigate how the existing voice triggering and retrigger behaviour can be improved without introducing artifacts or changing existing kit behaviour.

### Future Sensory Percussion integration

The user may eventually use Sensory Percussion v1 with a mesh-head drum.

Potential future requirements:

- Different sounds assigned to playing zones
- Velocity and zone-dependent layering
- Snare/clap blending
- Dynamic sound changes across playing positions
- Zone position as a modulation source

This is a longer-term possibility, not a requirement for v1.7.91.

## LAYER 10 — USB Multichannel Audio Research

A longer-term ambition is to investigate whether the LXR-02 can provide low-latency multichannel USB audio to a Mac, similar in concept to Elektron Overbridge.

The preferred initial routing concept is:

| USB channel | Sound source |
|---|---|
| 1 | Kick |
| 2 | Tom 1 + Tom 2 |
| 3 | Snare + Clap |
| 4 | Hi-hat |

Additional stereo FX routing or alternative assignments may be investigated.

However, **USB audio feasibility must not be assumed**.

Research the STM32F765 USB interfaces, hardware connections, clocking, audio buffering, USB endpoint bandwidth, firmware resource limitations and compatibility with USB MIDI.

Consider whether four channels means four mono output channels, how stereo processing would be handled, and whether MIDI/audio could operate together.

Useful research references include community projects involving USB audio on embedded synthesizers and drum machines.

Do not compromise existing MIDI or audio operation in pursuit of this feature.

This is initially a research project with a go/no-go feasibility decision, not a promised deliverable.

## LAYER 11 — Expanded Performance Display

Investigate additional visual feedback for live playing.

Ideas include:

- A waveform display
- Voice activity indicators
- Current morph position
- Active synthesis layers
- Performance effects status
- Modulation activity

A proposed future interface could provide multiple waveform display styles.

This is explicitly lower priority than audio reliability.

Visualisation must not interfere with interrupt timing, audio synthesis or responsiveness.

## LAYER 12 — Mac Testing and Development Tools

Provide a repeatable testing workflow for macOS, preferably compatible with Intel Macs.

The development environment should support:

- Compiling firmware
- Host-side DSP simulation
- Rendering test audio
- Automated regression testing
- MIDI event simulation
- Comparing original and modified sound output
- Detecting clipping and non-finite samples
- Measuring performance and memory usage
- Preparing validated firmware update images

A future companion utility could offer a simple interface for selecting builds, reviewing diagnostics and assisting with backup/restore.

A Max for Live companion may also be explored later for deeper modulation, MIDI control and performance integration.

These are supporting tools, not prerequisites for the first firmware build.

---

# 3. Development Priorities and Versions

Do not attempt to deliver every feature in one firmware release.

The architecture should anticipate the complete project, but implementation should be incremental.

## Phase 0 — Baseline and Compatibility

Establish a clean build from the repository.

Audit:

- Audio callback and synthesis architecture
- Voice memory layout
- Oscillator and wavetable implementations
- Parameter storage
- Preset serialisation
- MIDI and voice triggering
- OSC and FILTER page handling
- DSP performance constraints
- Build output and firmware packaging

The user has already installed and tested an experimental firmware labelled `v1.7.90`, which successfully booted and responded to live drumming.

However, do not assume the current source repository includes the exact modifications that produced that binary.

Preserve the original v1.7.90 image as the user's fallback.

## Phase 1 — JIMBONES v1.7.91

**Priority: Highest**

Deliver the first genuinely integrated additional-synthesis firmware.

Targets:

- Metallic FM layer
- Wavetable layer
- Independent optional engines for all six voices
- Original sound retained
- Independent layer levels
- Four macro controls
- Accessible deeper synthesis parameters
- Per-macro velocity modulation
- Hardware UI integration
- Safe preset compatibility
- Proven build and automated tests

If any of these cannot be implemented reliably, clearly document the omissions rather than claiming full completion.

This release is primarily about establishing the extended synthesis architecture correctly.

## Phase 2 — JIMBONES v1.7.92 and Later

**Priority: High**

Develop the expressive morph system:

- A/B/C sound snapshots
- Protected A state
- Independent B/C editing
- Smooth morphing
- Voice exclusion
- Velocity-to-morph
- LFO-to-morph
- Controlled state mutation

## Phase 3 — Analogue-style Processing and Filters

**Priority: High/Medium**

Develop:

- Per-voice tape/tube saturation
- Velocity-dependent drive
- Expanded bass filtering
- Comb filter
- Additional sound-design options

## Phase 4 — Live Performance Systems

**Priority: Medium**

Develop:

- Step-based performance FX
- Rhythmic retriggering
- Controlled per-hit randomisation
- Clock-synchronised patterns
- Performance effect voice exclusions
- Improved hi-hat expression

## Phase 5 — Advanced Integration

**Priority: Research/Long-term**

Investigate:

- Multichannel USB audio
- Mac testing utility
- Max for Live companion
- Sensory Percussion zone control
- Expanded graphical displays

These features should only progress once their feasibility and resource costs are understood.

---

# 4. Important Design Principles

## Preserve the existing instrument

If all JIMBONES additions are disabled, the LXR-02 should sound and behave like its original firmware.

## Modularity

New synthesis engines, effects and morphing systems should be separated into well-defined modules.

Avoid building one large, tightly coupled DSP implementation.

## Per-voice independence

Every voice should eventually support its own:

- Active synthesis engines
- Layer levels
- Velocity modulation
- Morph behaviour
- Saturation settings
- Filter settings
- Randomisation
- Performance effect participation

## Live performance first

This instrument must work when someone is playing complex rhythms with drumsticks and pedals.

Avoid solutions that are only reliable with slow, quantised MIDI sequencing.

## Resource awareness

The STM32F765 has finite processing and memory resources.

Measure the real cost of additional DSP.

Do not assume every advanced feature can run concurrently on every voice.

Consider shared processing, precomputation, bounded polyphony, quality modes or other optimisations where necessary.

Maintain stable audio processing and preserve a safe performance margin.

## Backward compatibility

Existing presets should remain compatible.

New synthesis settings must not corrupt old kits.

Use explicit version handling and safe defaults when extending preset data.

## Honest testing

Distinguish between:

- Conceptual design
- Desktop prototype
- Host-side test passed
- Embedded compilation passed
- Firmware image structurally validated
- Physical hardware tested

Do not conflate these stages.

---

# 5. What Success Looks Like

The long-term result should be an LXR-02 that can move from clean original drum synthesis to complex experimental timbres, dynamically and musically.

A single snare could start as a familiar electronic drum, develop metallic FM harmonics as it is played harder, introduce wavetable texture, and progressively morph into a distorted experimental percussion sound.

Meanwhile, the kick could remain completely original—or have its own carefully blended additional engines.

At any moment, the performer should be able to control how much experimentation is happening without losing the underlying rhythmic performance.

This is not intended to become a completely different synthesizer.

**It is intended to become a substantially more expressive version of the LXR-02.**

---

# 6. Instructions to Codex

Use this document as the **master architectural context and long-term development roadmap**.

Do not treat every future feature as part of the immediate release.

Begin with Phase 0 and Phase 1.

Implement modular foundations that will support future morphing, effects and modulation without unnecessary rework.

Prioritise producing a testable, correctly built **JIMBONES v1.7.91**.

Make real source-code changes, build the firmware, run available tests, report the results and clearly identify what remains unverified.

Create incremental commits and work on a separate development branch.

Never replace the user's known-working v1.7.90 firmware image.

Where future requirements are ambiguous, prefer extensible designs that preserve existing behaviour and keep unsupported features disabled until properly implemented.