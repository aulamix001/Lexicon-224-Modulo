# Lexicon224 — algorithmic reverb

C++17 / JUCE **7.0.12**, VST3 effect for Windows 10/11 x64. Original time-domain DSP inspired by the supplied Model 224 V3 description. This is a perceptual approximation, not recovered Lexicon firmware or a measured/bit-exact hardware emulation. No external DSP libraries, FFT or circuit simulation.

## Windows build

Install Visual Studio 2022 with Desktop development with C++, v143 and Windows SDK 10.0.19041 or newer; CMake 3.22+ and Git. From this checkout:

```powershell
cmake -S Lexicon224 -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

CMake fetches JUCE 7.0.12. To use an existing JUCE checkout, add `-DJUCE_PATH=C:/path/to/JUCE` to configure. JUCE includes the VST3 SDK. Output: `build/Lexicon224_artefacts/Release/VST3/Lexicon224.vst3`. Copy the complete bundle to `C:\Program Files\Common Files\VST3` and rescan in the DAW. No installer or standalone target is generated. Check JUCE's licensing terms before distribution.

## Cloud / Linux development

This task uses Debian 13 without administrative permissions. `bash scripts/bootstrap-cloud.sh` installs signed Debian development packages into `/workspace/tools/sysroot`, pins JUCE in `/workspace/deps/JUCE`, builds the Linux VST3 and runs tests. Package downloads use the session proxy with signature/checksum and TLS verification. These tools and libraries live outside the project checkout. The script is specific to this cloud image, not a Windows prerequisite.

For a normal Linux workstation, install CMake, Ninja, pkg-config, ALSA, FreeType and X11 development packages, then configure with `cmake -S Lexicon224 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`. The Linux binary cannot load in a Windows DAW. A Windows/MSVC build and a DAW listening/automation session remain separate platform checks.

Portable DSP tests need only C++17 and CMake:

```sh
cmake -S Lexicon224 -B build-dsp -DLEXICON_DSP_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-dsp --parallel 2
ctest --test-dir build-dsp --output-on-failure
```

## Panel and operation

- Programs 1–6: Small Concert Hall B, Vocal Plate, Large Concert Hall B, Acoustic Chamber, Percussion Plate A, Small Concert Hall A. CALL recalls the full recommended state (including wet mix, input mode and default converter profile); IMMED activates current fader positions. With CALL disabled, selecting a program changes the algorithm while preserving controls.
- Programs 7/8 toggle Mode Enhancement and Decay Optimization. Their rate/amount controls are below the hardware-style panel.
- After factory/register recall, faders use soft takeover: move across the preset position to engage. The display buttons' LEDs indicate engaged faders. IMMED engages all faders. Host automation remains active regardless of pickup.
- Drag a fader or hold its label button to see its value; the resting display shows the BASS/MID average. Red input meters indicate -24/-18/-12/-6/0 dBFS; E indicates internal overrange. Internal saturation bounds the wet output.
- SET toggles storage mode. A–D stores/recalls four quick registers. SHIFT then A–D chooses an extended bank; 1–8 stores/recalls one of its eight slots. FUNCTION returns to normal operation. Empty slots leave audio/state unchanged. All 36 slots persist in the DAW project state.
- Dry/Wet defaults to 100% wet; dry is unfiltered and polarity preserving. MONO BRIDGE averages input L/R before the engine; a mono host bus duplicates its input. Chamber always averages internally. Main outputs are A/C. Enable optional stereo B/D auxiliary output in the host for quad returns; plate routing is A=D and C=B. Auxiliary returns are wet-only and follow the wet level.
- Factory presets are exposed through the processor's six VST programs. Register slots live in plugin state; standalone `.vstpreset` file import/export relies on host facilities.

## DSP and interpretation choices

Stereo crossfeed, 20 Hz input highpass, stationary-read-head predelay crossfade, eight program-specific early taps, four input allpasses and an eight-branch dual-band tank per side, with orthogonal Walsh–Hadamard feedback scattering and four allpasses inside each feedback branch. Each program has its own delay and reflection timing tables; the engine does not use JUCE Reverb or a third-party reverb. Feedback gains use `10^(-3*d/RT60)`. One-pole coefficients use the stable exponential form `1-exp(-2*pi*f/fs)` rather than the unstable high-frequency Euler approximation in the brief. Feedback-time calculations include diffusion-path delay. Coefficients are double precision. Parameters are smoothed; algorithm changes use 35 ms wet-engine crossfades without allocating or resetting delay memory on the audio thread.

The brief gives inconsistent predelay conventions. This implementation interprets the fader as total nominal delay, clamps it to the selected program's limits, and adds the described stereo offset (hall nominal 24 ms → L 27 / R 20). Plates use equal nominal delays; chamber uses nominal L / R+7. Host parameter range remains 0–255 ms. The crossover uses a continuous skewed range so the specified 540 Hz factory settings can be represented; this differs from the also-requested 100 Hz steps below 1 kHz. DEPTH, mode rate and optimization amount use integer steps.

Program tap timings, diffusion and envelope behavior are designed approximations because exact coefficients/taps and hardware impulse responses were not supplied. Mode enhancement uses deterministic pseudo-random coefficient steps with 50 ms smoothing and small opposing fractional delay excursions; optimization uses fast/slow envelope detection. Long tails remain bounded, including chamber settings that could overflow original hardware. The UI adapts the reference to fit registers and plugin-only controls (535×960), with a 14 px charcoal brushed-gradient frame, 6 px corner radius and four 8 px screws. Program buttons use a single LED rather than the hardware's three-LED CALL indication.

## Converter profiles and acoustic validation

The additional CONVERTER MODEL selector has three profiles, all prepared in advance and crossfaded in real time:

1. **16-bit arithmetic (brief)** — default; fixed-step quantization of tank writes and wet outputs at host sample rate, following the supplied brief's arithmetic assumption. This is not a claim about the actual ADC/DAC word lengths.
2. **Experimental 20 kHz / 12-bit ranging** — hypothesis only: a 20 kHz processing clock, six-pole 8 kHz Butterworth anti-alias/reconstruction filters, sample interpolation and sample/hold, four instantaneous gain ranges with a 12-bit quantizer, and 16-bit tank storage. Neither gain thresholds, converter coding, filter topology nor clock rate have been verified against hardware documentation in this task. There is no added arbitrary noise or dither. This profile must not be represented as a verified original-converter emulation.
3. **Clean reference** — floating-point tank for quantization-error comparisons, with the same designed reverb network.

Generate 18 stereo, 12-second, 48 kHz float WAV responses (0.5 FS impulse in left input):

```sh
./build/render_responses build/responses
python scripts/analyze-responses.py build/responses --output docs/response-metrics.json
```

The offline analysis script requires NumPy and SciPy and measures onset, energy, C80, stereo correlation, broadband/octave-band Schroeder T20/T30 extrapolations and residual tail level. Offline analysis is separate from the time-domain audio engine. `--reference path/to/hardware-recordings` compares the same descriptors with matching hardware WAV filenames; hardware references must have the same excitation amplitude and documented settings. Quantized/noisy tails and nonlinear decays can bias Schroeder fits; inspect the reported R² and curves before interpreting extrapolated RT60. No hardware recordings were available, so the checked-in metrics characterize this implementation only. See [DSP validation and approximation register](docs/DSP-VALIDATION.md).

## Validation scope

`dsp_tests` exercises all 6 programs and all 3 converter profiles at 44.1/48/88.2/96 kHz (72 impulse cases), decay-time response, extreme settings, chamber mono cancellation, plate quad pairing predelay onset/changes, converter quantization bounds and anti-alias rejection. `plugin_tests` exercises dry transparency, varied blocks, factory recall, register persistence, wet output, preset switching converter switching at every supported rate, mono/quad bus negotiation and audible auxiliary-return energy. Automated tests do not establish acoustic equivalence or replace auditioning in a Windows DAW.

Windows CI is defined in `.github/workflows/windows.yml`; it is not evidence of a Windows build until that workflow runs successfully.

## Floating preset browser

Click **PRESETS** above the panel display to open/close the independent browser beside the editor. It opens to the right when screen space allows and to the left otherwise, stays within the screen and can be moved using its header. Closing/hiding the plugin editor also hides the browser. The layout follows the supplied reference: title and close icon, search, TYPE/ARTIST/ALL tabs, 12 tag buttons, favourites, current selection, DELETE/RENAME/SAVE and a scrolling alphabetical list. Artist metadata was not supplied, so ARTIST displays an explicit empty state. Display labels **Best Of** and **Modulation** map to the catalog's original **Best off** and **modulacao** tags.

The compiled catalog currently contains 55 named screenshot presets. Unnamed reference photos are kept in `presets/catalog.json` but excluded from the named browser list. Select a row, then press Enter or double-click to apply its mapping. Screenshot mappings are **partial**: only a confirmed, supported program and nonconflicting modifier toggles are applied. Existing fader values, mix, modulation amounts and converter settings are retained. Conflicting modifier observations are skipped; missing/unsupported algorithms make no audio change. The browser reports the outcome. Photo-derived presets are not presented as exact full-state reproductions.

The header favourites star filters the list; the selection star toggles a preset's favourite state. SAVE creates a complete user snapshot of current plugin parameters, using the currently selected tag. DELETE/RENAME apply to user snapshots only; screenshot references remain protected. User presets and favourites persist per user in `AulaMix/Lexicon224/presets.json` under JUCE's user application-data directory. They are independent of the 36 DAW-persisted register slots. Changes to that local library are not automatically synchronized across machines or plugin instances already open.

Browser/library tests cover catalogue loading, search/tag/favourite filtering, favourites persistence, partial mappings preserving unknown faders, unsupported algorithms preserving the audio program, conflicting modifiers, factory protection and user preset save/restore/rename/delete. `build/render_browser build/preset-browser.png` renders an offline component snapshot for inspection. Native floating-window placement, focus and scaling still require testing in Windows DAWs; the screenshot renderer is not a substitute for those host checks.

## Ready-to-run Windows build command

After installing Visual Studio 2022 (Desktop development with C++, v143 and Windows SDK), CMake and Git, run `build-windows.cmd` from this checkout. The script configures an x64 Release VST3, builds it, runs tests and creates `dist/Lexicon224-Windows-x64-VST3.zip`. Copy the complete generated `Lexicon224.vst3` bundle into `C:\Program Files\Common Files\VST3` and rescan the DAW. The build script has been prepared on Linux and has not been executed on Windows in this session.

## v0.1.1 panel layout (supersedes earlier UI descriptions)

The editor uses a proportional 506 x 865 layout with 8 program buttons, 8 function buttons, 6 vertical faders and 6 lower adjustment buttons. OPEN replaces PRESETS and opens the same floating preset browser. The extra SET/CALL/SHIFT/FUNCTION/register/mix/amount/converter widgets are removed from the face; existing host parameters and stored registers remain available in processor state. Lower pairs adjust diffusion, enhancement rate and optimization amount. Mix arrows adjust wet mix; SOLO selects a wet-only return. Initial default program is index 2 (Large Concert Hall B), BASS 3.0 / MID 2.6, showing 2.8 sec. The fader response curves have explicit visual anchors matching the requested starting handle coordinates; host parameter units remain unchanged.

Room A / Const Plate are still absent from the DSP and their program buttons do not switch to an invented algorithm. SYS NOISE is unimplemented, and rear buses must be configured in the host. Those two buttons expose availability information, not fake audio processing. IMMED indicates direct fader control; GUI faders update parameters directly. The paper texture, connector and bezels are procedural approximations, not photographic copies. DejaVu fonts are embedded under the included license. JUCE's transient startup branding remains enabled; the offline steady-state screenshot omits that transient only. The renderer now captures the plugin panel and checks the 23-button (including OPEN), six-fader, four-lit-LED initial structure.
