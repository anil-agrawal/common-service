cmake_minimum_required(VERSION 2.8.9)
project(MY_SERVICE_WRAPPER)

set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE "AppServiceWrapper")
set(EXECUTABLE_OUTPUT "ServiceWrapper")
# set(POSTGRES_ROOT_PATH "$ENV{POSTGRES_ROOT}")
	

file(GLOB main_sources      ../src/*.cpp)

if(WIN32)
	add_definitions(-DWINDOWS)
	file(GLOB win_sources      ../src/winservice/*.cpp)
	add_executable(${EXECUTABLE} ${main_sources} ${win_sources})
else(LINUX_DEV_PLATFORM)
	file(GLOB nix_sources      ../src/nixservice/*.cpp)
	add_executable(${EXECUTABLE} ${main_sources} ${nix_sources})
endif(WIN32)

target_compile_options(${EXECUTABLE} PUBLIC -std=c++11)

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread  -static -lws2_32")
else(LINUX_DEV_PLATFORM)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread")
endif(WIN32)

set_target_properties(${EXECUTABLE} PROPERTIES OUTPUT_NAME  ${EXECUTABLE_OUTPUT} )

if(WIN32)
	target_link_libraries(${EXECUTABLE} ws2_32 pthread)
else(LINUX_DEV_PLATFORM)
	target_link_libraries(${EXECUTABLE} pthread)
endif(WIN32)

#if(MINGW)
#   target_link_libraries(executableName ws2_32 pthread)
#endif()

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE_NAME:${EXECUTABLE}> "${CMAKE_CURRENT_LIST_DIR}/../bin/"
  COMMENT "Copying to output directory")