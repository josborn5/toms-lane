cd src

cmake -S . -B ./cmake-build || exit 1

cmake --build ./cmake-build --config Release || exit 1

./cmake-build/src/test/demo-tests || exit 1

cd ..

