cmake_minimum_required(VERSION 3.10)
project(Navix VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Platform-specific dependencies
if(WIN32 OR MINGW)
    # Windows build - no ncurses needed
    message(STATUS "Building for Windows - using Windows Console API")
else()
    # Unix/Linux build - find ncurses
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(NCURSES REQUIRED ncurses)
    message(STATUS "Building for Unix/Linux - using ncurses")
endif()

set(SOURCES
    src/main.cpp
    src/FileScanner.cpp
    src/Symbol.cpp
    src/TUI.cpp
    src/FileWatcher.cpp
    src/PerformanceLogger.cpp
    src/AutocompleteEngine.cpp
    src/JsonExporter.cpp
    src/LSPServer.cpp
)

add_executable(navix ${SOURCES})

# Platform-specific linking and compilation
if(WIN32 OR MINGW)
    # Windows-specific libraries and settings
    target_compile_definitions(navix PRIVATE _WIN32)
    if(MINGW)
        # For MinGW cross-compilation
        target_link_libraries(navix -static-libgcc -static-libstdc++)
    endif()
else()
    # Unix/Linux - link ncurses library
    target_link_libraries(navix ${NCURSES_LIBRARIES})
    target_include_directories(navix PRIVATE ${NCURSES_INCLUDE_DIRS})
    target_compile_options(navix PRIVATE ${NCURSES_CFLAGS_OTHER})
endif()
