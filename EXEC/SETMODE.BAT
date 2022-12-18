@echo off
echo.
echo Set the display resolution for Real-World Fractals programs
if x==x%1 goto askit
if %1==0 goto s0
if %1==1 goto s1
if %1==2 goto s2
if %1==3 goto s3
if %1==4 goto s4
if %1==5 goto s5
if %1==6 goto s6
goto s3

goto end

:askit
echo Enter the number of the desired display resolution:
echo.
echo    0 - 320 x 200
echo    1 - 640 x 400
echo    2 - 640 x 480
echo    3 - 800 x 600 (default)
echo    4 - 1024 x 768
echo    5 - 640  x 350
echo    6 - 1280 x 1024
echo.  

choice /c:0123456 /n Desired display mode ? 
if errorlevel 2 goto d1
:s0
set RWF_DISPLAY=0
echo Setting 320 x 200 display mode
goto end
:d1
if errorlevel 3 goto d2
:s1
set RWF_DISPLAY=1
echo Setting 640 x 400 display mode
goto end
:d2
if errorlevel 4 goto d3
:s2
set RWF_DISPLAY=2
echo Setting 640 x 480 display mode
goto end
:d3
if errorlevel 5 goto d4
:s3
set RWF_DISPLAY=3
echo Setting 800 x 600 display mode
goto end
:d4
if errorlevel 6 goto d5
:s4
set RWF_DISPLAY=4
echo Setting 1024 x 768 display mode
goto end
:d5
if errorlevel 7 goto s6
:s5
set RWF_DISPLAY=5
echo Setting 640 x 350 display mode
goto end

:s6
set RWF_DISPLAY=6
echo Setting 1280 x 1024 display mode

:end


