call .\build-scripts\compile-project.bat demo demo\demo.cpp %1

set APP_DIR=%OUTPUT_DIR%\demo\
xcopy "%CODE_DIR%\demo\teapot.obj" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\demo.exe %1
popd
