#!/bin/bash

# Build script for Navix - Cross-platform C++ file navigator
# Usage: ./build.sh [windows|native|all|pkg|clean|help]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Source files
SOURCES="src/main.cpp src/FileScanner.cpp src/Symbol.cpp src/TUI.cpp src/FileWatcher.cpp src/PerformanceLogger.cpp src/AutocompleteEngine.cpp src/JsonExporter.cpp src/LSPServer.cpp"

# Common compiler flags
COMMON_FLAGS="-std=c++17 -O2"

# Package information
PKG_IDENTIFIER="com.navix.lsp"
PKG_VERSION="1.0.0"
PKG_NAME="navix-installer"

echo -e "${BLUE}🚀 Navix Build Script${NC}"
echo -e "${BLUE}=====================${NC}"

build_native() {
    echo -e "\n${YELLOW}Building for native platform (Unix/Linux/macOS)...${NC}"
    
    # Check if ncurses is available
    if ! pkg-config --exists ncurses; then
        echo -e "${RED}❌ Error: ncurses development package not found${NC}"
        echo -e "${RED}   Please install: libncurses5-dev (Ubuntu/Debian) or ncurses (macOS)${NC}"
        return 1
    fi
    
    g++ $COMMON_FLAGS -o navix-native $SOURCES $(pkg-config --cflags --libs ncurses)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Native build successful: navix-native${NC}"
    else
        echo -e "${RED}❌ Native build failed${NC}"
        return 1
    fi
}

build_windows() {
    echo -e "\n${YELLOW}Building for Windows (cross-compilation)...${NC}"
    
    # Check if MinGW cross-compiler is available
    if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
        echo -e "${RED}❌ Error: MinGW-w64 cross-compiler not found${NC}"
        echo -e "${RED}   Please install: mingw-w64 package${NC}"
        echo -e "${RED}   macOS: brew install mingw-w64${NC}"
        echo -e "${RED}   Ubuntu: sudo apt install mingw-w64${NC}"
        return 1
    fi
    
    x86_64-w64-mingw32-g++ $COMMON_FLAGS -D_WIN32 -o navix-windows.exe $SOURCES -static-libgcc -static-libstdc++
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Windows build successful: navix-windows.exe${NC}"
    else
        echo -e "${RED}❌ Windows build failed${NC}"
        return 1
    fi
}

create_macos_package() {
    echo -e "\n${YELLOW}Creating macOS package installer...${NC}"
    
    # Check if we're on macOS
    if [[ "$OSTYPE" != "darwin"* ]]; then
        echo -e "${RED}❌ Error: macOS package creation is only supported on macOS${NC}"
        return 1
    fi
    
    # Check if native binary exists
    if [ ! -f "navix-native" ]; then
        echo -e "${YELLOW}📦 Native binary not found, building first...${NC}"
        build_native
        if [ $? -ne 0 ]; then
            echo -e "${RED}❌ Failed to build native binary for package${NC}"
            return 1
        fi
    fi
    
    # Create package root directory structure
    PKG_ROOT="pkg-root"
    PKG_INSTALL_PATH="$PKG_ROOT/usr/local/bin"
    PKG_DOC_PATH="$PKG_ROOT/usr/local/share/doc/navix"
    PKG_MAN_PATH="$PKG_ROOT/usr/local/share/man/man1"
    
    echo -e "${BLUE}📁 Creating package directory structure...${NC}"
    rm -rf "$PKG_ROOT"
    mkdir -p "$PKG_INSTALL_PATH"
    mkdir -p "$PKG_DOC_PATH"
    mkdir -p "$PKG_MAN_PATH"
    
    # Copy binary
    echo -e "${BLUE}📋 Copying binary and documentation...${NC}"
    cp navix-native "$PKG_INSTALL_PATH/navix"
    chmod +x "$PKG_INSTALL_PATH/navix"
    
    # Copy documentation
    cp README.md "$PKG_DOC_PATH/"
    cp LICENSE "$PKG_DOC_PATH/"
    cp CONTRIBUTING.md "$PKG_DOC_PATH/"
    
    # Create man page
    create_man_page "$PKG_MAN_PATH/navix.1"
    
    # Create package scripts directory
    PKG_SCRIPTS="pkg-scripts"
    mkdir -p "$PKG_SCRIPTS"
    
    # Create postinstall script
    cat > "$PKG_SCRIPTS/postinstall" << 'EOF'
#!/bin/bash
# Postinstall script for Navix

# Make sure the binary is executable
chmod +x /usr/local/bin/navix

# Update man database if available
if command -v mandb >/dev/null 2>&1; then
    mandb -q /usr/local/share/man 2>/dev/null || true
fi

echo "Navix has been installed successfully!"
echo "You can now run 'navix /path/to/project' from anywhere."
echo "For help, run 'navix --help' or 'man navix'."

exit 0
EOF
    
    # Create preinstall script
    cat > "$PKG_SCRIPTS/preinstall" << 'EOF'
#!/bin/bash
# Preinstall script for Navix

# Check if old installation exists and remove it
if [ -f "/usr/local/bin/navix" ]; then
    echo "Removing previous Navix installation..."
    rm -f /usr/local/bin/navix
fi

exit 0
EOF
    
    chmod +x "$PKG_SCRIPTS/postinstall"
    chmod +x "$PKG_SCRIPTS/preinstall"
    
    # Build the package
    echo -e "${BLUE}📦 Building package...${NC}"
    pkgbuild --root "$PKG_ROOT" \
             --identifier "$PKG_IDENTIFIER" \
             --version "$PKG_VERSION" \
             --install-location / \
             --scripts "$PKG_SCRIPTS" \
             "$PKG_NAME.pkg"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ macOS package created successfully: $PKG_NAME.pkg${NC}"
        
        # Clean up temporary directories
        rm -rf "$PKG_ROOT" "$PKG_SCRIPTS"
        
        # Show package info
        echo -e "${BLUE}📋 Package Information:${NC}"
        echo -e "   Package: $PKG_NAME.pkg"
        echo -e "   Identifier: $PKG_IDENTIFIER"
        echo -e "   Version: $PKG_VERSION"
        echo -e "   Size: $(du -h $PKG_NAME.pkg | cut -f1)"
        
        echo -e "\n${YELLOW}💡 Installation Instructions:${NC}"
        echo -e "   1. Double-click $PKG_NAME.pkg to install"
        echo -e "   2. Or use: sudo installer -pkg $PKG_NAME.pkg -target /"
        echo -e "   3. After installation, run: navix /path/to/project"
    else
        echo -e "${RED}❌ macOS package creation failed${NC}"
        rm -rf "$PKG_ROOT" "$PKG_SCRIPTS"
        return 1
    fi
}

create_man_page() {
    local man_file="$1"
    cat > "$man_file" << 'EOF'
.TH NAVIX 1 "2024" "Navix 1.0.0" "User Commands"
.SH NAME
navix \- Navigate & Index - A powerful C++ file navigator with symbol indexing
.SH SYNOPSIS
.B navix
[\fIOPTION\fR] [\fIPATH\fR]
.SH DESCRIPTION
Navix is a cross-platform file navigator and symbol indexer that provides fast file browsing, symbol searching, and code navigation capabilities. It features both command-line and interactive TUI modes.
.SH OPTIONS
.TP
.B \-\-tui
Launch interactive TUI (Text User Interface) mode
.TP
.B \-\-search \fITERM\fR
Search for symbols containing TERM
.TP
.B \-\-export\-tags
Export symbols in ctags format
.TP
.B \-\-help
Display help information
.TP
.B \-\-version
Display version information
.SH TUI CONTROLS
.TP
.B ↑/↓
Navigate through files or symbols
.TP
.B Enter
Open selected file or jump to symbol
.TP
.B Tab
Switch between Files and Symbols mode
.TP
.B /
Start new search
.TP
.B p
Toggle preview pane
.TP
.B q/ESC
Quit application
.SH EXAMPLES
.TP
.B navix .
Scan current directory
.TP
.B navix /path/to/project \-\-tui
Launch TUI mode for project
.TP
.B navix . \-\-search main
Search for symbols containing "main"
.SH FILES
.TP
.B ~/.navix/
Configuration directory (if applicable)
.SH SEE ALSO
.BR ctags (1),
.BR find (1),
.BR grep (1)
.SH AUTHOR
Navix development team
.SH BUGS
Report bugs at: https://github.com/zidniryi/navix/issues
EOF
}

show_help() {
    echo -e "\nUsage: $0 [windows|native|all|pkg|clean|help]"
    echo -e ""
    echo -e "Commands:"
    echo -e "  native   - Build for current platform (requires ncurses)"
    echo -e "  windows  - Cross-compile for Windows (requires MinGW-w64)"
    echo -e "  all      - Build for both platforms"
    echo -e "  pkg      - Create macOS package installer (macOS only)"
    echo -e "  clean    - Remove build artifacts"
    echo -e "  help     - Show this help message"
    echo -e ""
    echo -e "Examples:"
    echo -e "  $0 all       # Build for both platforms"
    echo -e "  $0 windows   # Build only Windows version"
    echo -e "  $0 pkg       # Create macOS .pkg installer"
}

clean_builds() {
    echo -e "\n${YELLOW}Cleaning build artifacts...${NC}"
    rm -f navix-native navix-windows.exe navix-installer.pkg
    rm -rf pkg-root pkg-scripts
    echo -e "${GREEN}✅ Clean complete${NC}"
}

# Main execution
case "${1:-all}" in
    "native")
        build_native
        ;;
    "windows")
        build_windows
        ;;
    "all")
        build_native
        build_windows
        ;;
    "pkg")
        create_macos_package
        ;;
    "clean")
        clean_builds
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        echo -e "${RED}❌ Unknown command: $1${NC}"
        show_help
        exit 1
        ;;
esac

if [ "$1" != "clean" ] && [ "$1" != "help" ] && [ "$1" != "-h" ] && [ "$1" != "--help" ]; then
    echo -e "\n${BLUE}📋 Build Summary${NC}"
    echo -e "${BLUE}=================${NC}"
    if [ -f navix-native ]; then
        echo -e "${GREEN}✅ Native executable: navix-native ($(du -h navix-native | cut -f1))${NC}"
    fi
    if [ -f navix-windows.exe ]; then
        echo -e "${GREEN}✅ Windows executable: navix-windows.exe ($(du -h navix-windows.exe | cut -f1))${NC}"
    fi
    if [ -f navix-installer.pkg ]; then
        echo -e "${GREEN}✅ macOS package: navix-installer.pkg ($(du -h navix-installer.pkg | cut -f1))${NC}"
    fi
    
    echo -e "\n${YELLOW}💡 Usage Tips:${NC}"
    echo -e "   ./navix-native /path/to/project              # Run on Unix/Linux/macOS"
    echo -e "   wine navix-windows.exe C:\\path\\to\\project    # Test Windows version on Unix"
    if [ -f navix-installer.pkg ]; then
        echo -e "   sudo installer -pkg navix-installer.pkg -target /  # Install macOS package"
    fi
fi 