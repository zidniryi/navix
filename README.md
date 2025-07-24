# 🚀 NAVIX - Navigate & Index

**Lightning-fast for your navigation**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/your-repo/navix)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![TypeScript](https://img.shields.io/badge/TypeScript-Support-blue.svg)](https://www.typescriptlang.org/)
[![JavaScript](https://img.shields.io/badge/JavaScript-Support-yellow.svg)](https://developer.mozilla.org/en-US/docs/Web/JavaScript)
[![Python](https://img.shields.io/badge/Python-Support-blue.svg)](https://python.org/)
[![ncurses](https://img.shields.io/badge/TUI-ncurses-green.svg)](https://invisible-island.net/ncurses/)

> 🎯 **Find. Navigate. Index.**
> 
> A powerful, cross-platform code navigation and indexing tool that supports C++, TypeScript, JavaScript, and Python. Built for developers who value speed and simplicity. Features both CLI and interactive TUI modes.

## ✨ Features

### 🔍 **Multi-Language Support**
- **C++**: `.cpp`, `.hpp`, `.h`, `.cc`, `.cxx`
- **TypeScript**: `.ts`, `.tsx`
- **JavaScript**: `.js`, `.jsx`, `.mjs`, `.cjs`
- **Python**: `.py`, `.pyw`, `.pyi`

### ⚡ **Lightning-Fast Symbol Indexing**
- Intelligent fuzzy search with ranking
- Exact symbol matching
- Cross-language symbol discovery
- Real-time file scanning

### 🖥️ **Interactive TUI Mode**
- **ncurses-based** text interface
- **Arrow key navigation** with smooth scrolling
- **Real-time fuzzy search** as you type
- **Dual-pane layout** with file preview
- **Tab switching** between files and symbols
- **Multi-language** symbol browsing

### 🎯 **Smart Symbol Recognition**
**C++ Symbols:**
- Functions, Classes, Structs
- Variables, Enums, Typedefs
- Macros, Namespaces

**TypeScript/JavaScript Symbols:**
- Functions (regular & arrow)
- Classes, Interfaces, Types
- Variables (const, let, var)
- Imports, Exports

**Python Symbols:**
- Functions, Classes, Methods
- Variables, Decorators
- Imports (from & import)
- Lambda functions

### 🚀 **Direct Editor Integration**
- **Supported Editors**: vim, nvim, VS Code, emacs, nano, Sublime Text, Atom
- **Auto-detection** of available editors
- **Jump-to-definition** with line precision
- **Environment variable** support (`$EDITOR`, `$VISUAL`)

### 📋 **Universal ctags Export**
- Standards-compliant ctags format
- IDE integration ready
- Multi-language symbol export
- Sorted and optimized output

### 🎨 **Beautiful CLI Interface**
- Modern, clean design
- Organized output with visual sections
- Emoji-enhanced feedback
- Professional error handling

## 🛠️ Installation

### Prerequisites
- **C++17** compatible compiler
- **CMake 3.10+**
- **ncurses library** (for TUI mode)
- **Git**

### Install Dependencies

**macOS (Homebrew):**
```bash
brew install ncurses cmake
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install libncurses5-dev libncursesw5-dev cmake build-essential
```

**Fedora/RHEL:**
```bash
sudo dnf install ncurses-devel cmake gcc-c++
```

### Build from Source

```bash
# Clone the repository
git clone https://github.com/zidniryi/navix.git
cd navix

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run Navix
./navix
```

### Quick Test
```bash
# Test in current directory
./navix .

# Launch interactive TUI
./navix . --tui

# Search for symbols
./navix . --search main

# Export tags
./navix . --export-tags
```

## 📖 Usage

### Basic Commands

```bash
# Scan all supported files
navix <project_root>

# Interactive TUI mode (recommended!)
navix <project_root> --tui

# Language-specific scanning
navix <project_root> --cpp              # C++ files only
navix <project_root> --ts               # TypeScript/JavaScript files
navix <project_root> --py               # Python files only

# Symbol search
navix <project_root> --search <symbol>  # Smart fuzzy search
navix <project_root> --search-exact <symbol>  # Exact match

# Navigation
navix <project_root> --goto <symbol>    # Jump to symbol
navix <project_root> --goto <symbol> vim  # Open in specific editor

# Export
navix <project_root> --export-tags      # Generate tags.txt
navix <project_root> --export-tags my.tags  # Custom filename
```

### Advanced Usage

```bash
# Filter by extensions
navix . --ext .cpp .hpp .ts .py

# Find specific files
navix . --name main.cpp package.json app.py

# Pattern matching
navix . --pattern test
```

## 🖥️ Interactive TUI Mode

The TUI (Text User Interface) provides the best Navix experience with real-time navigation and preview.

### Launch TUI
```bash
navix . --tui
```

### TUI Controls

| Key | Action | Description |
|-----|--------|-------------|
| `↑` `↓` | Navigate | Move through files/symbols |
| `Enter` | Open | Open selected file or jump to symbol |
| `Tab` | Switch Mode | Toggle between Files and Symbols view |
| `/` | Search | Clear search and start new query |
| `Backspace` | Delete | Remove character from search |
| `p` | Toggle Preview | Show/hide file preview pane |
| `q` / `ESC` | Quit | Exit TUI mode |
| `Type` | Filter | Real-time fuzzy search |

### TUI Features

#### **Dual-Pane Layout**
- **Left Pane**: File list or symbol list
- **Right Pane**: Live file preview (50 lines)
- **Split Screen**: Optimal use of terminal space

#### **Real-Time Search**
- **Fuzzy Matching**: Type to filter results instantly
- **Smart Scoring**: Best matches appear first
- **Live Preview**: See file contents as you navigate

#### **Two Modes**
- **Files Mode**: Browse and search all project files
- **Symbols Mode**: Navigate functions, classes, variables across languages

#### **Visual Feedback**
- **Color Coding**: Different colors for file types and symbol types
- **Status Bar**: Shows current mode, search query, and counts
- **Scroll Indicators**: Visual cues for long lists

## 🌟 Examples

### Quick Start
```bash
# Navigate to your project
cd /path/to/your/project

# Launch interactive mode (recommended)
navix . --tui
```

### TUI Workflow
```bash
# 1. Launch TUI
navix . --tui

# 2. Search for files
#    Type: main.cpp
#    Result: Shows matching files with preview

# 3. Switch to symbols (Tab key)
#    Type: App
#    Result: Shows all symbols containing "App"

# 4. Navigate with arrow keys
#    Enter: Opens file at symbol location
```

### Find Symbols
```bash
# Find all symbols containing "App"
navix . --search App

# Find exact symbol match
navix . --search-exact AppComponent

# Results show: symbol(type) in file:line
# Example output:
# 📍 AppComponent(js-class) in App.tsx:15
# 📍 App(js-function) in main.ts:8
# 📍 main(py-function) in app.py:23
```

### Navigate to Code
```bash
# Jump to main function
navix . --goto main

# Open in VS Code
navix . --goto AppComponent code

# Open in vim
navix . --goto process_data vim
```

### IDE Integration
```bash
# Generate ctags for your editor
navix . --export-tags

# Use with vim
vim -t AppComponent

# Use with emacs
emacs --eval "(find-tag \"AppComponent\")"
```

### Language-Specific Scanning
```bash
# C++ projects
navix . --cpp
# Found symbols: functions, classes, namespaces...

# TypeScript/React projects  
navix . --ts
# Found symbols: components, interfaces, types...

# Python projects
navix . --py
# Found symbols: functions, classes, decorators...

# Mixed codebases
navix .
# Found symbols from all supported languages
```

## 🔧 Editor Setup

### Environment Variables
```bash
# Set your preferred editor
export EDITOR=vim          # or code, emacs, nano, etc.
export VISUAL=code         # Alternative editor variable
```

### Vim Integration
```bash
# Generate tags
navix . --export-tags

# Add to .vimrc
set tags=./tags,tags

# Navigate in vim
:tag SymbolName
Ctrl+]  # Jump to definition
Ctrl+T  # Jump back
```

### VS Code Integration
```bash
# Direct navigation
navix . --goto SymbolName code

# Or set as default editor
export EDITOR=code
navix . --goto SymbolName
```

## 📊 Output Examples

### TUI Interface
```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         🚀 NAVIX - Navigate & Index                        │
└─────────────────────────────────────────────────────────────────────────────┘

┌─ Search Files ──────────────────┐
│ Search: App                     │
└─────────────────────────────────┘

┌─ Files ─────────────────────────┐  ┌─ Preview ───────────────────────┐
│ > src/App.tsx                   │  │ import React from 'react';      │
│   src/AppRouter.tsx             │  │ import './App.css';             │
│   src/components/AppHeader.tsx  │  │                                 │
│   src/utils/AppConfig.js        │  │ function App() {                │
│   tests/App.test.ts             │  │   return (                      │
│   ...                           │  │     <div className="App">       │
└─────────────────────────────────┘  │       <header>                  │
                                     │         <h1>Welcome</h1>        │
Files: 5/247 | Search: App    [FILES] │       </header>                 │
↑↓:Navigate | Enter:Open | Tab:Switch │     </div>                      │
                                     │   );                            │
                                     │ }                               │
                                     └─────────────────────────────────┘
```

### Symbol Search Results
```
┌─ RESULTS ──────────────────────────────────────────────────────────────────┐
│ Found 7 symbol(s):
│ 📍 AppComponent(js-class) in App.tsx:15
│ 📍 App(js-function) in main.ts:8  
│ 📍 AppRouter(js-class) in router.ts:23
│ 📍 appConfig(const) in config.js:5
│ 📍 Application(class) in main.cpp:12
│ 📍 app_init(py-function) in app.py:45
│ 📍 AppHandler(py-class) in handlers.py:8
└────────────────────────────────────────────────────────────────────────────┘
```

### File Scanning
```
┌─ FILES ────────────────────────────────────────────────────────────────────┐
│ Found 1247 matching files:
│ 📄 src/components/App.tsx
│ 📄 src/utils/helpers.ts
│ 📄 src/main.cpp
│ 📄 include/parser.hpp
│ 📄 scripts/deploy.py
│ 📄 tests/test_api.py
│ ...
└────────────────────────────────────────────────────────────────────────────┘
```

## 🎯 Supported Symbol Types

| Language | Types | Description |
|----------|-------|-------------|
| **C++** | `function`, `class`, `struct` | Core C++ constructs |
| | `variable`, `enum`, `typedef` | Data types |
| | `macro`, `namespace` | Preprocessor & organization |
| **TypeScript** | `js-function`, `arrow-func` | Function declarations |
| | `js-class`, `interface`, `type` | Type system |
| | `const`, `let`, `var` | Variables |
| **JavaScript** | `import`, `export` | Module system |
| **Python** | `py-function`, `py-class` | Core Python constructs |
| | `py-variable`, `py-decorator` | Variables & decorators |
| | `py-import`, `py-from-import` | Module imports |
| | `py-lambda` | Lambda functions |

## 💡 Tips & Best Practices

### TUI Tips
- **Start with TUI**: Always try `navix . --tui` first for the best experience
- **Use fuzzy search**: Type partial names for quick filtering
- **Preview files**: Keep preview pane open to understand context
- **Switch modes**: Use Tab to toggle between file and symbol browsing

### Performance Tips
- **Project scope**: Run Navix from your project root for best results
- **Language focus**: Use `--cpp`, `--ts`, `--py` for language-specific projects
- **Editor integration**: Export tags for seamless IDE/editor integration

### Workflow Integration
- **Tag generation**: Regular `navix . --export-tags` for editor users
- **Symbol navigation**: Use `--goto` for quick symbol jumping
- **Search refinement**: Start broad, then narrow with exact search

## 🤝 Contributing

We welcome contributions! Whether you're fixing bugs, adding features, or improving documentation, your help makes Navix better for everyone.

**👉 See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines:**
- 🚀 Quick start guide for new contributors
- 📋 Development setup and coding standards  
- 🧪 How to add support for new programming languages
- 🐛 Bug reporting and feature request processes
- 🎯 Areas where we need help

## 📄 License

This project is licensed under the **MIT License**.

**👉 See [LICENSE](LICENSE) for full license text.**

**TL;DR**: You can use, modify, and distribute this software freely. Just include the original license notice.

## 🙏 Acknowledgments

- **Inspired by** ctags, cscope, and modern language servers
- **Built with** modern C++17 and ncurses libraries
- **TUI powered by** ncurses for cross-platform terminal interfaces
- **Designed for** developer productivity and code navigation
- **Made with ❤️** for developers who value speed and simplicity

## 📞 Support

- **GitHub Issues**: [Report bugs & request features](https://github.com/zidniryi/navix/issues)
- **Discussions**: [Community discussions](https://github.com/zidniryi/navix/discussions)
- **Documentation**: This README and inline code comments

---

**⭐ Star this repo if Navix helps you navigate code faster!**

*Made with ❤️ by developers, for developers.* 