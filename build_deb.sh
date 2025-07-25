#!/bin/bash

set -e

echo "🔧 Building navix binary..."

mkdir -p build
g++ -std=c++17 -pthread -o build/navix \
    src/main.cpp \
    src/FileScanner.cpp \
    src/Symbol.cpp \
    src/TUI.cpp \
    src/FileWatcher.cpp \
    src/PerformanceLogger.cpp \
    src/AutocompleteEngine.cpp \
    src/JsonExporter.cpp \
    src/LSPServer.cpp \
    -lncurses -lstdc++fs

echo "📦 Setting up Debian package structure..."

rm -rf navix_deb
mkdir -p navix_deb/usr/local/bin
mkdir -p navix_deb/DEBIAN

cp build/navix navix_deb/usr/local/bin/

cat <<EOF > navix_deb/DEBIAN/control
Package: navix
Version: 1.0.0
Section: utils
Priority: optional
Architecture: amd64
Maintainer: Zidni Ridwan <your@email.com>
Description: Navix - A TUI navigation app built with ncurses.
EOF

echo "📦 Building .deb package..."
dpkg-deb --build navix_deb

echo "✅ Done! Output: navix_deb.deb"
