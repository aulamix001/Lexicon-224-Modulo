# Lexicon224 — Windows x64 VST3 preview

Download `Lexicon224-Windows-x64-VST3.zip`, extract it and copy the complete `Lexicon224.vst3` folder into `C:\Program Files\Common Files\VST3`. Rescan plugins in your DAW.

This preview includes an original six-program algorithmic reverb and the floating preset browser. It is an approximation, not a verified emulation of Lexicon hardware. Screenshot preset mappings are partial: exact fader values are unavailable, conflicting modifiers are preserved as pending, and Room A / Const Plate are not implemented. Complete user preset snapshots can be saved locally. See README.md and docs/DSP-VALIDATION.md for details.

The workflow publishes this asset only after compiling with Visual Studio 2022 and passing the DSP and processor/library tests. DAW compatibility and listening tests are still required.

## v0.1.1 panel redesign

506 x 865 proportional panel, eight program buttons on one row, eight hardware-style function buttons, six faders and six lower adjustment buttons. OPEN opens the floating preset browser. The initial program is Large Concert Hall B, with 2.8 sec on the resting display. Embedded DejaVu fonts make rendering consistent across machines; the font license is included in the bundle. Room A / Const Plate and system noise remain unimplemented; their controls do not impersonate another algorithm. The new diffusion parameter controls the input allpass coefficients.
