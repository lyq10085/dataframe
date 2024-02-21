cmake_minimum_required(VERSION 3.11)
project(dataFrame)
include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG        master
)
FetchContent_GetProperties(absl)
if(NOT absl_POPULATED)
    FetchContent_Populate(absl)
    add_subdirectory(${absl_SOURCE_DIR} ${absl_BINARY_DIR})
endif()

add_executable(dataFrame main.cpp
        dataframe.h
        dataframe.cpp
        slice.h
        arena.cpp
        arena.h
        FeatureValue.h )



target_link_libraries( dataFrame absl::variant )
