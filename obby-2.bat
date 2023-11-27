call .\build-scripts\compile-project.bat obby-2 obby\obby-2\obby-2.cpp %1

set APP_DIR=%OUTPUT_DIR%\obby-2\

xcopy "%CODE_DIR%\obby\background.sprc" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\obby-2.exe %1
popd