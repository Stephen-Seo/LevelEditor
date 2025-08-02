cmake_minimum_required(VERSION 3.25.1)
project(LevelEditor CXX)

set(LevelEditor_SOURCES
    src/main.cpp
    src/game.cpp
    src/stateStack.cpp
    src/state.cpp
    src/editState.cpp
)

add_executable(LevelEditor ${LevelEditor_SOURCES})

target_compile_features(LevelEditor PUBLIC cxx_std_23)

add_compile_options(
    -Wall -Wextra -Wpedantic -Wno-missing-braces
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug', none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()
target_compile_options(LevelEditor PUBLIC
    -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough
    -Werror=format-security
    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3
    -D_GLIBCXX_ASSERTIONS
    -fstrict-flex-arrays=3
    -fstack-clash-protection -fstack-protector-strong
    -Wl,-z,nodlopen -Wl,-z,noexecstack
    -Wl,-z,relro -Wl,-z,now
    -Wl,--as-needed -Wl,--no-copy-dt-needed-entries
    -fPIE -pie
)

target_link_options(LevelEditor PUBLIC
    -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough
    -Werror=format-security
    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3
    -D_GLIBCXX_ASSERTIONS
    -fstrict-flex-arrays=3
    -fstack-clash-protection -fstack-protector-strong
    -Wl,-z,nodlopen -Wl,-z,noexecstack
    -Wl,-z,relro -Wl,-z,now
    -Wl,--as-needed -Wl,--no-copy-dt-needed-entries
    -fPIE -pie
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(LevelEditor PUBLIC
        -O3
        -fno-delete-null-pointer-checks -fno-strict-overflow
        -fno-strict-aliasing -ftrivial-auto-var-init=zero
        -DNDEBUG
    )
    target_link_options(LevelEditor PUBLIC
        -O3
        -fno-delete-null-pointer-checks -fno-strict-overflow
        -fno-strict-aliasing -ftrivial-auto-var-init=zero
        -DNDEBUG
    )
else()
    target_compile_options(LevelEditor PUBLIC
        -Og
    )
    target_link_options(LevelEditor PUBLIC
        -Og
    )
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML3 REQUIRED sfml-system sfml-window sfml-graphics)
target_link_libraries(LevelEditor PUBLIC ${SFML3_LIBRARIES})
target_include_directories(LevelEditor PUBLIC ${SFML3_INCLUDE_DIRS})
