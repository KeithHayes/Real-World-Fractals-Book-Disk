@echo off
rem Install Real-World Fractals sources and demonstration programs

echo Decompressing sources
pkunzip source.zip
echo Decompressing executables
pkunzip exec.zip

set RWF_DISPLAY=3
echo The modules have been installed and may now be compiled with make
