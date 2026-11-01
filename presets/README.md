# Visual preset catalog

User-supplied screenshots are grouped by their supplied tag. `catalog.json` preserves raw visual evidence separately from estimated playable reconstructions. `loadable: false` describes the unverified original transcription. The explicit `visual_reconstruction` field now defines estimated states for all 55 named references, with documented numeric conversion and hidden-control defaults. These reconstructions are not confirmed original states. Room A, Constant Plate and chorus now have distinct approximate implementations. Historical batch notes below describe the state when the photos were catalogued; references to absent algorithms are superseded by v0.2.0.

## Vocal — batch 01

| Preset | Program LED | Display / unit | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Ballad Vocals | 2 — Vocal Plate | 3.4 sec | On | On | Off | Off | On |
| Ballad Vox Plate | 5 — Percussion Plate A | 2.2 sec | On | Off | On | Off | On |
| Brooklyn BVs | 2 — Vocal Plate | 2.8 sec | On | On | On | Off | On |
| Smooth Vocal Hall | 3 — Large Concert Hall B | 3.4 sec | On | On | On | On | On |

A single illuminated program LED determines the program mapping, regardless of preset name. Multiple illuminated program LEDs must be recorded without assigning a unique algorithm. In particular, **Ballad Vox Plate selects program 5**, not program 2. Catalog `plugin_index` and confirmed parameter `program` use zero-based indices, whereas `panel_number` is one-based.

The visible numeric display is preserved literally. Its currently displayed parameter is not identified in these screenshots; it must not be substituted for MID, BASS or their mean. Fader positions in the JSON are rough geometric estimates along the visible slots, with zero at bottom and one at top. They are not calibrated parameter values, JUCE normalized values, milliseconds, seconds or hertz. Photographs alone do not establish the source plugin's nonlinear slider mappings.

Mode enhancement and decay optimization LEDs map to the existing `mode` and `optimize` parameters. Their rate/amount values are not visible. No mix, diffusion or converter setting is inferred. SYS NOISE, REAR OUTS and SOLO are recorded as visible LED states only: the current plugin does not implement equivalent controls, and REAR OUTS must not silently be treated as enabling the host's quad bus.

All six numeric fader values remain unknown. Exact reconstruction needs a source-plugin preset export or views showing individual parameter values, plus any hidden diffusion, modifier, mix and routing settings. Until supplied, retain these entries as incomplete references. Original screenshot files are not present in the checkout; `image_ordinal` identifies the order in the user's Vocal message.

## Vocal — batch 02

Tag inherited from the preceding Vocal batch. Screenshot ordinals refer to the five images in this second message.

| Preset | Program LED(s) | Display / unit | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Smooth Vox Hall | 1 — Small Concert Hall B | 2.2 sec | On | Off | On | Off | On |
| Thicker Voc Plate | 5 — Percussion Plate A | 1.8 sec | On | On | On | Off | On |
| Vocal Slap | All eight — algorithm pending | 3.0 sec | On | Off | On | Off | On |
| Vocal Wide Chorus | All eight — algorithm pending | 4.6 sec | On | Off | On | Off | On |
| Vox Plate | 2 — Vocal Plate | 2.6 sec | On | On | On | Off | On |

**Smooth Vox Hall** is a separate preset from batch 01's **Smooth Vocal Hall**: their names, selected programs and visible settings differ. For **Vocal Slap** and **Vocal Wide Chorus**, the program identifiers are null and are omitted from `confirmed_plugin_parameters`. All eight LEDs are visibly lit; the precise special mode remains unconfirmed. The panel's printed chorus shortcut and the preset names alone do not establish its algorithm or numerical settings.

## Best off — batch 01

Tag spelling preserved exactly as provided by the user.

| Preset | Program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO | Images |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| A Little Bit Of Hall | 1 — Small Concert Hall B | 3.0 sec | On | On | Off | Off | Off | On | 1 |
| Atmosphere | 6 — Small Concert Hall A | 6.5 sec | On | On | On | On | Off | On | 2 |
| Ballad Vocals | 2 — Vocal Plate | 3.4 sec | On | On | Off | Off | Off | On | 3 |
| Big 80s Snare | 6 — Small Concert Hall A | 1.7 sec | On | Off | Off | On | On | On | 4, 5 |

Ballad Vocals matches the earlier Vocal reference in its visible settings and receives the additional Best off tag without duplicating the preset. Both Big 80s Snare images have the same visible settings and are retained as two source references for one entry. Hidden parameter equality cannot be established from screenshots. All entries remain incomplete, with exact numeric fader values unknown.

## Best off — batch 02

Tag inherited from the preceding Best off batch.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| DI's Drum Room | 7 — Room A | 1.8 sec | On | Off | Off | On | On |
| Echo Robo Voc | 4 — Acoustic Chamber | 3.4 sec | On | On | On | Off | On |
| Heavy Echo | 8 — Const Plate | 1.1 sec | On | Off | On | Off | On |
| In the name of Lex | 5 — Percussion Plate A | 1.8 sec | On | Off | On | Off | On |
| Lingering Hall | 3 — Large Concert Hall B | 4.6 sec | On | On | Off | On | On |

The source panel has eight algorithm buttons, including Room A and Const Plate. These two algorithms are absent from the current plugin's six-program bank. Their catalog plugin indices remain null and their `program` parameter is omitted from the confirmed mapping. Do not map source algorithm buttons 7/8 to this plugin's mode-enhancement/decay-optimization toggles. This is a source/target capability mismatch, not an ambiguous LED reading. IMMED is off in all five images. Exact fader values remain unknown.

## Best off — batch 03

Tag inherited from preceding Best off batches.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Live Room | 7 — Room A (unsupported) | 1.3 sec | On | Off | On | Off | On |
| Lush Piano | 6 — Small Concert Hall A | 6.5 sec | On | On | On | Off | On |
| Vocal Wide Chorus | All eight LEDs — algorithm pending | 4.6 sec | On | Off | On | Off | On |
| Vox Plate | 2 — Vocal Plate | 2.6 sec | On | On | On | Off | On |

Vocal Wide Chorus and Vox Plate match their earlier Vocal screenshots in visible settings and receive Best off as an additional tag, with new source references. Hidden values remain unconfirmed. Live Room's Room A is not implemented in the current DSP; its target program index remains null. IMMED is off in all four images. The Live Room preset title is partly clipped by the top image edge but remains readable. Numeric fader values remain unknown.

## Hall — batch 01

| Preset | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| A Little Bit Of Hall | 1 — Small Concert Hall B | 3.0 sec | On | On | Off | Off | Off | On |
| Atmosphere | 6 — Small Concert Hall A | 6.5 sec | On | On | On | On | Off | On |
| Default | 6 — Small Concert Hall A | 2.4 sec | On | On | Off | Off | Off | On |
| Horn Hall | 6 — Small Concert Hall A | 3.4 sec | On | Off | Off | On | Off | On |
| Lingering Hall | 3 — Large Concert Hall B | 4.6 sec | On | On | Off | Off | On | On |

A Little Bit Of Hall, Atmosphere and Lingering Hall receive Hall as an additional tag and retain the previous Best off references. Visible settings match within estimation tolerance; hidden values are not confirmed. Default and Horn Hall are new catalog entries. The source preset named Default selects program 6; this is a screenshot preset name, not a request to change this project's default program. All six exact fader values remain unknown.

## Hall — batch 02

Tag inherited from the preceding Hall batch. All six images select source program 6, Small Concert Hall A.

| Preset | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Loose Hall | 2.4 sec | On | Off | Off | On | Off | On |
| Lush Piano | 6.5 sec | On | On | Off | On | Off | On |
| Percussive Slap | 0.6 sec | On | Off | On | On | Off | On |
| Plateau Piano | 10.0 sec | On | Off | Off | On | Off | On |
| Room 4 Growth | 2.8 sec | On | On | Off | Off | Off | On |
| Shuffle Slap | 0.8 sec | Off | Off | Off | On | Off | On |

Lush Piano receives the additional Hall tag and a new source reference without duplicating the existing Best off entry. The other five presets are new. Shuffle Slap has both modifier LEDs off. Display text is transcribed literally; it is not assigned to any individual parameter. Exact fader values, including the minimum-looking controls in Percussive Slap, remain unknown.

## Hall — batch 03

Tag inherited from preceding Hall batches.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Slap Hall | 3 — Large Concert Hall B | 5.2 sec | On | On | Off | On | On |
| Smooth Vocal Hall | 3 — Large Concert Hall B | 3.4 sec | On | On | On | On | On |
| Smooth Vox Hall | 1 — Small Concert Hall B | 2.2 sec | On | Off | On | Off | On |
| Wider-1 | 6 — Small Concert Hall A | 0.6 sec | On | Off | Off | Off | On |

Smooth Vocal Hall and Smooth Vox Hall receive Hall as an additional tag with new source references. They remain distinct presets because their names, programs and visible settings differ. Slap Hall and Wider-1 are new entries. IMMED is off in all four screenshots. Values in the display are transcribed without assigning them to individual parameters; numeric fader values remain unknown.

## Plate — batch 01

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 80s Snare | 5 — Percussion Plate A | 0.6 sec | Off | Off | On | Off | On |
| 90s Big Kick | 5 — Percussion Plate A | 1.5 sec | On | Off | On | Off | On |
| Ballad Vocals | 2 — Vocal Plate | 3.4 sec | On | On | Off | Off | On |
| Ballad Vox Plate | 5 — Percussion Plate A | 2.2 sec | On | Off | On | Off | On |
| Brooklyn BVs | 2 — Vocal Plate | 2.8 sec | On | On | On | Off | On |

80s Snare and 90s Big Kick are new entries. 80s Snare remains separate from Big 80s Snare: their names, programs, display and modifier/routing states differ. The three existing vocal presets receive Plate as an additional tag with new source references. IMMED is off throughout this batch. Exact numeric fader values remain unknown; the display is transcribed without assigning it to a specific parameter.

## Plate — batch 02

Tag inherited from the preceding Plate batch.

| Preset | Program visible in this image | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Density Plate | 8 — Const Plate (unsupported) | 2.8 sec | On | Off | On | Off | On |
| Drum Ballad Plate | 5 — Percussion Plate A | 2.8 sec | On | Off | On | Off | On |
| Fat Drum Plate | 5 — Percussion Plate A | 3.0 sec | On | Off | On | Off | On |
| Heavy Echo | 8 — Const Plate (unsupported) | 1.1 sec | On | Off | On | Off | On |
| In the name of Lex | All program LEDs appear off; see discrepancy | 1.8 sec | On | Off | On | Off | On |

The first three presets are new. Heavy Echo and In the name of Lex receive the additional Plate tag and retain separate image references. In the name of Lex previously showed program 5 in Best-off-02 image 4; this batch's image 5 appears to show no lit program LEDs. The earlier program assignment is retained with explicit conflicting evidence, and reconciliation is pending. No replacement algorithm is inferred. Density Plate and Heavy Echo use Const Plate, which is not implemented by the current DSP. IMMED is off throughout this batch. Exact fader values remain unknown.

## Plate — batch 03

Tag inherited from preceding Plate batches. Names preserve the screenshot spelling, including Marv Conga Plate and Mexicon Snare Plate.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Marv Conga Plate | 5 — Percussion Plate A | 2.8 sec | On | On | On | Off | On |
| Mexicon Snare Plate | 5 — Percussion Plate A | 1.3 sec | On | On | Off | Off | On |
| Mid Tempo Snare | 5 — Percussion Plate A | 1.5 sec | On | Off | On | Off | On |
| Midnight Plate | 8 — Const Plate (unsupported) | 3.0 sec | On | Off | Off | Off | On |
| Mountains of Needles | 5 — Percussion Plate A | 4.6 sec | On | Off | On | Off | On |
| Percussion Plate | 5 — Percussion Plate A | 5.7 sec | On | On | On | Off | On |

All six entries are new. Midnight Plate has no target algorithm mapping because Const Plate is absent from the current DSP. IMMED is off in every image. The display is transcribed literally without assigning it to a specific parameter. Fader positions remain approximate geometric observations; exact parameter values are unknown, including apparent endpoint settings.

## modulacao — batch 01

Tag spelling preserved as supplied by the user. The tag is an organizational category, not evidence of a specific chorus mode or modulation amount.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Drum Crunch Room | 7 — Room A (unsupported) | 0.6 sec | On | Off | On | Off | On |
| Echo Robo Voc | 4 — Acoustic Chamber | 3.4 sec | On | On | On | Off | On |
| Indie Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | On | On | On | On |
| Live Room | 7 — Room A (unsupported) | 1.3 sec | On | Off | On | Off | On |
| Loose Hall | 6 — Small Concert Hall A | 2.4 sec | On | Off | On | Off | On |

Drum Crunch Room and Indie Drum Room are new. The other three entries receive modulacao as an additional tag and retain separate image references. Room A remains absent from the current DSP, so those entries have no target program index. IMMED is off in all five images. Display readings do not establish individual parameter values; exact fader values and modulation rate remain pending.

## modulacao — batch 02

Tag inherited from the preceding modulacao batch. Preset spelling preserves MidTempo Snare Room as shown in the image; it is distinct from Mid Tempo Snare in Plate.

| Preset | Source program | Display / unit | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| MidTempo Snare Room | 7 — Room A (unsupported) | 1.4 sec | On | On | On | On | On |
| Mountains of Needles | 5 — Percussion Plate A | 4.6 sec | On | Off | On | Off | On |
| Plateau Piano | 6 — Small Concert Hall A | 10.0 sec | On | Off | On | Off | On |
| Wider-1 | 6 — Small Concert Hall A | 0.6 sec | On | Off | Off | Off | On |
| Wider-2 | 7 — Room A (unsupported) | 0.6 sec | On | Off | Off | Off | On |
| Wider-3 | 7 — Room A (unsupported) | 10.9 kHz | On | Off | Off | Off | On |

MidTempo Snare Room, Wider-2 and Wider-3 are new. Mountains of Needles, Plateau Piano and Wider-1 receive the additional modulacao tag and new source references without duplication. Wider-3's illuminated unit is kHz, not sec. A frequency parameter is being displayed, but its identity is not confirmed; do not assign 10.9 to decay time or silently set TREBLE DECAY from this image. Room A is absent from the current DSP. IMMED is off in all six images. Exact numerical fader values remain pending.

## Long — batch 01

| Preset | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| A Little Bit Of Hall | 1 — Small Concert Hall B | 3.0 sec | On | On | Off | Off | Off | On |
| Atmosphere | 6 — Small Concert Hall A | 6.5 sec | On | On | On | On | Off | On |
| Ballad Vocals | 2 — Vocal Plate | 3.4 sec | On | On | Off | Off | Off | On |
| Chamber Made | 4 — Acoustic Chamber | 3.4 sec | On | On | Off | Off | On | On |

Chamber Made is new. The first three presets receive Long as an additional tag without duplication, with separate source references. The Long tag is the user's category and does not establish an exact decay time. Display readings remain unassigned to individual parameters, and exact fader values remain pending.

## Long — batch 02

Tag inherited from the preceding Long batch. All five entries already exist and receive Long as an additional tag with separate source references; none is duplicated.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Horn Hall | 6 — Small Concert Hall A | 3.4 sec | On | Off | On | Off | On |
| Lingering Hall | 3 — Large Concert Hall B | 4.6 sec | On | On | Off | On | On |
| Lush Piano | 6 — Small Concert Hall A | 6.5 sec | On | On | On | Off | On |
| Midnight Plate | 8 — Const Plate (unsupported) | 3.0 sec | On | Off | Off | Off | On |
| Mountains of Needles | 5 — Percussion Plate A | 4.6 sec | On | Off | On | Off | On |

Visible settings match previous screenshots within estimation tolerance. IMMED is off in every image. Midnight Plate remains without a target algorithm mapping because Const Plate is absent from the current DSP. Display readings are not assigned to individual parameters, and exact fader values remain pending.

## Long — batch 03

Tag inherited from preceding Long batches. All four presets already exist and receive the additional Long tag with separate image references.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Plateau Piano | 6 — Small Concert Hall A | 10.0 sec | On | Off | On | Off | On |
| Room 4 Growth | 6 — Small Concert Hall A | 2.8 sec | On | On | Off | Off | On |
| Slap Hall | 3 — Large Concert Hall B | 5.2 sec | On | On | Off | On | On |
| Smooth Vocal Hall | 3 — Large Concert Hall B | 3.4 sec | On | On | On | On | On |

Visible settings match prior references within estimation tolerance. IMMED is off in all four screenshots. Smooth Vocal Hall remains distinct from Smooth Vox Hall. Display readings remain unassigned to specific parameters; exact fader values are unknown. No duplicate preset entries are created.

## Dark — batch 01

| Preset | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 80s Snare | 5 — Percussion Plate A | 0.6 sec | Off | Off | Off | On | Off | On |
| 90s Big Kick | 5 — Percussion Plate A | 1.5 sec | On | Off | Off | On | Off | On |
| A Little Bit Of Hall | 1 — Small Concert Hall B | 3.0 sec | On | On | Off | Off | Off | On |
| Big 80s Snare | 6 — Small Concert Hall A | 1.7 sec | On | Off | Off | On | On | On |
| Dark Room | 7 — Room A (unsupported) | 1.4 sec | On | Off | On | On | Off | On |

Dark Room is new. The other four receive Dark as an additional tag with new source references and no duplicate entries. 80s Snare remains distinct from Big 80s Snare. Room A is absent from the current DSP, so Dark Room has no target program mapping. The Dark category does not establish any hidden frequency cutoff; display readings remain unassigned to specific parameters and exact fader values remain unknown.

## Dark — batch 02

Tag inherited from the preceding Dark batch.

| Preset / reference | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Fat Drum Plate | 5 — Percussion Plate A | 3.0 sec | On | Off | On | Off | On |
| Hard Chamber | 4 — Acoustic Chamber | 0.9 sec | On | Off | On | Off | On |
| Heavy Echo | 8 — Const Plate (unsupported) | 1.1 sec | On | Off | On | Off | On |
| Lingering Hall | 3 — Large Concert Hall B | 4.6 sec | On | On | Off | On | On |
| Unidentified — image 5 | 4 — Acoustic Chamber | 2.8 sec | On | Off | On | On | On |
| Midnight Plate | 8 — Const Plate (unsupported) | 3.0 sec | On | Off | Off | Off | On |

Hard Chamber is new; four existing presets receive the additional Dark tag without duplication. The fifth image has no visible title/header. It is stored with `name: null` and status `unidentified_visual_reference`; it must not count as a confirmed unique preset until identified and compared with existing entries. IMMED is off throughout this batch. Const Plate remains unsupported. Exact fader values remain pending.

## Dark — batch 03

Tag inherited from preceding Dark batches.

| Preset / reference | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Percussion Plate | 5 — Percussion Plate A | 5.7 sec | On | On | On | Off | On |
| Room 4 Growth | 6 — Small Concert Hall A | 2.8 sec | On | On | Off | Off | On |
| Slap Hall | 3 — Large Concert Hall B | 5.2 sec | On | On | Off | On | On |
| Unidentified — image 4 | 3 — Large Concert Hall B | 3.4 sec | On | On | On | On | On |

The first three existing presets receive Dark as an additional tag without duplication. The fourth image has no visible title. Its visible settings resemble Smooth Vocal Hall, recorded as an unconfirmed candidate match only. Until confirmed, retain it as an unidentified reference and do not count it as a confirmed unique preset or add Dark to the candidate's tags. The unnamed image from Dark-02 remains separately unresolved. IMMED is off throughout this batch. Exact fader values remain pending.

## Short — batch 01

| Preset | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 80s Fine Grained Gate | 4 — Acoustic Chamber | 0.6 sec | On | Off | On | On | Off | On |
| 80s Snare | 5 — Percussion Plate A | 0.6 sec | Off | Off | Off | On | Off | On |
| 90s Big Kick | 5 — Percussion Plate A | 1.5 sec | On | Off | Off | On | Off | On |
| Dark Room | 7 — Room A (unsupported) | 1.4 sec | On | Off | On | On | Off | On |
| Drum Crunch Room | 7 — Room A (unsupported) | 0.6 sec | On | Off | Off | On | Off | On |

80s Fine Grained Gate is new. The other four receive Short as an additional tag without duplication. The first preset's name suggests a gated effect but the screenshot does not establish its mechanism or any hidden gate settings. All six faders appear near the bottom; their exact values remain unknown. Room A is absent from the current DSP. Short is the user's organizational category and does not establish numeric decay times. Display readings remain unassigned to specific parameters. The two unnamed Dark references remain unresolved.

## Short — batch 02

Tag inherited from the preceding Short batch. All five presets already exist and receive Short as an additional tag, preserving their previous tags and image references.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Fat Drum Plate | 5 — Percussion Plate A | 3.0 sec | On | Off | On | Off | On |
| Hard Chamber | 4 — Acoustic Chamber | 0.9 sec | On | Off | On | Off | On |
| Heavy Echo | 8 — Const Plate (unsupported) | 1.1 sec | On | Off | On | Off | On |
| Live Room | 7 — Room A (unsupported) | 1.3 sec | On | Off | On | Off | On |
| Mexicon Snare Plate | 5 — Percussion Plate A | 1.3 sec | On | On | Off | Off | On |

Visible settings match previous references within estimation tolerance. IMMED is off throughout this batch. No duplicate preset entries are created. The Short category is retained exactly as supplied even when a display reads 3.0 sec; category membership does not determine a decay parameter. Room A and Const Plate remain absent from the current DSP. Exact numeric fader values and the two unnamed Dark references remain unresolved.

## Short — batch 03

Tag inherited from preceding Short batches.

| Preset / reference | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Unidentified — image 1 | 7 — Room A (unsupported) | 1.1 sec | On | On | Off | Off | Off | On |
| Unidentified — image 2; possible MidTempo Snare Room | 7 — Room A (unsupported) | 1.4 sec | On | On | Off | On | On | On |
| Percussion Plate | 5 — Percussion Plate A | 5.7 sec | On | On | Off | On | Off | On |
| Unidentified — image 4; possible Percussive Slap | 6 — Small Concert Hall A | 0.6 sec | On | Off | On | On | Off | On |
| Unidentified — image 5; possible Shuffle Slap | 6 — Small Concert Hall A | 0.8 sec | Off | Off | Off | On | Off | On |
| Snare Plate | 8 — Const Plate (unsupported) | 1.1 sec | On | Off | Off | On | Off | On |

Snare Plate is a new named entry. Percussion Plate receives Short as an additional tag without duplication. Four images have no visible preset titles and are stored as unidentified references; the candidate matches above remain unconfirmed and do not change the candidates' tags. Those references are not counted as confirmed unique presets. The two unnamed Dark references also remain unresolved. Room A and Const Plate are absent from the current DSP. Display readings remain unassigned to specific parameters and exact fader values remain unknown.

## Short — batch 04

Tag inherited from preceding Short batches. All named presets already exist and receive the additional Short tag. Two repeated Wider-3 images are retained as separate references to one entry.

| Preset / reference | Source program | Display | Mode enhancement in this batch | Decay optimization in this batch | Images |
| --- | --- | --- | --- | --- | --- |
| Vocal Slap | All eight LEDs — algorithm pending | 3.0 sec | On | Off | 1 |
| Wider-1 | 6 — Small Concert Hall A | 0.6 sec | On | On (differs from earlier Off) | 2 |
| Unidentified — possible Wider-2 | 7 — Room A (unsupported) | 0.6 sec | On | Off | 3 |
| Wider-3 | 7 — Room A (unsupported) | 10.9 kHz | Off (differs from earlier On) | Off | 4, 5 |

The third image lacks a visible title and remains an unidentified reference with an unconfirmed Wider-2 match. Wider-1's Decay Optimization LED and Wider-3's Mode Enhancement LED differ from previous references. The catalog preserves earlier mappings and explicitly flags conflicting source observations with per-image LED states; a canonical modifier state cannot yet be chosen. Do not silently load those mappings as settled presets. IMMED, SYS NOISE and REAR OUTS appear off in all five images; SOLO is on. Wider-3 retains the kHz unit, with the displayed parameter still unidentified. Exact numeric fader values remain pending.

## Room — batch 01

All five screenshots select source program 7, Room A, which is absent from the current six-program DSP. No target algorithm index is assigned.

| Preset | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Dark Room | 1.4 sec | On | Off | On | On | Off | On |
| DI's Drum Room | 1.8 sec | On | Off | Off | Off | On | On |
| DI's Room | 1.8 sec | On | Off | Off | On | On | On |
| Drum Crunch Room | 0.6 sec | On | Off | Off | On | Off | On |
| Indie Drum Room | 1.8 sec | On | On | Off | On | On | On |

DI's Room is new and remains distinct from DI's Drum Room: names, SYS NOISE state and visible fader positions differ. The other four receive Room as an additional tag without duplication. Display readings remain unassigned to individual parameters and exact fader values remain pending. The seven unidentified references from earlier batches remain unresolved.

## Room — batch 02

Tag inherited from preceding Room batch. All six images select program 7 — Room A (unsupported by the current DSP).

| Preset / reference | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- |
| Live Room | 1.3 sec | On | Off | On | Off | On |
| Mexicon Snare Room | 1.1 sec | On | On | Off | Off | On |
| Unidentified — possible MidTempo Snare Room | 1.4 sec | On | On | On | On | On |
| Unidentified — image 4 | 2.2 sec | On | Off | On | Off | On |
| Wider-2 | 0.6 sec | On | On (differs from prior Off) | Off | Off | On |
| Wider-3 | 10.9 kHz | On (differs from Short-04 Off) | Off | Off | Off | On |

Mexicon Snare Room is new and remains separate from Mexicon Snare Plate. Live Room, Wider-2 and Wider-3 receive Room as an additional tag. Images 3 and 4 lack titles and remain unidentified references. The cropped Short-03 image 1 now has Mexicon Snare Room as an unconfirmed candidate; no automatic merge is performed. Wider-2 and Wider-3 have source-specific conflicting modifier observations, preserved without overwriting earlier mappings. IMMED is off in all six screenshots. Exact fader values and missing titles remain pending.

## Drums — batch 01

All five presets already exist and receive Drums as an additional tag, preserving prior tags and source references.

| Preset | Source program | Display | Mode enhancement | Decay optimization in this batch | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 80s Snare | 5 — Percussion Plate A | 0.6 sec | Off | Off | On | Off | On |
| 90s Big Kick | 5 — Percussion Plate A | 1.5 sec | On | On (differs from earlier Off) | On | Off | On |
| Big 80s Snare | 6 — Small Concert Hall A | 1.7 sec | On | Off | On | On | On |
| DI's Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | Off | Off | On | On |
| Drum Ballad Plate | 5 — Percussion Plate A | 2.8 sec | On | Off | On | Off | On |

90s Big Kick shows Decay Optimization ON in this batch, while earlier screenshots show OFF. The catalog retains the earlier mapping with a conflict flag and the new per-image observation; no canonical modifier state is selected. IMMED is off throughout. No duplicate preset entries are created. Room A remains absent from the current DSP. Exact fader values and the nine unidentified references from prior batches remain pending.

## Drums — batch 02

Tag inherited from the preceding Drums batch.

| Preset / reference | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Drum Crunch Room | 7 — Room A (unsupported) | 0.6 sec | On | Off | On | Off | On |
| Unidentified — image 2; possible Fat Drum Plate | 5 — Percussion Plate A | 3.0 sec | On | Off | On | Off | On |
| Unidentified — image 3; possible Indie Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | On | On | On | On |
| Live Room | 7 — Room A (unsupported) | 1.3 sec | On | Off | On | Off | On |
| Mexicon Snare Plate | 5 — Percussion Plate A | 1.3 sec | On | On | Off | Off | On |

The three named presets receive Drums as an additional tag without duplication. Images 2 and 3 lack visible titles; they are stored as unidentified references with unconfirmed Fat Drum Plate and Indie Drum Room matches. These images do not add tags to the candidate entries or count as confirmed new presets. IMMED is off in every image. Room A remains absent from the current DSP, and exact fader values remain pending.

## Drums — batch 03

Tag inherited from preceding Drums batches.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Mexicon Snare Room | 7 — Room A (unsupported) | 1.1 sec | On | On | Off | Off | On |
| MidTempo Snare Room | 7 — Room A (unsupported) | 1.4 sec | On | On | On | On | On |
| Percussion Plate | 5 — Percussion Plate A | 5.7 sec | On | On | On | Off | On |
| Shock the Toms | 5 — Percussion Plate A | 2.2 sec | On | Off | On | Off | On |
| Snare Plate | 8 — Const Plate (unsupported) | 1.1 sec | On | Off | On | Off | On |

Shock the Toms is new. The other four receive Drums as an additional tag without duplication. IMMED is off throughout this batch. Named references do not by themselves confirm the identity of previously cropped photos; those remain pending. Room A and Const Plate remain absent from the current DSP. Exact numeric fader values remain unknown.

## Medium — batch 01

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Acoustic Solo Chamber | 4 — Acoustic Chamber | 1.8 sec | On | Off | On | Off | On |
| Ballad Vox Plate | 5 — Percussion Plate A | 2.2 sec | On | Off | On | Off | On |
| Big 80s Snare | 6 — Small Concert Hall A | 1.7 sec | On | Off | On | On | On |
| Brooklyn BVs | 2 — Vocal Plate | 2.8 sec | On | On | On | Off | On |
| Default | 6 — Small Concert Hall A | 2.4 sec | On | On | Off | Off | On |
| Density Plate | 8 — Const Plate (unsupported) | 2.8 sec | On | Off | On | Off | On |

Acoustic Solo Chamber is new. The other five receive Medium as an additional tag without duplication, with separate source references. IMMED is off throughout. Medium is the user's category and does not establish exact decay times. The display is transcribed without assigning it to a specific parameter. Const Plate remains absent from the current DSP. Exact numeric fader values and earlier unnamed references remain pending.

## Medium — batch 02

Tag inherited from preceding Medium batch.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| DI's Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | Off | Off | On | On |
| DI's Room | 7 — Room A (unsupported) | 1.8 sec | On | Off | On | On | On |
| Drum Ballad Plate | 5 — Percussion Plate A | 2.8 sec | On | Off | On | Off | On |
| In the name of Lex | 5 — Percussion Plate A | 1.8 sec | On | Off | On | Off | On |
| Indie Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | On | On | On | On |
| Loose Chamber | 4 — Acoustic Chamber | 2.8 sec | On | Off | On | On | On |

Loose Chamber is new; the other five receive Medium as an additional tag without duplication. Loose Chamber is now an unconfirmed candidate for the cropped Dark-02 image 5; no automatic identity merge is performed. In the name of Lex again shows program 5 lit, corroborating its original assignment while preserving the Plate-02 observation of no lit program LEDs. IMMED is off throughout. Room A is absent from the current DSP. Display readings remain unassigned to individual parameters and exact fader values remain pending.

## Medium — batch 03

Tag inherited from preceding Medium batches.

| Preset / reference | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Loose Hall | 6 — Small Concert Hall A | 2.4 sec | On | Off | On | Off | On |
| Marv Conga Plate | 5 — Percussion Plate A | 2.8 sec | On | On | On | Off | On |
| Mid Tempo Snare | 5 — Percussion Plate A | 1.5 sec | On | Off | On | Off | On |
| Moments in Lex | 7 — Room A (unsupported) | 2.2 sec | On | Off | On | Off | On |
| Shock the Toms | 5 — Percussion Plate A | 2.2 sec | On | Off | On | Off | On |
| Unidentified — possible Smooth Vox Hall | 1 — Small Concert Hall B | 2.2 sec | On | Off | On | Off | On |

Moments in Lex is new. Four existing named presets receive Medium as an additional tag without duplication. The sixth image lacks its title and is retained as an unidentified reference with an unconfirmed Smooth Vox Hall match. Moments in Lex is also a candidate for the cropped Room-02 image 4, without automatically merging that reference. Mid Tempo Snare remains distinct from MidTempo Snare Room. IMMED is off throughout this batch. Room A remains absent from the current DSP; display readings are unassigned to individual parameters and exact fader values remain pending.

## Medium — batch 04

Tag inherited from preceding Medium batches. Both presets already exist and receive Medium as an additional tag without duplication.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Thicker Voc Plate | 5 — Percussion Plate A | 1.8 sec | On | On | On | Off | On |
| Vox Plate | 2 — Vocal Plate | 2.6 sec | On | On | On | Off | On |

IMMED is off in both images. Visible settings match prior references within estimation tolerance. Exact numerical fader values remain unknown; display readings are not assigned to individual parameters. Earlier unidentified references remain pending.

## Bright — batch 01

All six presets already exist and receive Bright as an additional tag without duplication, preserving their prior categories and source references.

| Preset | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Acoustic Solo Chamber | 4 — Acoustic Chamber | 1.8 sec | On | Off | Off | On | Off | On |
| Atmosphere | 6 — Small Concert Hall A | 6.5 sec | On | On | On | On | Off | On |
| Ballad Vox Plate | 5 — Percussion Plate A | 2.2 sec | On | Off | Off | On | Off | On |
| Brooklyn BVs | 2 — Vocal Plate | 2.8 sec | On | On | Off | On | Off | On |
| Chamber Made | 4 — Acoustic Chamber | 3.4 sec | On | On | Off | Off | On | On |
| Default | 6 — Small Concert Hall A | 2.4 sec | On | On | Off | Off | Off | On |

Visible settings match earlier references within estimation tolerance, including the smaller screenshots. Bright is the user's organizational category, not proof of a particular treble cutoff or measured spectral response. Display readings remain unassigned to individual parameters and exact fader values remain pending. Earlier unidentified references and source-state discrepancies are unchanged.

## Bright — batch 02

Tag inherited from the preceding Bright batch. All six presets already exist and receive Bright as an additional tag without duplication.

| Preset | Source program | Display | Mode enhancement | Decay optimization | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Density Plate | 8 — Const Plate (unsupported) | 2.8 sec | On | Off | On | Off | On |
| DI's Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | Off | Off | On | On |
| DI's Room | 7 — Room A (unsupported) | 1.8 sec | On | Off | On | On | On |
| Drum Ballad Plate | 5 — Percussion Plate A | 2.8 sec | On | Off | On | Off | On |
| Echo Robo Voc | 4 — Acoustic Chamber | 3.4 sec | On | On | On | Off | On |
| Horn Hall | 6 — Small Concert Hall A | 3.4 sec | On | Off | On | Off | On |

Visible settings match earlier references within estimation tolerance. IMMED is off in every image. DI's Room and DI's Drum Room remain separate entries with distinct visible settings. Room A and Const Plate remain absent from the current DSP. Display readings are unassigned to individual parameters; exact fader values and earlier unidentified references remain pending.

## Bright — batch 03

Tag inherited from preceding Bright batches. All five presets already exist and receive Bright as an additional tag without duplication.

| Preset | Source program | Display | Mode enhancement | Decay optimization in this batch | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| In the name of Lex | 5 — Percussion Plate A | 1.8 sec | On | On (differs from earlier Off) | On | Off | On |
| Indie Drum Room | 7 — Room A (unsupported) | 1.8 sec | On | On | On | On | On |
| Loose Hall | 6 — Small Concert Hall A | 2.4 sec | On | Off | On | Off | On |
| Lush Piano | 6 — Small Concert Hall A | 6.5 sec | On | On | On | Off | On |
| Marv Conga Plate | 5 — Percussion Plate A | 2.8 sec | On | On | On | Off | On |

In the name of Lex shows Decay Optimization ON here, unlike OFF in earlier named references. The catalog preserves previous mappings, flags the discrepancy and records the new image-specific modifier states; no canonical state is selected. Its program 5 LED is clearly lit in this image. IMMED is off throughout this batch. Room A remains absent from the current DSP. Exact numerical fader values and earlier unidentified references remain pending.

## Bright — batch 04

Tag inherited from preceding Bright batches. All five presets already exist and receive Bright as an additional tag without duplication.

| Preset | Source program | Display | Mode enhancement | Decay optimization | IMMED | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Mexicon Snare Plate | 5 — Percussion Plate A | 1.3 sec | On | On | Off | Off | Off | On |
| Mexicon Snare Room | 7 — Room A (unsupported) | 1.1 sec | On | On | Off | Off | Off | On |
| Mid Tempo Snare | 5 — Percussion Plate A | 1.5 sec | On | Off | Off | On | Off | On |
| Mountains of Needles | 5 — Percussion Plate A | 4.6 sec | On | Off | Off | On | Off | On |
| Percussive Slap | 6 — Small Concert Hall A | 0.6 sec | On | Off | On | On | Off | On |

Visible settings match earlier references within estimation tolerance. Mexicon Snare Plate and Mexicon Snare Room remain distinct; Mid Tempo Snare remains separate from MidTempo Snare Room. Room A remains absent from the current DSP. Display readings remain unassigned to individual parameters; exact numerical fader values and earlier unidentified references remain pending.

## Bright — batch 05

Tag inherited from preceding Bright batches. All four presets already exist and receive Bright as an additional tag without duplication.

| Preset | Source program | Display | Mode enhancement | Decay optimization in this batch | SYS NOISE | REAR OUTS | SOLO |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Plateau Piano | 6 — Small Concert Hall A | 10.0 sec | On | Off | On | Off | On |
| Shock the Toms | 5 — Percussion Plate A | 2.2 sec | On | Off | On | Off | On |
| Shuffle Slap | 6 — Small Concert Hall A | 0.8 sec | Off | Off | On | Off | On |
| Wider-1 | 6 — Small Concert Hall A | 0.6 sec | On | Off | Off | Off | On |

Wider-1 shows Decay Optimization OFF here, consistent with its initial references; its Short-04 image 2 observation of ON remains preserved and flagged as a discrepancy. IMMED is off throughout. Display readings remain unassigned to individual parameters; exact numerical fader values and earlier unidentified references remain pending.
