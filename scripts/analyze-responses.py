#!/usr/bin/env python3
"""Offline characterization, not proof of hardware equivalence. Requires numpy/scipy.
Optional --reference points to hardware WAVs with the same program/profile filenames.
The reverb itself does not use FFTs; these offline measurement tools may.
"""
import argparse
import csv
import json
import struct
from pathlib import Path
import numpy as np
from scipy.signal import butter, sosfilt

def read_wav(path):
    raw = path.read_bytes()
    if raw[:4] != b'RIFF' or raw[8:12] != b'WAVE':
        raise ValueError(f'Unsupported WAV: {path}')
    position, fmt, payload = 12, None, None
    while position + 8 <= len(raw):
        kind, count = struct.unpack_from('<4sI', raw, position)
        chunk = raw[position+8:position+8+count]
        if kind == b'fmt ':
            fmt = struct.unpack_from('<HHIIHH', chunk)
        if kind == b'data':
            payload = chunk
        position += 8 + count + (count % 2)
    if fmt is None or payload is None:
        raise ValueError(f'Missing WAV chunks: {path}')
    code, channels, rate, _, _, bits = fmt
    if code == 3 and bits == 32:
        signal = np.frombuffer(payload, '<f4').astype(float)
    elif code == 1 and bits == 16:
        signal = np.frombuffer(payload, '<i2').astype(float) / 32768
    elif code == 1 and bits == 24:
        b = np.frombuffer(payload, np.uint8).reshape(-1, 3).astype(np.int32)
        v = b[:, 0] | (b[:, 1] << 8) | (b[:, 2] << 16)
        signal = ((v ^ 0x800000) - 0x800000) / 8388608
    else:
        raise ValueError(f'Unsupported WAV encoding ({code}/{bits}): {path}')
    return rate, signal.reshape(-1, channels)

def decay_time(signal, rate, low, high):
    power = np.mean(signal * signal, axis=1)
    edc = np.cumsum(power[::-1])[::-1]
    if edc[0] == 0:
        return None
    db = 10*np.log10(np.maximum(edc/edc[0], 1e-30))
    idx = np.flatnonzero((db <= low) & (db >= high))
    if len(idx) < 100:
        return None
    slope, intercept = np.polyfit(idx/rate, db[idx], 1)
    fit = slope*idx/rate+intercept
    r2 = 1-np.sum((db[idx]-fit)**2)/max(1e-30, np.sum((db[idx]-np.mean(db[idx]))**2))
    return {'rt60_extrapolated_s': round(float(-60/slope), 4), 'r_squared': round(float(r2), 5)}

def measure(path):
    rate, x = read_wav(path)
    if x.shape[1] < 2 or not np.all(np.isfinite(x)):
        raise ValueError(f'Invalid stereo response: {path}')
    power = np.mean(x*x, axis=1)
    audible = np.flatnonzero(np.max(np.abs(x), axis=1) > 1e-5)
    energy = np.sum(power)
    early = np.sum(power[:int(rate*.08)])
    late = np.sum(power[int(rate*.08):])
    tail = x[-rate:]
    row = {'file': path.name, 'sample_rate': rate,
           'first_above_minus100dB_ms': round(float(audible[0]/rate*1000), 3) if len(audible) else None,
           'peak': round(float(np.max(np.abs(x))), 6),
           'energy': float(energy),
           'c80_dB': round(float(10*np.log10(max(early,1e-30)/max(late,1e-30))), 3),
           'tail_last_second_rms_dBFS': round(float(20*np.log10(max(np.sqrt(np.mean(tail*tail)),1e-15))), 3),
           'stereo_correlation': round(float(np.corrcoef(x.T)[0,1]), 5),
           'T20': decay_time(x,rate,-5,-25), 'T30': decay_time(x,rate,-5,-35)}
    for centre in [125, 500, 2000, 8000]:
        band = sosfilt(butter(3, [centre/np.sqrt(2), min(centre*np.sqrt(2),rate*.49)], btype='bandpass',fs=rate,output='sos'),x,axis=0)
        row[f'T20_{centre}Hz'] = decay_time(band,rate,-5,-25)
    return row

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory',type=Path)
    parser.add_argument('--output',type=Path,default=Path('response-metrics.json'))
    parser.add_argument('--reference',type=Path)
    args=parser.parse_args()
    paths=sorted(args.directory.glob('program*_profile*.wav'))
    if not paths:
        parser.error('No matching impulse responses; render responses first')
    measured=[]
    for path in paths:
        row=measure(path)
        if args.reference:
            reference=args.reference/path.name
            if not reference.exists():
                raise FileNotFoundError(f'Missing matching hardware reference: {reference}')
            # Acoustic descriptors do not require waveform/sample-clock alignment.
            row['hardware_reference']=measure(reference)
        measured.append(row)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps({'hardware_reference_supplied':bool(args.reference),'measurements':measured},indent=2)+'\n')
    for row in measured:
        rt=row['T20']['rt60_extrapolated_s'] if row['T20'] else None
        print(f"{row['file']}: onset {row['first_above_minus100dB_ms']} ms, T20→RT60 {rt} s, corr {row['stereo_correlation']}, residual {row['tail_last_second_rms_dBFS']} dBFS")
if __name__=='__main__':
    main()
