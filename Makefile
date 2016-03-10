appName=hotrod

# this is the default because it's the first one
br: build run

build:
	mkdir -p bin/cmake
	cd bin/cmake && cmake ../../
	make -C bin/cmake -j16
	mv bin/cmake/$(appName) bin/
	
run:
	cd bin && ./$(appName)
