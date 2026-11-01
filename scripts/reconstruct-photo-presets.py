#!/usr/bin/env python3
"""Build explicit estimated states from photo positions. Never asserts original accuracy."""
import json,math
from pathlib import Path
root=Path(__file__).resolve().parent.parent
p=root/'presets/catalog.json';data=json.loads(p.read_text())
ranges={'bass':(.6,70,4),'mid':(.6,70,4),'crossover':(100,10900,1000),'treble':(100,10900,4400),'depth':(0,71,None),'predelay':(0,255,None)}
limits=[(24,152),(20,127),(24,152),(25,255),(0,107),(24,152),(0,255),(0,255),(0,255)]
for entry in data['presets']:
 if not entry['name']:continue
 program=entry['confirmed_plugin_parameters'].get('program')
 if program is None:
  number=entry['program'].get('panel_number')
  program=number-1 if number in (7,8) else 8
 entry['implemented_algorithm_index']=program
 model={'method':'photo-slot-to-host-range-v1','verified_original':False,'loadable_with_current_dsp':program is not None,'warning':'Reconstructed from estimated photo positions; original slider transfer curves are unknown. Hidden values use explicit defaults. First named reference resolves conflicting LEDs for this reconstruction only.'}
 if program is not None:
  values={'program':program,'mode':True,'optimize':True,'modeRate':2,'decayAmount':7 if program in (1,4) else 5,'mix':100,'mono':False,'converter':0,'diffusion':.5}
  for key,(lo,hi,centre) in ranges.items():
   # Recorded observations include roughly 10% travel margin for handle thickness.
   normalized=max(0,min(1,(entry['approximate_fader_positions']['positions'][key]-.1)/.8))
   skew=math.log(.5)/math.log((centre-lo)/(hi-lo)) if centre else 1
   value=lo+(hi-lo)*(normalized**(1/skew))
   if key in ('depth','predelay'):value=math.floor(value+.5)
   values[key]=value
  values['predelay']=max(limits[program][0],min(limits[program][1],values['predelay']))
  values.update(entry['confirmed_plugin_parameters'])
  model['parameters']=values
 entry['visual_reconstruction']=model
p.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
print(sum(bool(e.get('visual_reconstruction',{}).get('loadable_with_current_dsp')) for e in data['presets']),'estimated complete states generated; originals unverified')
