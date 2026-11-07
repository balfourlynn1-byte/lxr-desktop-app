# JIMBONES v1.8.00 Intel listening bench

Unzip the Mac Test Bench and open JIMBONES.app. It is unsigned. If Gatekeeper asks, review the download and use System Settings → Privacy & Security → Open Anyway. Running a test uses /usr/bin/python3 from Apple Command Line Tools. The WAVs in the audio package play directly without installing tools.

Click **Restore aggressive preset**, then **Render and play my mix**. The eight sliders apply to all six experimental voices for that audition. The regular test buttons always use their documented fixed settings. **View Results** opens a fresh run's WAVs, MIDI and results.json under ~/Library/Application Support/JIMBONES/Results. The Play Experimental Audio button plays the reference. Compare Approved-v1799.wav and Aggressive-reference.wav at equal playback volume. Full-before and Full-after compare the same source MIDI at maximum faders. Simultaneous files isolate 1, 2, 3, 4 and 6 voices with equal-onset hits, velocity changes, pitched retriggers and overlapping long layer decays.

## Front panel and MIDI

Select the voice. First OSC press opens the legacy oscillator page. Press OSC again to enter the extension. Repeated OSC presses advance four controls at a time. Performance page 3 still provides the Step FX slot, rate, probability and voice mask. The encoder selects a voice and clicking toggles inclusion. The eighth OSC bank adds per-voice repeat amount independently of the mask.

| OSC bank | NRPN fine numbers, left to right | Panel controls |
|---|---|---|
| 1 | 0–3 | Mode, Metallic level, Wavetable level, amplitude decay |
| 2 | 4–7 | Metal, Noise, Aggression, Texture |
| 3 | 8–11 | Velocity to Metal, Noise, Aggression and Texture |
| 4 | 12–15 | Carrier pitch, ratio, FM depth, feedback |
| 5 | 16–19 | Detune, noise level, noise colour, transient length |
| 6 | 20–23 | Drive, phase shape/saturation character, bit reduction, velocity sensitivity |
| 7 | 24–27 | Wavetable position, movement, randomisation, pitch envelope |
| 8 | 28–31 | Wet blend, WaveShape amount, Step FX amount, velocity to blend |

The first seven banks were panel-editable in 1.7.99. The old bench fixed the mode and layer count, decay 127, six repeat destinations, slot 8, faders 40% and test MIDI. Its separate WaveShape demonstration fixed character 110 and aggression 78. The approved aggressive reference uses the original default character and aggression. The earlier firmware had no dedicated external MIDI addresses for these layer controls. The new slider controls send the production MIDI parser the same NRPN messages available on hardware.

On the voice or global MIDI channel with CC reception enabled, send CC99 = 96 + zero-based voice, CC98 = parameter number, CC6 = value. E.g. Metallic level 80 on Tom 1 (second drum voice) is CC99=97, CC98=1, CC6=80. Mode accepts 0–3. Other controls accept 0–127. Send an explicit CC99 when switching NRPN banks. Step FX slot, rate, probability and mask remain panel controls. The audio package includes Aggressive-reference-settings.mid for channels 1–6; reproducing the full sound also requires the same original kit, faders, velocity modulation, pitches, routing and Step FX slot 8 with all six voices included. The app restores all of these in its deterministic performance. The settings MIDI programs synthesis controls only.

Blend scales the added layer and its pre-filter noise. Zero restores original dry gain. WaveShape 0 removes wavetable phase warp and folding; 64 reproduces the previous algorithm; higher settings increase both. Step FX amount scales the velocity of generated hits and zero suppresses them. Sound may be nonlinear with velocity, particularly on snare noise. Velocity to blend is neutral at 64. Above centre, soft hits have less wet signal; below centre, hard hits have less. Old version-1 kit extensions load with neutral new controls. Stock kits start with the optional layers disabled.

## Gain and timing

At high faders, an armed voice reserves summing headroom according to layer levels and blend. At reference level or higher, squared fader gains above 0.161 increase at 4% of the former slope. The curve depends on knob positions and precedes voice summation. Bypassed original voices retain the stock fader law and can still clip when summed at maximum. Zeroing the layer levels or blend leaves any separately enabled Step FX repeats in place.

The report records the largest adjacent stereo-channel step and its location. Compare that location in the matched reference before judging a pitched attack. Hash identity checks every sample, including sharp attacks. Hardware ISR/deadline problems cannot be diagnosed by listening to desktop WAVs alone. Performance page 4 shows STM32 current/peak render load, a 64-block exponential average, overdue renders, pending DMA requests and rail/knee counters. Turn the encoder to reach per-voice peak cycles for the layer-render and gain-staging calls. Click to reset peaks. Pre-filter noise, native voice DSP and non-audio MIDI/UI work are outside that per-layer interval. Mac mean_us and peak_us include host scheduling. No physical STM32 measurements were collected.

To rebuild on Intel macOS, install Apple Command Line Tools, CMake and Ninja. From this branch run bash tests/mac/build.sh /path/to/Erica /path/to/NoClip /path/to/packages /path/to/v1.7.99. The workflow pins each source revision. This release distributes the listening bench, audio and diagnostics. Hardware review remains open.
