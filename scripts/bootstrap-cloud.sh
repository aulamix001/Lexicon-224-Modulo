#!/usr/bin/env bash
set -euo pipefail
# Rootless Debian 13 setup. Keep TLS and APT package signature verification enabled.
repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
python -m venv /workspace/tools
/workspace/tools/bin/pip install cmake==4.4.4 ninja==1.13.2
mkdir -p /workspace/tools/apt/{lists/partial,archives/partial,etc,cache} /workspace/tools/sysroot
cat > /workspace/tools/apt/config <<'CONFIG'
Dir::Etc::parts "-";
Dir::Etc::main "-";
Dir::Etc::sourcelist "/workspace/tools/apt/etc/sources.list";
Dir::Etc::sourceparts "-";
Dir::State::lists "/workspace/tools/apt/lists";
Dir::Cache::archives "/workspace/tools/apt/archives";
Dir::Cache "/workspace/tools/apt/cache";
APT::Get::List-Cleanup "false";
CONFIG
cat > /workspace/tools/apt/etc/sources.list <<'SOURCES'
deb [signed-by=/usr/share/keyrings/debian-archive-keyring.gpg] https://deb.debian.org/debian trixie main
SOURCES
export APT_CONFIG=/workspace/tools/apt/config
/usr/bin/apt-get update
/usr/bin/apt-get -o Debug::NoLocking=true --download-only -y install pkg-config libasound2-dev libx11-dev libxext-dev libxinerama-dev libxrandr-dev libxcursor-dev libfreetype6-dev libgl1-mesa-dev
for archive in /workspace/tools/apt/archives/*.deb; do dpkg-deb -x "$archive" /workspace/tools/sysroot; done
python - <<'PYCODE'
from pathlib import Path
import os
root=Path('/workspace/tools/sysroot/usr/lib/x86_64-linux-gnu')
for p in root.glob('*.so'):
    if p.is_symlink() and not p.exists():
        target=os.readlink(p)
        source=Path('/usr/lib/x86_64-linux-gnu')/target
        if source.exists() and not (p.parent/target).exists():
            (p.parent/target).symlink_to(source)
PYCODE
mkdir -p /workspace/deps
if [[ ! -d /workspace/deps/JUCE/.git ]]; then
 git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git /workspace/deps/JUCE
fi
[[ "$(git -C /workspace/deps/JUCE rev-parse HEAD)" == 4f43011b96eb0636104cb3e433894cda98243626 ]]
export PKG_CONFIG_SYSROOT_DIR=/workspace/tools/sysroot
export PKG_CONFIG_PATH=/workspace/tools/sysroot/usr/lib/x86_64-linux-gnu/pkgconfig:/workspace/tools/sysroot/usr/share/pkgconfig
/workspace/tools/bin/cmake -S "$repo_dir/Lexicon224" -B "$repo_dir/build" -G Ninja -DCMAKE_MAKE_PROGRAM=/workspace/tools/bin/ninja -DJUCE_PATH=/workspace/deps/JUCE -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-I/workspace/tools/sysroot/usr/include -DCMAKE_LIBRARY_PATH=/workspace/tools/sysroot/usr/lib/x86_64-linux-gnu -DCMAKE_INCLUDE_PATH=/workspace/tools/sysroot/usr/include
/workspace/tools/bin/cmake --build "$repo_dir/build" -j 2
/workspace/tools/bin/ctest --test-dir "$repo_dir/build" --output-on-failure
