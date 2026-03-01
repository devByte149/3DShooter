cmake_minimum_required(VERSION 3.20)
project(3DShooter)

# --- 1. Project Settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 2. Find Vulkan ---
find_package(Vulkan REQUIRED)

# --- 3. Identify Source Files ---
# We use GLOB here for convenience since you are starting from scratch
file(GLOB_RECURSE SRC_FILES 
    "./Source/*.cpp"
    "./Source/*.h"
    "./Source/*.hpp"
)

# --- 4. Create Executable (MUST COME BEFORE INCLUDE/LINK) ---
# We add "Source/main.cpp" explicitly to ensure the list isn't empty
add_executable(${PROJECT_NAME} ${SRC_FILES})

# --- 5. Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE 
    "./Source"
    "./gateware-main"
    "./entt-3.13.1/single_include"
    ${Vulkan_INCLUDE_DIRS}
)

# --- 6. Link Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Vulkan::Vulkan
)

# --- 7. Windows Specifics ---
if (WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    target_compile_options(${PROJECT_NAME} PRIVATE "/MD")
    # Gateware/Vulkan often need this for Windows
    add_definitions(-DUNICODE -D_UNICODE) 
endif()