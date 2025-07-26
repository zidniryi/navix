# 🚀 NAVIX - Navigate & Index

**Lightning-fast for your navigation**

A powerful C++ file navigator with symbol indexing and autocomplete capabilities.

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/your-repo/navix)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![ncurses](https://img.shields.io/badge/TUI-ncurses-green.svg)](https://invisible-island.net/ncurses/)


## Features

- **Fast File Navigation**: TUI-based interface for quick file browsing
- **Symbol Indexing**: Parse and index symbols from various programming languages
- **Autocomplete Engine**: Intelligent code completion suggestions
- **Cross-Platform**: Runs on Unix/Linux/macOS and Windows
- **Performance Monitoring**: Built-in performance profiling
- **LSP Server**: Language Server Protocol integration
- **File Watching**: Real-time file system monitoring

## Cross-Platform Compilation

Navix supports both Unix/Linux/macOS (using ncurses) and Windows (using Windows Console API).

### Quick Build

Use the provided build script for easy compilation:

```bash
# Build for all platforms
./build.sh all

# Build for current platform only
./build.sh native

# Build for Windows only (cross-compilation)
./build.sh windows

# Create macOS package installer (macOS only)
./build.sh pkg

# Create Debian package for Linux
./build_deb.sh

# Clean build artifacts
./build.sh clean
```

### Manual Compilation

#### Unix/Linux/macOS (Native)
```bash
# Install dependencies
# macOS: brew install ncurses
# Ubuntu/Debian: sudo apt install libncurses5-dev
# Fedora: sudo dnf install ncurses-devel

# Compile
g++ -std=c++17 -o navix-native src/*.cpp $(pkg-config --cflags --libs ncurses)
```

#### Windows (Cross-compilation)
```bash
# Install MinGW-w64
# macOS: brew install mingw-w64
# Ubuntu: sudo apt install mingw-w64

# Cross-compile for Windows
x86_64-w64-mingw32-g++ -std=c++17 -D_WIN32 -o navix-windows.exe src/*.cpp -static-libgcc -static-libstdc++
```

### Platform-Specific Notes

#### Windows
- Uses Windows Console API instead of ncurses
- Simplified TUI interface optimized for Windows terminal
- All core functionality is preserved
- Executable is statically linked for easy distribution

#### Unix/Linux/macOS
- Full ncurses-based TUI with rich interface
- Advanced color support and window management
- Requires ncurses development headers

## Resolving the ncurses Issue

If you encounter the error:
```
fatal error: ncurses.h: No such file or directory
```

This happens when trying to compile for Windows, which doesn't have ncurses. The project now includes:

1. **Conditional compilation**: Uses ncurses on Unix/Linux/macOS, Windows Console API on Windows
2. **Cross-platform abstractions**: Unified interface for both platforms
3. **Automatic platform detection**: CMake and manual builds detect the target platform
4. **Build scripts**: Automated building for both platforms

## CMake Build (Alternative)

```bash
# Create build directory
mkdir build && cd build

# Configure for native platform
cmake ..
make

# Configure for Windows cross-compilation
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-toolchain.cmake ..
make
```

## Dependencies

### Runtime Dependencies
- **Unix/Linux/macOS**: ncurses library
- **Windows**: No external dependencies (statically linked)

### Build Dependencies
- **All platforms**: C++17 compatible compiler
- **Unix/Linux/macOS**: ncurses development headers, pkg-config
- **Windows cross-compilation**: MinGW-w64 toolchain

## Installation

### Option 1: Linux Debian Package (Recommended for Ubuntu/Debian)

```bash
# Build and create .deb package
git clone https://github.com/zidniryi/navix.git
cd navix
./build_deb.sh

# Install the package
sudo dpkg -i navix_1.0.0_amd64.deb

# If dependencies are missing, fix them with:
sudo apt-get install -f

# Verify installation
navix --version
```

### Option 2: macOS Package Installer (Recommended for macOS)

```bash
# Build and create package installer
git clone https://github.com/zidniryi/navix.git
cd navix
./build.sh pkg

# Install the package
sudo installer -pkg navix-installer.pkg -target /
# Or double-click navix-installer.pkg in Finder

# Verify installation
navix --version
```

### Option 3: Windows Executable (Recommended for Windows)

```bash
# Build and create Windows executable
git clone https://github.com/zidniryi/navix.git
cd navix
./build.sh windows

# The navix-windows.exe file is ready to use
# Copy to desired location or add to PATH

# Verify
navix-windows.exe --version
```

### Option 4: Manual Build

1. Clone the repository
2. Install platform-specific dependencies
3. Run the build script: `./build.sh all`
4. Copy the appropriate executable to your desired location

### Option 5: Direct Binary Usage

```bash
# Build without installation
./build.sh native
./navix-native /path/to/project
```

## Usage

```bash
# Unix/Linux/macOS
./navix-native /path/to/your/project

# Windows
navix-windows.exe C:\path\to\your\project
```

## Platform Support Matrix

| Platform | TUI Interface | Symbol Indexing | Autocomplete | File Watching | Package Installer |
|----------|---------------|-----------------|--------------|---------------|-------------------|
| Linux    | ✅ (ncurses)   | ✅              | ✅           | ✅            | ✅ (.deb)         |
| macOS    | ✅ (ncurses)   | ✅              | ✅           | ✅            | ✅ (.pkg)         |
| Windows  | ✅ (Console)   | ✅              | ✅           | ✅            | ✅ (.exe)         |

## Package Installer Features

### Linux Debian Package (.deb)

The Debian package provides:

- **System-wide installation**: Installs to `/usr/local/bin/navix`
- **Automatic dependency management**: Handles ncurses dependencies
- **Package manager integration**: Install/remove with `dpkg` or `apt`
- **Clean uninstall**: `sudo dpkg -r navix`

### macOS Package Installer (.pkg)

The macOS package installer provides:

- **System-wide installation**: Installs to `/usr/local/bin/navix`
- **Automatic PATH setup**: Available from any terminal
- **Man page integration**: Access help with `man navix`
- **Clean uninstall**: Proper removal of previous versions
- **Documentation**: Includes README, LICENSE, and CONTRIBUTING files
- **Installer scripts**: Pre/post-install automation
- **Metadata**: Proper package identification and versioning

### Windows Executable (.exe)

The Windows executable provides:

- **Portable binary**: No installation required
- **Static linking**: All dependencies included
- **Console interface**: Windows-native TUI experience
- **Easy distribution**: Single executable file

### Package Contents

#### Linux (.deb)
```
/usr/local/bin/navix                    # Main executable
```

#### macOS (.pkg)
```
/usr/local/bin/navix                    # Main executable
/usr/local/share/man/man1/navix.1       # Man page
/usr/local/share/doc/navix/README.md    # Documentation
/usr/local/share/doc/navix/LICENSE      # License
/usr/local/share/doc/navix/CONTRIBUTING.md  # Contributing guide
```

#### Windows (.exe)
```
navix-windows.exe                       # Portable executable
```

## Documentation

📚 **Complete Documentation Available!**

For comprehensive documentation including API reference, development guides, troubleshooting, and advanced usage examples, see:

**[📖 DOCUMENTATION.md](https://zidniryi.xyz/navix.html)**

### Documentation Sections:

- **📋 Overview** - Project features and supported languages
- **🏗️ Architecture** - Core components and class hierarchy  
- **⚙️ Installation** - Platform-specific setup instructions
- **🚀 Usage Guide** - Basic and advanced commands
- **📖 API Reference** - Complete class and method documentation
- **🔧 Development Guide** - Building from source and extending
- **🛠️ Troubleshooting** - Common issues and solutions
- **💡 Examples** - Practical usage and integration examples
- **🤝 Contributing** - Guidelines for contributors

### Quick Links:

- **[Installation Guide](DOCUMENTATION.md#installation)** - Get started quickly
- **[API Reference](DOCUMENTATION.md#api-reference)** - Complete API documentation
- **[Usage Examples](DOCUMENTATION.md#examples)** - Practical examples
- **[Troubleshooting](DOCUMENTATION.md#troubleshooting)** - Solve common issues

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on contributing to this project.

## License

This project is licensed under the terms specified in [LICENSE](LICENSE).

---

**Note**: The cross-platform support ensures that Navix works seamlessly across different operating systems while maintaining full functionality. The Windows version uses a simplified but fully functional console interface that provides the same core navigation and indexing capabilities. 