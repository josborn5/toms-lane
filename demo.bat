call "%~dp0\compile.bat"

pushd "%~dp0\cmake-build\Debug"
call .\demo-win32.exe
popd
