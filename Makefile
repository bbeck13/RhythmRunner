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
	ln -sf build/compile_commands.json .

xcode:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=RelWithDebInfo .. -G Xcode
	ln -sf build/compile_commands.json .

xcode_release:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release .. -G Xcode
	ln -sf build/compile_commands.json .

release:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release .. && ${MAKE_CMD}
	ln -sf build/compile_commands.json .

debug:
	git submodule init && git submodule update
	mkdir -p build && cd build && cmake ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug .. && ${MAKE_CMD}
	ln -sf build/compile_commands.json .

clean:
	rm -rf build
	rm compile_commands.json
