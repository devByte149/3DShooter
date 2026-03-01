cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME 3DShooter)
project(${PROJECT_NAME})

# Great way to reduce compile times for large files like Gateware.h 
# https://edgarluque.com/blog/cmake-precompiled-headers/
set(PRE_COMPILED
	./Source/precompiled.h 
)

# Create list of source files.
# CMake generally discourages the use of GLOB. (it can make doing more complex operations fail)
# For students the conveinece of not editing the CMake constantly is probably more important.  
file(GLOB_RECURSE PROJECT_FILES CONFIGURE_DEPENDS
    ./Source/*.h
    ./Source/*.hpp
    ./Source/*.cpp
)

#file(GLOB IMGUI_CORE
#    ./imgui/*.h
#    ./imgui/*.cpp
#)

#set(IMGUI_BACKENDS
#    ./imgui/backends/imgui_impl_vulkan.h
#    ./imgui/backends/imgui_impl_vulkan.cpp
#    ./imgui/backends/imgui_impl_win32.h
#    ./imgui/backends/imgui_impl_win32.cpp
#)

file(GLOB SHADER_FILES ./Shaders/*.hlsl)

set(SOURCE_FILES 
    ${PROJECT_FILES} 
    ${IMGUI_CORE} 
    ${IMGUI_BACKENDS} 
    ${SHADER_FILES}
    ./gateware-main/Gateware.h
    ./entt-3.13.1/single_include/entt/entt.hpp
    ./defaults.ini
    ./inifile-cpp-master/include/inicpp.h
)


# force unicode in some libraries on win32
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

if (WIN32)
	# by default CMake selects "ALL_BUILD" as the startup project 
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	add_executable (${PROJECT_NAME} ${SOURCE_FILES})

	source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_FILES})


	target_include_directories(${PROJECT_NAME} PUBLIC
    $ENV{VULKAN_SDK}/Include/
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
    )



	
	target_link_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Lib/)
	# shaderc_combined.lib in Vulkan requires this for debug & release (runtime shader compiling)
	target_compile_options(${PROJECT_NAME} PRIVATE "/MD")


endif(WIN32)

if(UNIX AND NOT APPLE)
	# libshaderc_combined.a is required for runtime shader compiling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lX11 -lshaderc_combined")
    find_package(X11)
	find_package(Vulkan REQUIRED)
    link_libraries(${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
    include_directories(${Vulkan_INCLUDE_DIR}) 
	#link_directories(${Vulkan_LIBRARY}) this is currently not working
	link_libraries(${Vulkan_LIBRARIES})
	# the path is (properly)hardcoded because "${Vulkan_LIBRARY}" currently does not 
	# return a proper path on MacOS (it has the .dynlib appended)
    link_libraries(/usr/lib/x86_64-linux-gnu/libshaderc_combined.a)
    add_executable (${PROJECT_NAME} ${SOURCE_FILES})
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_FILES})
endif(UNIX AND NOT APPLE)

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fmodules -fcxx-modules")
	set(Architecture ${CMAKE_OSX_ARCHITECTURES})
	find_package(Vulkan REQUIRED)
	include_directories(${Vulkan_INCLUDE_DIR}) 
	#link_directories(${Vulkan_LIBRARY}) this is currently not working
	link_libraries(${Vulkan_LIBRARIES})
	# libshaderc_combined.a is required for runtime shader compiling
	# the path is (properly)hardcoded because "${Vulkan_LIBRARY}" currently does not 
	# return a proper path on MacOS (it has the .dynlib appended)
	link_libraries(/usr/local/lib/libshaderc_combined.a)
	add_executable (${PROJECT_NAME} source/main.mm)
endif(APPLE)

# EnTT requires c++17 support
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# adding gateware.h and other librairies as a precompiled headers to reduce compile times
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRE_COMPILED})

# Disable all shaders from compiling since they may have Vulkan specifics

set_source_files_properties(${SHADER_FILES} PROPERTIES
    VS_TOOL_OVERRIDE "None" # Prevents MSVC from trying to compile them
    HEADER_FILE_ONLY TRUE   # Tells VS these are just reference files
)

set_source_files_properties("./Shaders/LevelVertex.hlsl" PROPERTIES 
    VS_SHADER_TYPE Vertex
    VS_SHADER_MODEL 5.1
    VS_SHADER_ENTRYPOINT main
    VS_TOOL_OVERRIDE "None"
)

set_source_files_properties("./Shaders/LevelPixel.hlsl" PROPERTIES 
    VS_SHADER_TYPE Pixel
    VS_SHADER_MODEL 5.1
    VS_SHADER_ENTRYPOINT main
    VS_TOOL_OVERRIDE "None"
)
