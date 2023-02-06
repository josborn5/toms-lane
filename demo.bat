call ./build-scripts/set-variables.bat

pushd %OUTPUT_DIR%
del .\teapot.obj
xcopy ..\%CODE_DIR%\demo\teapot.obj .
popd

call .\build-scripts\compile-project.bat demo demo\demo.cpp %1
call .\build-scripts\run-project.bat demo  %1
