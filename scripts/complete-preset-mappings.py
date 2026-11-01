#!/usr/bin/env python3
"""Complete missing preset states from names; preserve every existing parameter.

Name estimates are new sound designs, never recovered original settings.
"""
import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
KEYS = {'program', 'mode', 'optimize', 'modeRate', 'decayAmount', 'mix',
        'mono', 'converter', 'diffusion', 'bass', 'mid', 'crossover',
        'treble', 'depth', 'predelay'}


def estimate(name):
    name = name.casefold()
    program, decay, predelay = 2, 2.8, 24
    if any(word in name for word in ('chorus', 'wider', 'wide')):
        program, decay, predelay = 8, 1.2, 24
    elif 'plate' in name or any(word in name for word in ('snare', 'kick', 'toms')):
        program, decay, predelay = (1 if any(w in name for w in ('vox', 'voc')) else 4), 1.5, 20
    elif 'room' in name:
        program, decay, predelay = 6, .9, 8
    elif 'chamber' in name:
        program, decay, predelay = 3, 1.8, 25
    elif 'hall' in name or 'piano' in name:
        program, decay, predelay = 2, 3.2, 24
    if any(word in name for word in ('long', 'lush', 'lingering', 'atmosphere', 'ballad', 'mountains')):
        decay *= 1.65
    if any(word in name for word in ('short', 'slap', 'gate', 'percussive')):
        decay = .65
    if 'echo' in name or 'slap' in name:
        predelay = 105
    # Stable small variations avoid giving unrelated names identical designs.
    shade = hashlib.sha256(name.encode()).digest()[0] / 255
    decay *= .9 + .2 * shade
    dark = any(word in name for word in ('dark', 'midnight', 'heavy'))
    bright = any(word in name for word in ('bright', 'shimmer', 'needles'))
    bounds = [(24, 152), (20, 127), (24, 152), (25, 255), (0, 107),
              (24, 152), (0, 255), (0, 255), (0, 255)]
    return dict(program=program, mode=True, optimize=True, modeRate=2,
                decayAmount=7 if program in (1, 4) else 5, mix=100,
                mono=False, converter=0, diffusion=.75,
                bass=max(.6, decay * (1.2 if dark else 1.05)),
                mid=max(.6, decay), crossover=500,
                treble=2800 if dark else 9000 if bright else 5500,
                depth=40 if program == 8 else 20,
                predelay=max(bounds[program][0], min(bounds[program][1], predelay)))


def complete(catalog):
    retained, generated = [], []
    for entry in catalog['presets']:
        if not entry.get('name'):
            continue
        model = entry.get('visual_reconstruction', {})
        existing = model.get('parameters', {})
        if KEYS <= existing.keys() and model.get('loadable_with_current_dsp'):
            retained.append(entry['name'])
            continue
        state = estimate(entry['name'])
        state.update(existing)
        entry['visual_reconstruction'] = dict(
            method='name-semantic-estimate-v1', verified_original=False,
            loadable_with_current_dsp=True,
            warning='Original sound design estimated from the name; existing values retained. Not an original preset recovery.',
            parameters=state)
        entry['implemented_algorithm_index'] = state['program']
        generated.append(entry['name'])
    return dict(existing_states_retained=retained,
                name_estimates_created=generated,
                original_equivalence_verified=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--catalog', type=Path, default=ROOT / 'presets/catalog.json')
    parser.add_argument('--report', type=Path, default=ROOT / 'docs/preset-mapping-completion.json')
    args = parser.parse_args()
    data = json.loads(args.catalog.read_text())
    report = complete(data)
    if report['name_estimates_created']:
        args.catalog.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n')
    args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n')
    print(f"{len(report['existing_states_retained'])} existing states retained; "
          f"{len(report['name_estimates_created'])} name-based estimates created")


if __name__ == '__main__':
    main()
