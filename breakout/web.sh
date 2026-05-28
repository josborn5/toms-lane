rm -r ./web/cmake-build

cmake -S web -B web/cmake-build || exit 1

cmake --build web/cmake-build --config Release || exit 1

python3 -m http.server -d web

