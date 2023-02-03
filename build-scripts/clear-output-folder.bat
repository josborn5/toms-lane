set OUTPUT_DIR=%1

rmdir /S /Q %OUTPUT_DIR%
mkdir %OUTPUT_DIR%
pushd %OUTPUT_DIR%