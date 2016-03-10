cmake_minimum_required(VERSION 3.2)
project(hotrod)

# collect source files
file(GLOB_RECURSE sources src/*.cpp)

# where to find includes
include_directories(SYSTEM
	/usr/local/include
	${CMAKE_CURRENT_SOURCE_DIR}/libs/v8
	/home/bart/software/bullet3-2.83.6/src
)

# where to find static libraries
link_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/v8/bin
	/home/bart/software/bullet3-2.83.6/bin
)

# compile these to a binary
add_executable(hotrod ${sources})

# add pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# which libraries to link to
target_link_libraries(hotrod
	Threads::Threads
	SDL2 GL GLEW
	v8_base v8_external_snapshot v8_libbase v8_libplatform
	dl rt
	BulletDynamics_gmake_x64_release
	BulletCollision_gmake_x64_release
	LinearMath_gmake_x64_release
)

# TODO
#warnings="-pedantic -Wall -Wno-unused-result -Wno-missing-field-initializers -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef"
##warnings="$warnings -Weffc++"
##warnings="$warnings -Werror"
##warnings="$warnings -Wpadded"

# compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
