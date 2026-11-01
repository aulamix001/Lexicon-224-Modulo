# DSP validation and approximation register

The current implementation is an original, purpose-built approximation inspired by the supplied Lexicon 224 V3 brief. It does **not** use `juce::Reverb`, Freeverb or a stock reverb object. It also does **not** contain recovered 224 firmware, verified original coefficients or a converter schematic. A custom network alone is not evidence of fidelity to hardware.

## Evidence and limits

- The supplied text is the functional source for program names, parameter ranges, recommended settings, broad onset/diffusion descriptions, subprogram behavior and A/C/B/D routing.
- The supplied panel image informs the editor, not DSP coefficients.
- No hardware impulse responses or dry/processed recordings were supplied (confirmed by the user). No hardware A/B comparison was performed.
- Requests to the Lexicon and Universal Audio documentation sites were denied by the environment's egress policy. Domain additions were saved to the environment configuration draft; that save does not itself grant runtime access. No unconsulted document is cited as proof.
- JUCE 7.0.12, commit `4f43011b96eb0636104cb3e433894cda98243626`, supplies plugin integration and UI, not the reverb algorithm.

## Implemented signal behavior

Input highpass → optional mono bridge/chamber sum → independently crossfaded stereo predelay → program-specific sparse reflections → program-specific allpass diffusion → eight frequency-dependent delay branches with unit-energy orthogonal scattering and nested allpass recirculation → four decoded wet outputs. Gain calculation accounts for the branch and diffusion delays; bass/mid split and treble damping remain inside the loop. Random coefficient steps have deterministic seeds and 50 ms slew. Opposing fractional read-head offsets create subtle time variation. The nonlinear decay envelope and transient detection reduce early feedback without multiplying loop gain above unity. Presets and converter changes crossfade prepared engines over 35 ms.

The diffusion/feedback topology, tap values and output decoding are designed hypotheses. They were not obtained from the actual 224 microcode. Halls, plates and chamber have distinct timing, initial diffusion, early-energy weighting, decay envelopes and routing, but share this implementation's tank structure. That shared topology and lack of hardware calibration remain material fidelity limits.

## Converter approximations

| Profile | Implemented | Unverified aspects |
| --- | --- | --- |
| 16-bit arithmetic (default) | Tank-write and output rounding, host-rate processing | The brief's 16-bit assumption is not a verified original ADC/DAC specification; arithmetic scaling and rounding locations are approximated |
| Experimental clock/converter | 20 kHz internal clock, interpolated input event sampling, held DAC output, six-pole 8 kHz filters, 12-bit quantization with four binary gain ranges, 16-bit tank writes | Clock rate, gain-ranging thresholds/settling, mantissa/exponent coding, converter linearity, filter pole positions and actual hardware reconstruction are not established |
| Clean reference | Same network with floating-point delay storage | Useful for quantization comparisons, not a hardware model |

No arbitrary hiss, random dither, clock jitter, transformer model or invented component tolerances are added. Input dBm calibration, analog frequency/phase response, converter differential/integral nonlinearity, exact floating-point conversion and overflow arithmetic await authoritative data. The mild output soft saturation and bounded internal buses deliberately avoid wraparound; they cannot reproduce the original machine's overload exactly. A static Q=2–4 resonant coloration model was not added without a measured frequency target; coloration currently arises from the designed delay network.

## Completed checks

- Linux Release VST3 build using JUCE 7.0.12.
- 72 program/rate/converter impulse cases: all six programs, 44.1/48/88.2/96 kHz and all three converter profiles, nonzero stereo tails and finite outputs.
- Longer BASS/MID settings increase late energy; lowering TREBLE DECAY reduces high-frequency tail energy.
- All programs/profiles remain finite under maximum decay/brightness and sustained overlevel test input.
- Cold-start inherent predelay has no zero-delay impulse leakage. Delay automation crossfades preserve amplitude bounds.
- Chamber cancels antiphase stereo input by mono averaging; plates obey A=D and C=B.
- Quantization errors obey the defined profile's bounds; the experimental 8 kHz lowpass rejects 16 kHz energy by more than 30 dB relative to 1 kHz.
- Exact silence without excitation; no added noise generator.
- Processor tests: dry polarity/amplitude, blocks of 1/17/127/512/1024 samples, factory recall, all 36 registers through state serialization, empty-register behavior, wet energy, program changes, converter switching at all supported rates, mono/quad negotiation and nonzero auxiliary returns.
- Rootless cloud setup was exercised again: existing dependencies reused, Linux build and both CTest executables passed.

A Windows/MSVC build, Windows DAW scan/automation/listening tests, GUI interaction checks and Steinberg validator/pluginval have not run here. The Windows GitHub Actions workflow is supplied but has not been executed in this session.

## Measured implementation responses

Run: 48 kHz, 12 seconds, 0.5 FS single-sample impulse in LEFT only, default program settings. Each number is from this implementation, **not the hardware**. T20 fits the Schroeder energy decay between −5 and −25 dB and extrapolates to a nominal RT60. Nonuniform decay, frequency-dependent damping and quantization make this different from the displayed BASS/MID times. Native-clock filtering adds small group delay; the measured onset threshold is −100 dBFS.

| Program | Default-profile onset (ms) | Default-profile broadband T20→RT60 (s) | Clean 125 Hz T20→RT60 (s) | Clean 2 kHz T20→RT60 (s) | Default-profile stereo correlation |
| --- | ---: | ---: | ---: | ---: | ---: |
| Small Concert Hall B | 20.000 | 1.830 | 2.956 | 1.548 | -0.0046 |
| Vocal Plate | 20.000 | 1.421 | 2.198 | 1.822 | 0.3297 |
| Large Concert Hall B | 20.000 | 2.061 | 3.296 | 1.937 | -0.0134 |
| Acoustic Chamber | 25.000 | 2.101 | 3.425 | 1.843 | -0.0014 |
| Percussion Plate A | 0.000 | 1.363 | 1.847 | 1.825 | 0.3231 |
| Small Concert Hall A | 20.000 | 1.673 | 2.967 | 1.540 | -0.0112 |

All measured quantized responses reached digital silence by the final second of the 12-second capture; the analysis reports −300 dBFS for zero RMS. This is a numerical floor, not a claim of a −300 dB hardware noise floor. The deterministic converter model lacks the original machine's analog noise.

Raw metrics, including C80, T30 and regression R², are in [response-metrics.json](response-metrics.json). Regenerate WAVs with `render_responses` and metrics with `scripts/analyze-responses.py`. Captures are build outputs and can be regenerated rather than checked into Git.

## Corrections driven by measurement

Initial measurement exposed impulse leakage from the predelay's zero-delay crossfade head at startup. Initialization now sets both heads to the actual inherent/preset delay. An onset regression test covers this. Measured low-band decays were too long because diffusion delay was omitted from feedback timing; including the allpass-chain delay brought the clean Small Concert Hall B 125 Hz extrapolation from about 5.04 s to about 2.96 s for a nominal 3.0 s BASS setting. This is internal calibration to the parameter target, not calibration to a real 224.

## What would improve fidelity next

Obtain authoritative algorithm/service documentation or program ROM analysis with provenance, then replace hypothesized topology, taps, modulation sequence and arithmetic with documented behavior. Measure a real V3.2 unit with mode/optimization both off and on: stereo/mono impulses, multiple input levels, sustained tones, decay sweeps, noise floor, frequency response and ADC/DAC transfer near range transitions. Record firmware revision, routing, input calibration and every parameter. The analysis tool accepts matching hardware captures via `--reference`; absent those recordings, no numerical hardware similarity score is reported.
