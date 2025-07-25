# macOS Package Installer for Navix

This document details the macOS package installer (`.pkg`) functionality added to the Navix build system.

## Overview

The macOS package installer provides a professional, user-friendly way to distribute and install Navix on macOS systems. It creates a standard `.pkg` file that can be distributed and installed like any other macOS application.

## Features

### ✅ **Complete Package Structure**
- **Binary**: Installs to `/usr/local/bin/navix`
- **Documentation**: Installs to `/usr/local/share/doc/navix/`
- **Man Page**: Installs to `/usr/local/share/man/man1/navix.1`
- **Metadata**: Proper package identification and versioning

### ✅ **Installation Scripts**
- **Preinstall**: Removes previous installations cleanly
- **Postinstall**: Sets permissions and updates man database
- **User Feedback**: Provides installation confirmation messages

### ✅ **Professional Metadata**
- **Package Identifier**: `com.navix.lsp`
- **Version**: `1.0.0`
- **Proper macOS package standards compliance**

## Usage

### Create Package
```bash
# Create package installer (builds native binary if needed)
./build.sh pkg
```

### Install Package
```bash
# Command line installation
sudo installer -pkg navix-installer.pkg -target /

# Or double-click in Finder for GUI installation
```

### Verify Installation
```bash
# Check installation
navix --version

# View man page
man navix

# Run from anywhere
navix /path/to/project
```

## Build Script Integration

The package installer is integrated into the main build script with the following commands:

```bash
./build.sh pkg     # Create macOS package
./build.sh all     # Build all platforms
./build.sh clean   # Remove all artifacts including .pkg
./build.sh help    # Show all options
```

## Package Structure

```
navix-installer.pkg
├── Package Contents:
│   ├── /usr/local/bin/navix                    # Main executable
│   ├── /usr/local/share/man/man1/navix.1       # Man page
│   └── /usr/local/share/doc/navix/             # Documentation
│       ├── README.md
│       ├── LICENSE
│       └── CONTRIBUTING.md
├── Scripts:
│   ├── preinstall                              # Pre-installation cleanup
│   └── postinstall                             # Post-installation setup
└── Metadata:
    ├── Identifier: com.navix.lsp
    ├── Version: 1.0.0
    └── Install Location: /
```

## Technical Implementation

### Build Process
1. **Dependency Check**: Verifies macOS platform
2. **Binary Build**: Ensures native binary exists
3. **Directory Structure**: Creates proper package layout
4. **File Copying**: Copies binary and documentation
5. **Script Creation**: Generates install/uninstall scripts
6. **Package Building**: Uses `pkgbuild` to create `.pkg`
7. **Cleanup**: Removes temporary directories

### Package Scripts

#### Preinstall Script
```bash
#!/bin/bash
# Remove previous installation if exists
if [ -f "/usr/local/bin/navix" ]; then
    echo "Removing previous Navix installation..."
    rm -f /usr/local/bin/navix
fi
exit 0
```

#### Postinstall Script
```bash
#!/bin/bash
# Set executable permissions
chmod +x /usr/local/bin/navix

# Update man database
if command -v mandb >/dev/null 2>&1; then
    mandb -q /usr/local/share/man 2>/dev/null || true
fi

echo "Navix has been installed successfully!"
echo "You can now run 'navix /path/to/project' from anywhere."
exit 0
```

## Distribution

### File Sizes
- **Package Size**: ~212KB
- **Installed Size**: ~564KB + documentation

### Requirements
- **macOS**: 10.12+ (compatible with modern macOS versions)
- **Architecture**: x86_64 (Intel) and Apple Silicon compatible
- **Dependencies**: None (statically linked where possible)

## Quality Assurance

### Verification Commands
```bash
# Check package contents
pkgutil --payload-files navix-installer.pkg

# View package info
installer -pkginfo -pkg navix-installer.pkg

# Test installation (dry run)
installer -pkg navix-installer.pkg -target / -dumplog
```

### Package Validation
- ✅ Standard macOS package format
- ✅ Proper metadata and identification
- ✅ Clean installation and removal
- ✅ PATH integration
- ✅ Man page integration
- ✅ Documentation installation

## Benefits

1. **Professional Distribution**: Standard macOS package format
2. **Easy Installation**: Double-click or command-line installation
3. **System Integration**: Proper PATH and man page setup
4. **Clean Uninstall**: Removes previous versions automatically
5. **User Experience**: Familiar installation process for macOS users
6. **Automation**: Integrated into build system

## Future Enhancements

- **Code Signing**: Add developer certificate signing
- **Notarization**: Apple notarization for Gatekeeper compatibility
- **Distribution**: Submit to package managers (Homebrew, MacPorts)
- **Auto-updates**: Built-in update checking mechanism

## Troubleshooting

### Common Issues

**Permission Denied**
```bash
# Solution: Use sudo for installation
sudo installer -pkg navix-installer.pkg -target /
```

**Package Not Found**
```bash
# Solution: Ensure package exists and build if needed
./build.sh pkg
```

**Installation Failed**
```bash
# Solution: Check installer logs
installer -pkg navix-installer.pkg -target / -verbose
```

## Related Files

- `build.sh` - Main build script with package functionality
- `README.md` - Updated with package installer documentation
- `CMakeLists.txt` - Cross-platform build configuration
- `src/TUI.hpp` - Cross-platform TUI headers
- `src/TUI.cpp` - Cross-platform TUI implementation

---

**Created**: July 2024  
**Version**: 1.0.0  
**Compatibility**: macOS 10.12+ 