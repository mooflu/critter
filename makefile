all: .check-env
	./scripts/build-emscripten.sh

.check-env:
	@test $${EMSDK?Please run emscripten env setup}

run:
	cd build/game; ../../scripts/server.py

clean:
	rm -f CMakeCache.txt
	rm -rf CMakeFiles
	rm -rf cmake_install.cmake
	rm -rf */CMakeFiles
	rm -rf */cmake_install.cmake
	rm -rf */Makefile
	rm -rf build*

dist-clean: clean
	rm -f resource.dat
	rm -rf glm
	rm -rf physfs
	rm -rf oem
	rm -rf oem.emscripten

