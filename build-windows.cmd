@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul
if errorlevel 1 (
  echo ERRO: instale CMake 3.22 ou superior e adicione ao PATH.
  goto failed
)
where git >nul 2>nul
if errorlevel 1 (
  echo ERRO: instale Git for Windows e adicione ao PATH.
  goto failed
)
echo Requer Visual Studio 2022: Desktop development with C++, v143 e Windows SDK.
cmake -S Lexicon224 -B build-windows -G "Visual Studio 17 2022" -A x64
if errorlevel 1 goto failed
cmake --build build-windows --config Release --parallel 2
if errorlevel 1 goto failed
ctest --test-dir build-windows -C Release --output-on-failure
if errorlevel 1 goto failed
if not exist dist mkdir dist
powershell -NoProfile -Command "Compress-Archive -Path 'build-windows/Lexicon224_artefacts/Release/VST3/Lexicon224.vst3','docs/preset-readiness-audit.json' -DestinationPath 'dist/Lexicon224-Windows-x64-VST3.zip' -Force"
if errorlevel 1 goto failed
echo.
echo VST3 gerado em build-windows\Lexicon224_artefacts\Release\VST3\Lexicon224.vst3
echo ZIP gerado em dist\Lexicon224-Windows-x64-VST3.zip
echo Copie a pasta Lexicon224.vst3 inteira para C:\Program Files\Common Files\VST3
exit /b 0
:failed
echo.
echo A compilacao nao foi concluida. Consulte o erro acima.
exit /b 1
