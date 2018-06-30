build/Makefile:	CMakeLists.txt
	mkdir -p build; \
	cd build; \
	cmake ../ -DCMAKE_TOOLCHAIN_FILE=~/github/vcpkg/scripts/buildsystems/vcpkg.cmake

clean:
	rm -rf build/