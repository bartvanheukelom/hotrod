appName=hotrod

# this is the default because it's the first one
br: build run

generate:
	mkdir -p bin/gen
	cd tools && ./mapglew.py && ./generategl.py

build:
	mkdir -p bin/cmake
	cd bin/cmake && cmake ../../
	make -C bin/cmake -j16
	mv bin/cmake/$(appName) bin/

full: generate build
	
run:
	cd bin && ./$(appName)
