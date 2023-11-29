call .\build-scripts\set-variables.bat %DEBUG_FLAG%

call .\build-scripts\compile.bat breakout breakout\breakout.cpp %1
call .\build-scripts\link.bat "%APP_DIR%\%NAME%.obj" %OUTPUT_DIR%\%TL_PLATFORM%.lib %OUTPUT_DIR%\%TL_WIN32%.lib
call .\build-scripts\run.bat breakout %1
