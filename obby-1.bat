call .\build-scripts\compile-project.bat obby-1 obby\obby-1\obby-1.cpp %1

pushd %OUTPUT_DIR%
xcopy ..\%CODE_DIR%\obby\brick.sprc .
xcopy ..\%CODE_DIR%\obby\checkpoint.sprc .
xcopy ..\%CODE_DIR%\obby\player.sprc .
popd

call .\build-scripts\run-project.bat obby-1 %1