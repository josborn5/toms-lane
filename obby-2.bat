call .\build-scripts\compile-project.bat obby-2 obby\obby-2\obby-2.cpp %1

pushd %OUTPUT_DIR%

del .\background.sprc

xcopy ..\%CODE_DIR%\obby\background.sprc .

popd

call .\build-scripts\run-project.bat obby-2 %1