call "%~dp0../../compile.bat"

pushd "%~dp0/../../cmake-build/Debug"
	call "toms-lane-platform-test.exe"
popd

