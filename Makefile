ifeq ($(OS),Windows_NT)
	CMAKE_ARGS = -G "Visual Studio 14 2015 Win64"
	MAKE_CMD = echo windoze
else
	CMAKE_ARGS = 
	MAKE_CMD = make
endif

.PHONY: default release_debug release debug clean

default: release_debug

release_debug:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && ${MAKE_CMD}

release:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && ${MAKE_CMD}

debug:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && ${MAKE_CMD}

clean:
	rm -rf build
