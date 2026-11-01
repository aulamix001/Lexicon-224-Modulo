# Lexicon224 — VST3 Windows x64

C++17 / JUCE 7.0.12 algorithmic effect with a hardware-style 506×865 scalable editor and an independent floating preset browser. This is a purpose-built approximation inspired by the supplied references, not recovered Lexicon firmware or verified emulation of the photographed plugin.

## Download and install

Download `Lexicon224-Windows-x64-VST3.zip` from the repository's **v0.2.3** release. Extract and copy the complete `Lexicon224.vst3` folder to `C:\Program Files\Common Files\VST3`. Replace an older version rather than keeping duplicate copies in different plugin folders. Restart the DAW and rescan VST3 plugins. The binary uses the Microsoft Visual C++ x64 runtime. Source archives are not installable plugins.

## The 55 presets

All **55 named photo presets** are now executable, deterministic reconstructed states, available both through the host's program list and the **OPEN** floating organizer. They are classified into **12 tags**. Repeated photographs across categories are references to the same preset, not duplicates. Twelve cropped/unnamed photos remain supporting evidence and are not additional presets.

Click a name in the organizer or select a program in the host to load it. Search filters names/tags; a tag filters the list; the favourites star filters/toggles favourites. SAVE captures an exact full state of this plugin as a user preset; RENAME/DELETE apply to user presets only. User presets/favourites persist in `AulaMix/Lexicon224/presets.json` under JUCE's user application-data directory. The floating window is draggable, opens beside the plugin within screen bounds and hides when the editor closes. Artist metadata was not supplied, so ARTIST has an explicit empty state. Display labels Best Of and Modulation map to the original catalog tags Best off and modulacao.

### Reconstruction accuracy

The raw photographic observations are retained in `presets/catalog.json`. The separate `visual_reconstruction` field contains complete estimated parameter states. `scripts/reconstruct-photo-presets.py` regenerates these reproducibly. Six photographed handle positions are mapped through documented host parameter curves with an estimated 10% margin at each end. Hidden controls use explicit defaults; predelay follows algorithm limits. Where LEDs conflict across photos, the first named reference is used for reconstruction and the disagreement remains documented.

**All 55 load; exact equivalence to the original presets is unverified.** Their original numeric values, slider transfer functions, hidden controls and firmware coefficients were not supplied. The two all-program-LED references are interpreted as chorus, consistent with the panel's printed shortcut and one preset name, but that interpretation remains an estimate. A reference's raw `loadable: false` means its original settings are not fully verified; `visual_reconstruction.loadable_with_current_dsp` describes the executable reconstructed bank.

## Algorithms and controls

The algorithm selector contains eight panel programs: Small Concert Hall B, Vocal Plate, Large Concert Hall B, Acoustic Chamber, Percussion Plate A, Small Concert Hall A, Room A and Constant Plate. A ninth chorus mode is accessed by chorus presets or Shift+OPEN, indicated by all eight program LEDs. The eight numbered keys select algorithms, while the DAW program list selects the 55 named reconstructed presets. These are separate concepts.

Room A uses a compact eight-branch tank with its own early taps. Constant Plate uses a dense plate profile with a uniform feedback envelope. Chorus uses three phase-offset interpolated delay voices per channel with stereo crossfeed and tone filtering. These three additions are designed approximations, not claimed original topologies.

Faders automate BASS, MID, CROSSOVER, TREBLE DECAY, DEPTH and PRE-DELAY. Lower pairs adjust input diffusion, mode enhancement rate and decay optimization amount. Dry/wet arrows adjust mix; SOLO toggles a wet-only return. Mode enhancement and decay optimization toggle independently. SYS NOISE audio is not modeled; rear buses must be configured in the host. Their photographed LED states and IMMED are restored as panel metadata, not claims that the associated hardware circuitry has been emulated. The decorative paper, connector and beveled controls are procedural. Fonts are embedded with their license in the bundle. JUCE's transient startup branding remains enabled.

Main outputs A/C are stereo; optional B/D returns are enabled through host bus settings. Mono host inputs are supported, and chamber internally averages stereo. Plates pair A=D and C=B. The dry path preserves amplitude/polarity. Feedback is bounded; converter profiles include 16-bit arithmetic, an experimental 20 kHz gain-ranging model and a clean floating-point reference. Converter specifications are hypotheses; see `docs/DSP-VALIDATION.md` for the original six-program characterization and approximation register.

## Build

Windows: install Visual Studio 2022 Desktop development with C++, v143, Windows SDK, CMake 3.22+ and Git, then run `build-windows.cmd`. Alternatively:

```powershell
cmake -S Lexicon224 -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Output: `build/Lexicon224_artefacts/Release/VST3/Lexicon224.vst3`. CMake fetches JUCE 7.0.12 or accepts `-DJUCE_PATH=...`. JUCE licensing applies to distribution. The GitHub workflow publishes after build and tests succeed.

Cloud Debian 13: `bash scripts/bootstrap-cloud.sh` installs verified packages rootlessly into `/workspace/tools/sysroot`, tools under `/workspace/tools/bin`, and JUCE under `/workspace/deps/JUCE`, then builds/tests. It preserves proxy, TLS and package verification. Linux VST3 binaries cannot load in Windows DAWs.

## Validation

- DSP suite: nine algorithms × four sample rates × three converter profiles (108 impulse cases), extreme stability, band-dependent decay, tone damping, predelay, mono/quad behavior and quantizers.
- Processor/library suite: all 55 named host and browser presets, full parameter mapping, repeatable recall independent of earlier settings, state persistence, favourites, tags/search and user preset CRUD. Audio from all presets is checked for finite samples.
- Panel suite: 8+8+6 buttons plus OPEN, six faders, initial positions and four lit LEDs. Windows additionally opens and closes an actual native floating preset window.

The tests establish implemented behavior, not identity with a hardware unit or compatibility with every DAW. No original hardware recordings were supplied. `render_browser` generates an offline panel screenshot; `render_responses` and `scripts/analyze-responses.py` support additional acoustic characterization. Noises, converter characteristics and unsupported hardware details are not asserted to be exact.
