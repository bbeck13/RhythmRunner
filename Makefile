default:
	if [ -a build ] ; then \
	   cd build && make; \
	else \
	   mkdir build && cd build && cmake .. && make;\
	fi;

clean:
	if [ -a build ] ; then \
	   rm -rf build; \
	fi;

