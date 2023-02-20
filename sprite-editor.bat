call ./build-scripts/set-variables.bat

call .\build-scripts\compile-project.bat sprite-editor sprite-editor\sprite-editor.cpp %1
call .\build-scripts\run-project.bat sprite-editor  %1
