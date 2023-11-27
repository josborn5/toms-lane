call .\build-scripts\compile-project.bat obby-1 obby\obby-1\obby-1.cpp %1

set APP_DIR=%OUTPUT_DIR%\obby-1\

xcopy "%CODE_DIR%\obby\brick.sprc" "%APP_DIR%\"
xcopy "%CODE_DIR%\obby\checkpoint.sprc" "%APP_DIR%\"
xcopy "%CODE_DIR%\obby\player.sprc" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\obby-1.exe %1
popd