default:
	git submodule init && git submodule update
	if [ -a build ] ; then \
	   cd build && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make; \
	else \
	   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make;\
	fi;

release:
	git submodule init && git submodule update
	if [ -a build ] ; then \
	   cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make; \
	else \
	   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make;\
	fi;

debug:
	git submodule init && git submodule update
	if [ -a build ] ; then \
	   cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make; \
	else \
	   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make;\
	fi;

clean:
	if [ -a build ] ; then \
	   rm -rf build; \
	fi;

