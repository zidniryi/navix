cmake_minimum_required(VERSION 3.10)
project(navix)

set(CMAKE_CXX_STANDARD 17)

# Find ncurses for TUI support
find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES REQUIRED ncurses)

set(SOURCES
    src/main.cpp
    src/FileScanner.cpp
    src/Symbol.cpp
    src/TUI.cpp
    src/FileWatcher.cpp
    src/PerformanceLogger.cpp
)

add_executable(navix ${SOURCES})

# Link ncurses library
target_link_libraries(navix ${NCURSES_LIBRARIES})
target_include_directories(navix PRIVATE ${NCURSES_INCLUDE_DIRS})
target_compile_options(navix PRIVATE ${NCURSES_CFLAGS_OTHER})
