# 📚 Navix Documentation

**Complete documentation for the Navix file navigator and symbol indexing tool**

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Installation](#installation)
4. [Usage Guide](#usage-guide)
5. [API Reference](#api-reference)
6. [Development Guide](#development-guide)
7. [Troubleshooting](#troubleshooting)
8. [Examples](#examples)
9. [Contributing](#contributing)

---

## Overview

Navix is a lightning-fast C++ file navigator with symbol indexing and autocomplete capabilities. It provides a powerful TUI (Text User Interface) for navigating codebases and finding symbols across multiple programming languages.

### Key Features

- **Multi-Language Support**: C++, TypeScript/JavaScript, Python, Go, Swift, Kotlin, Java, PHP, Ruby, Rust, Bash, and text files
- **Symbol Indexing**: Parse and index symbols from various programming languages
- **Autocomplete Engine**: Intelligent code completion with fuzzy matching
- **Cross-Platform**: Runs on Unix/Linux/macOS and Windows
- **Performance Monitoring**: Built-in performance profiling
- **LSP Server**: Language Server Protocol integration
- **File Watching**: Real-time file system monitoring
- **Export Formats**: JSON, ctags, and LSP-compatible output

### Supported Languages

| Language | File Extensions | Symbol Types |
|----------|----------------|--------------|
| C++ | `.cpp`, `.hpp`, `.cc`, `.hh`, `.cxx`, `.hxx` | Functions, Classes, Structs, Variables, Enums, Typedefs, Macros, Namespaces |
| TypeScript/JavaScript | `.ts`, `.js`, `.tsx`, `.jsx` | Functions, Classes, Interfaces, Types, Constants, Imports, Exports |
| Python | `.py`, `.pyw` | Functions, Classes, Methods, Variables, Imports, Decorators |
| Go | `.go` | Functions, Methods, Structs, Interfaces, Types, Variables, Constants |
| Swift | `.swift` | Functions, Methods, Classes, Structs, Protocols, Enums, Extensions |
| Kotlin | `.kt`, `.kts` | Functions, Classes, Objects, Variables, Properties |
| Java | `.java` | Methods, Classes, Interfaces, Variables, Constants |
| PHP | `.php` | Functions, Classes, Variables, Constants |
| Ruby | `.rb` | Methods, Classes, Modules, Variables, Constants |
| Rust | `.rs` | Functions, Structs, Enums, Traits, Variables |
| Bash/Shell | `.sh`, `.bash` | Functions, Variables, Aliases |
| Text | `.txt`, `.md`, `.rst` | Headers, URLs, Emails, TODOs, Notes |

---

## Architecture

### Core Components

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   FileScanner   │    │   SymbolIndex   │    │   TUI Engine    │
│                 │    │                 │    │                 │
│ • Scan files    │───▶│ • Parse symbols │───▶│ • Interactive   │
│ • Filter types  │    │ • Build index   │    │   interface     │
│ • Recursive     │    │ • Search        │    │ • Navigation    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  FileWatcher    │    │ Autocomplete    │    │ Performance     │
│                 │    │   Engine        │    │   Logger        │
│ • Watch changes │    │ • Fuzzy match   │    │ • Profile ops   │
│ • Real-time     │    │ • Prefix match  │    │ • Metrics       │
│ • Notifications │    │ • Suggestions   │    │ • Reports       │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Class Hierarchy

#### Core Classes

1. **SymbolIndex** - Main symbol parsing and indexing engine
2. **FileScanner** - File system scanning and filtering
3. **TUI** - Text User Interface for interactive navigation
4. **FileWatcher** - Real-time file system monitoring
5. **AutocompleteEngine** - Intelligent code completion
6. **PerformanceLogger** - Performance profiling and metrics
7. **JsonExporter** - Export symbols to various formats
8. **LSPServer** - Language Server Protocol implementation

#### Symbol Types

```cpp
enum class SymbolType {
    // C++ symbols
    FUNCTION, CLASS, STRUCT, VARIABLE, ENUM, TYPEDEF, MACRO, NAMESPACE,
    
    // TypeScript/JavaScript symbols
    JS_FUNCTION, JS_ARROW_FUNCTION, JS_CLASS, JS_INTERFACE, JS_TYPE,
    JS_CONST, JS_LET, JS_VAR, JS_IMPORT, JS_EXPORT, JS_MODULE,
    
    // Python symbols
    PY_FUNCTION, PY_CLASS, PY_METHOD, PY_VARIABLE, PY_IMPORT,
    PY_FROM_IMPORT, PY_DECORATOR, PY_LAMBDA,
    
    // Go symbols
    GO_FUNCTION, GO_METHOD, GO_STRUCT, GO_INTERFACE, GO_TYPE,
    GO_VARIABLE, GO_CONSTANT, GO_PACKAGE, GO_IMPORT,
    
    // Swift symbols
    SWIFT_FUNCTION, SWIFT_METHOD, SWIFT_CLASS, SWIFT_STRUCT,
    SWIFT_PROTOCOL, SWIFT_ENUM, SWIFT_EXTENSION, SWIFT_VARIABLE,
    SWIFT_CONSTANT, SWIFT_PROPERTY, SWIFT_INITIALIZER, SWIFT_SUBSCRIPT,
    SWIFT_IMPORT,
    
    // Text file symbols
    TXT_HEADER, TXT_SUBHEADER, TXT_URL, TXT_EMAIL, TXT_TODO,
    TXT_NOTE, TXT_FIXME, TXT_LINE, TXT_WORD,
    
    UNKNOWN
};
```

### Platform Support

#### Unix/Linux/macOS
- **TUI**: Full ncurses-based interface
- **Dependencies**: ncurses library
- **Features**: Advanced color support, window management

#### Windows
- **TUI**: Windows Console API
- **Dependencies**: None (statically linked)
- **Features**: Simplified but functional interface

---

## Installation

### Prerequisites

- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake** 3.10 or higher
- **Platform-specific dependencies** (see below)

### Platform-Specific Dependencies

#### Unix/Linux/macOS
```bash
# Ubuntu/Debian
sudo apt install libncurses5-dev pkg-config

# Fedora/RHEL
sudo dnf install ncurses-devel pkg-config

# macOS
brew install ncurses pkg-config
```

#### Windows
- No external dependencies required
- MinGW-w64 for cross-compilation (optional)

### Installation Methods

#### 1. Package Managers

**Linux (Debian/Ubuntu)**
```bash
# Build and install .deb package
./build_deb.sh
sudo dpkg -i navix_1.0.0_amd64.deb
```

**macOS**
```bash
# Build and install .pkg
./build.sh pkg
sudo installer -pkg navix-installer.pkg -target /
```

#### 2. Manual Build

```bash
# Clone repository
git clone https://github.com/zidniryi/navix.git
cd navix

# Build for current platform
./build.sh native

# Build for all platforms
./build.sh all

# Build for Windows (cross-compilation)
./build.sh windows
```

#### 3. CMake Build

```bash
mkdir build && cd build
cmake ..
make
```

### Verification

```bash
# Check installation
navix --version

# Test basic functionality
navix . --search main
```

---

## Usage Guide

### Basic Commands

#### 1. Scan Project
```bash
# Scan all supported files in current directory
navix .

# Scan specific directory
navix /path/to/project

# Scan with specific file types
navix . --cpp          # C++ files only
navix . --ts           # TypeScript/JavaScript files
navix . --py           # Python files only
```

#### 2. Search Symbols
```bash
# Search for symbols (fuzzy matching)
navix . --search functionName

# Exact search
navix . --search "exactSymbolName"

# Navigate to specific symbol
navix . --goto className
```

#### 3. Interactive Modes
```bash
# Interactive TUI mode
navix . --tui

# Interactive autocomplete
navix . --autocomplete

# Get completions for query
navix . --complete "partialName"
```

#### 4. Live Monitoring
```bash
# Live file watching
navix . --live

# Live mode with performance logging
navix . --live --perf
```

#### 5. Export Formats
```bash
# Export to JSON
navix . --json symbols.json

# Export compact JSON
navix . --json-compact

# Export with statistics
navix . --json-stats

# Export LSP-compatible JSON
navix . --json-lsp

# Export ctags format
navix . --export-tags
```

### Advanced Usage

#### Performance Monitoring
```bash
# Enable performance logging
navix . --perf

# View performance metrics
navix . --perf --json-stats
```

#### Language Server Protocol
```bash
# Start LSP server mode
navix . --lsp
```

#### Custom File Extensions
```bash
# Scan specific file extensions
navix . --ext .cpp .hpp .cc .hh
```

### Interactive TUI Commands

When in TUI mode (`--tui`), use these keyboard shortcuts:

| Key | Action |
|-----|--------|
| `j/k` | Navigate up/down |
| `Enter` | Open file at symbol |
| `Ctrl+f` | Search mode |
| `Ctrl+n` | Next search result |
| `Ctrl+p` | Previous search result |
| `q` | Quit |
| `h` | Help |
| `Ctrl+c` | Force quit |

---

## API Reference

### SymbolIndex Class

#### Constructor
```cpp
SymbolIndex();
```

#### Public Methods

##### `void addSymbol(const Symbol& symbol)`
Add a symbol to the index.

**Parameters:**
- `symbol` - Symbol object to add

**Example:**
```cpp
SymbolIndex index;
Symbol sym("main", SymbolType::FUNCTION, "main.cpp", 10);
index.addSymbol(sym);
```

##### `void buildIndex(const std::vector<std::string>& files)`
Build symbol index from a list of files.

**Parameters:**
- `files` - Vector of file paths to parse

**Example:**
```cpp
std::vector<std::string> files = {"main.cpp", "utils.hpp"};
index.buildIndex(files);
```

##### `std::vector<Symbol> search(const std::string& query, bool fuzzy = true)`
Search for symbols using fuzzy or exact matching.

**Parameters:**
- `query` - Search query string
- `fuzzy` - Enable fuzzy matching (default: true)

**Returns:**
- Vector of matching symbols

**Example:**
```cpp
auto results = index.search("main", true);
for (const auto& symbol : results) {
    std::cout << symbol.name << " in " << symbol.file << ":" << symbol.line << "\n";
}
```

##### `std::vector<Symbol> exactSearch(const std::string& query)`
Perform exact symbol search.

**Parameters:**
- `query` - Exact symbol name to search for

**Returns:**
- Vector of exact matches

##### `std::vector<Symbol> fuzzySearch(const std::string& query)`
Perform fuzzy symbol search using Levenshtein distance.

**Parameters:**
- `query` - Query string for fuzzy matching

**Returns:**
- Vector of fuzzy matches

##### `void clear()`
Clear all symbols from the index.

##### `size_t size() const`
Get the number of symbols in the index.

**Returns:**
- Number of symbols

##### `std::string symbolTypeToString(SymbolType type) const`
Convert symbol type enum to string representation.

**Parameters:**
- `type` - Symbol type enum

**Returns:**
- String representation of symbol type

### FileScanner Class

#### Static Methods

##### `std::vector<std::string> scanDirectory(const std::string& rootPath)`
Scan directory for all supported file types.

**Parameters:**
- `rootPath` - Root directory path

**Returns:**
- Vector of file paths

**Example:**
```cpp
auto files = FileScanner::scanDirectory("/path/to/project");
```

##### `std::vector<std::string> scanByExtensions(const std::string& rootPath, const std::vector<std::string>& extensions)`
Scan directory for specific file extensions.

**Parameters:**
- `rootPath` - Root directory path
- `extensions` - Vector of file extensions to include

**Returns:**
- Vector of matching file paths

**Example:**
```cpp
std::vector<std::string> exts = {".cpp", ".hpp"};
auto files = FileScanner::scanByExtensions("/path/to/project", exts);
```

### TUI Class

#### Constructor
```cpp
TUI(SymbolIndex& index);
```

#### Public Methods

##### `void run()`
Start the interactive TUI interface.

**Example:**
```cpp
SymbolIndex index;
TUI tui(index);
tui.run();
```

##### `void setSearchQuery(const std::string& query)`
Set the initial search query.

**Parameters:**
- `query` - Initial search query

### FileWatcher Class

#### Constructor
```cpp
FileWatcher(const std::string& path);
```

#### Public Methods

##### `void start()`
Start watching for file changes.

##### `void stop()`
Stop file watching.

##### `void setCallback(std::function<void(const std::string&)> callback)`
Set callback function for file change events.

**Parameters:**
- `callback` - Function to call when files change

**Example:**
```cpp
FileWatcher watcher("/path/to/project");
watcher.setCallback([](const std::string& file) {
    std::cout << "File changed: " << file << "\n";
});
watcher.start();
```

### AutocompleteEngine Class

#### Constructor
```cpp
AutocompleteEngine(const SymbolIndex& index);
```

#### Public Methods

##### `std::vector<std::string> getCompletions(const std::string& query)`
Get autocomplete suggestions for a query.

**Parameters:**
- `query` - Partial query string

**Returns:**
- Vector of completion suggestions

**Example:**
```cpp
AutocompleteEngine engine(index);
auto completions = engine.getCompletions("main");
```

### PerformanceLogger Class

#### Constructor
```cpp
PerformanceLogger();
```

#### Public Methods

##### `void startTimer(const std::string& operation)`
Start timing an operation.

**Parameters:**
- `operation` - Operation name

##### `void endTimer(const std::string& operation)`
End timing an operation.

**Parameters:**
- `operation` - Operation name

##### `void logMetric(const std::string& metric, double value)`
Log a performance metric.

**Parameters:**
- `metric` - Metric name
- `value` - Metric value

##### `std::string getReport() const`
Get performance report as string.

**Returns:**
- Formatted performance report

### JsonExporter Class

#### Static Methods

##### `void exportToJson(const SymbolIndex& index, const std::string& filename, bool compact = false)`
Export symbols to JSON file.

**Parameters:**
- `index` - Symbol index to export
- `filename` - Output filename
- `compact` - Use compact format (default: false)

**Example:**
```cpp
JsonExporter::exportToJson(index, "symbols.json", false);
```

##### `void exportToCtags(const SymbolIndex& index, const std::string& filename)`
Export symbols to ctags format.

**Parameters:**
- `index` - Symbol index to export
- `filename` - Output filename

---

## Development Guide

### Building from Source

#### Prerequisites
```bash
# Install build dependencies
# Ubuntu/Debian
sudo apt install build-essential cmake libncurses5-dev pkg-config

# macOS
brew install cmake ncurses pkg-config

# Windows
# Install Visual Studio or MinGW-w64
```

#### Build Process
```bash
# Clone repository
git clone https://github.com/zidniryi/navix.git
cd navix

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)

# Install (optional)
sudo make install
```

#### Development Build
```bash
# Configure with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build with debug information
make -j$(nproc)

# Run with debugging
gdb ./navix
```

### Adding Language Support

#### Step 1: Define Symbol Types
Add new symbol types to `SymbolType` enum in `src/Symbol.hpp`:

```cpp
enum class SymbolType {
    // ... existing types ...
    
    // New language symbols
    NEW_FUNCTION,
    NEW_CLASS,
    NEW_VARIABLE,
    // ...
};
```

#### Step 2: Add File Detection
Implement file detection method in `SymbolIndex` class:

```cpp
bool SymbolIndex::isNewLanguage(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".new" || ext == ".ext";
}
```

#### Step 3: Implement Parser
Add parsing method for the new language:

```cpp
void SymbolIndex::parseNewLanguage(const std::string& line, 
                                  const std::string& filePath, 
                                  int lineNumber) {
    // Implement language-specific parsing logic
    std::regex functionRegex(R"(\bfunction\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(line, match, functionRegex)) {
        addSymbol(Symbol(match[1].str(), SymbolType::NEW_FUNCTION, 
                        filePath, lineNumber, line));
    }
    
    // Add more patterns as needed
}
```

#### Step 4: Update String Conversion
Add string conversion for new symbol types:

```cpp
std::string SymbolIndex::symbolTypeToString(SymbolType type) const {
    switch (type) {
        // ... existing cases ...
        case SymbolType::NEW_FUNCTION: return "new-function";
        case SymbolType::NEW_CLASS: return "new-class";
        // ...
    }
}
```

#### Step 5: Add to File Scanner
Update file scanner to include new extensions:

```cpp
std::vector<std::string> FileScanner::scanForNewLanguage(const std::string& rootPath) {
    std::vector<std::string> extensions = {".new", ".ext"};
    return scanByExtensions(rootPath, extensions);
}
```

#### Step 6: Update CLI Interface
Add command-line option in `main.cpp`:

```cpp
} else if (mode == "--newlang") {
    files = FileScanner::scanForNewLanguage(rootPath);
    std::cout << "🔍 Scanning for New Language files in: " << rootPath << "\n";
    // ... output logic
}
```

### Testing

#### Unit Tests
```bash
# Run unit tests (if available)
make test

# Run specific test
./test_symbol_index
```

#### Manual Testing
```bash
# Test basic functionality
./navix . --search main
./navix . --goto FileScanner
./navix . --export-tags

# Test edge cases
./navix /nonexistent/path
./navix . --search ""
./navix . --goto NonexistentSymbol

# Test different file types
./navix . --cpp
./navix . --ts
./navix . --py
```

#### Performance Testing
```bash
# Test with large codebase
./navix /path/to/large/project --perf

# Compare performance
time ./navix . --search main
```

### Code Style

#### C++ Standards
- **C++17** compliance required
- Use **modern C++** features appropriately
- **RAII** for resource management
- **const-correctness** where applicable

#### Naming Conventions
```cpp
// Classes: PascalCase
class SymbolIndex {};

// Functions: camelCase  
void parseFile(const std::string& filePath);

// Variables: camelCase
std::string fileName;

// Constants: UPPER_SNAKE_CASE
const int MAX_SYMBOLS = 1000;

// Private members: trailing underscore
class Example {
private:
    int count_;
};
```

#### Code Organization
```cpp
// Header files (.hpp)
#ifndef FILENAME_HPP
#define FILENAME_HPP

#include <system_headers>
#include "local_headers.hpp"

class Example {
public:
    // Public interface first
    
private:
    // Private members last
};

#endif // FILENAME_HPP
```

---

## Troubleshooting

### Common Issues

#### 1. Build Errors

**Error: `ncurses.h: No such file or directory`**
```bash
# Ubuntu/Debian
sudo apt install libncurses5-dev

# macOS
brew install ncurses

# Fedora
sudo dnf install ncurses-devel
```

**Error: `pkg-config: command not found`**
```bash
# Ubuntu/Debian
sudo apt install pkg-config

# macOS
brew install pkg-config
```

#### 2. Runtime Errors

**Error: `navix: command not found`**
```bash
# Add to PATH
export PATH="/usr/local/bin:$PATH"

# Or create symlink
sudo ln -s /path/to/navix /usr/local/bin/navix
```

**Error: `Permission denied`**
```bash
# Make executable
chmod +x navix-native

# Or install properly
sudo make install
```

#### 3. Performance Issues

**Slow symbol indexing**
```bash
# Use specific file types only
navix . --cpp

# Enable performance logging
navix . --perf

# Check system resources
top
htop
```

**Memory usage high**
```bash
# Limit file scanning
navix . --ext .cpp .hpp

# Use smaller project subset
navix ./src
```

#### 4. Platform-Specific Issues

**Windows: Console not responding**
- Use Windows Terminal or PowerShell
- Ensure Windows Console API is available
- Try running as administrator

**macOS: ncurses not found**
```bash
# Install via Homebrew
brew install ncurses

# Set PKG_CONFIG_PATH
export PKG_CONFIG_PATH="/usr/local/opt/ncurses/lib/pkgconfig:$PKG_CONFIG_PATH"
```

**Linux: TUI not displaying correctly**
```bash
# Check terminal type
echo $TERM

# Set terminal type
export TERM=xterm-256color

# Check ncurses installation
pkg-config --modversion ncurses
```

### Debug Mode

#### Enable Debug Output
```bash
# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with debug output
./navix . --perf --json-stats
```

#### Verbose Mode
```bash
# Enable verbose output
./navix . --perf --verbose

# Check all available options
./navix --help
```

### Getting Help

#### Documentation
- Check this documentation file
- Read the README.md
- Review CONTRIBUTING.md

#### Community Support
- GitHub Issues: Report bugs and request features
- GitHub Discussions: Ask questions and share ideas
- Email maintainers: For private concerns

---

## Examples

### Basic Usage Examples

#### 1. Find All Functions
```bash
# Find all functions in C++ files
navix . --cpp --search "function"

# Find specific function
navix . --search "main"

# Find functions in specific file
navix . --search "parseFile"
```

#### 2. Navigate to Classes
```bash
# Find all classes
navix . --search "class"

# Navigate to specific class
navix . --goto "SymbolIndex"

# Find classes in specific language
navix . --cpp --search "class"
```

#### 3. Export Project Structure
```bash
# Export to JSON
navix . --json project_symbols.json

# Export compact format
navix . --json-compact compact_symbols.json

# Export with statistics
navix . --json-stats stats_symbols.json
```

### Advanced Examples

#### 1. Multi-Language Project Analysis
```bash
# Analyze mixed-language project
navix . --json mixed_languages.json

# Filter by specific languages
navix . --cpp --ts --py --json filtered.json
```

#### 2. Performance Analysis
```bash
# Profile symbol indexing
navix . --perf --json-stats performance.json

# Compare different file types
navix . --cpp --perf
navix . --ts --perf
navix . --py --perf
```

#### 3. Editor Integration
```bash
# Generate ctags for editor
navix . --export-tags .tags

# Start LSP server
navix . --lsp

# Export LSP-compatible format
navix . --json-lsp lsp_symbols.json
```

#### 4. Live Development
```bash
# Watch for changes during development
navix . --live --perf

# Interactive development
navix . --tui

# Autocomplete during coding
navix . --autocomplete
```

### Integration Examples

#### 1. Vim/Neovim Integration
```vim
" Add to .vimrc
set tags=.tags
set tags+=./.tags

" Use ctags file generated by Navix
:!navix . --export-tags .tags
```

#### 2. VS Code Integration
```json
// settings.json
{
    "typescript.preferences.includePackageJsonAutoImports": "on",
    "typescript.suggest.autoImports": true,
    "typescript.preferences.includeCompletionsForModuleExports": true
}
```

#### 3. Shell Aliases
```bash
# Add to .bashrc or .zshrc
alias nav='navix . --tui'
alias navs='navix . --search'
alias navg='navix . --goto'
alias nave='navix . --export-tags'
```

#### 4. Git Hooks
```bash
# .git/hooks/pre-commit
#!/bin/bash
navix . --export-tags .tags
git add .tags
```

### Scripting Examples

#### 1. Batch Processing
```bash
#!/bin/bash
# process_projects.sh

for project in /path/to/projects/*; do
    if [ -d "$project" ]; then
        echo "Processing $project"
        navix "$project" --json "$project"_symbols.json
    fi
done
```

#### 2. CI/CD Integration
```yaml
# .github/workflows/navix.yml
name: Navix Analysis
on: [push, pull_request]

jobs:
  analyze:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Navix
        run: |
          git clone https://github.com/zidniryi/navix.git
          cd navix
          ./build.sh native
          sudo cp navix-native /usr/local/bin/navix
      - name: Analyze Code
        run: navix . --json-stats analysis.json
      - name: Upload Results
        uses: actions/upload-artifact@v2
        with:
          name: navix-analysis
          path: analysis.json
```

#### 3. Performance Monitoring
```bash
#!/bin/bash
# monitor_performance.sh

echo "Starting Navix performance monitoring..."

# Baseline measurement
start_time=$(date +%s)
navix . --perf --json-stats baseline.json
baseline_time=$(( $(date +%s) - start_time ))

echo "Baseline time: ${baseline_time}s"

# Monitor over time
while true; do
    start_time=$(date +%s)
    navix . --perf --json-stats "perf_$(date +%Y%m%d_%H%M%S).json"
    current_time=$(( $(date +%s) - start_time ))
    
    echo "$(date): ${current_time}s"
    
    if [ $current_time -gt $((baseline_time * 2)) ]; then
        echo "WARNING: Performance degradation detected!"
    fi
    
    sleep 300  # Check every 5 minutes
done
```

---

## Contributing

### Getting Started

1. **Fork the repository**
2. **Clone your fork**
   ```bash
   git clone https://github.com/your-username/navix.git
   cd navix
   ```
3. **Create a feature branch**
   ```bash
   git checkout -b feature/your-amazing-feature
   ```
4. **Make your changes**
5. **Test thoroughly**
   ```bash
   ./build.sh native
   ./navix-native . --search test
   ```
6. **Commit with clear message**
   ```bash
   git commit -m "feat: add amazing new feature"
   ```
7. **Push and create pull request**

### Development Guidelines

#### Code Style
- Follow existing code style and conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Ensure C++17 compliance

#### Testing
- Test your changes on multiple platforms
- Add tests for new functionality
- Verify performance impact
- Test edge cases and error conditions

#### Documentation
- Update documentation for new features
- Add examples for new functionality
- Update API reference if needed
- Include usage examples

### Areas for Contribution

#### High Priority
- **Language Support**: Add support for new programming languages
- **Performance**: Optimize symbol parsing and indexing
- **Testing**: Add comprehensive test suite
- **Documentation**: Improve and expand documentation

#### Medium Priority
- **Editor Integration**: Enhance editor plugin support
- **Symbol Recognition**: Improve parsing accuracy
- **Export Formats**: Add new export formats
- **Configuration**: Add configuration file support

#### Nice to Have
- **GUI Interface**: Optional graphical interface
- **Plugins**: VS Code extension
- **Analytics**: Code metrics and insights
- **Themes**: Customizable output styling

### Reporting Issues

#### Before Reporting
- Check existing issues
- Try the latest version
- Search documentation for solutions

#### When Reporting Include
- **OS and version**
- **Compiler and version**
- **Steps to reproduce**
- **Expected vs actual behavior**
- **Error messages**
- **Sample files** (if applicable)

### Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Mentioned in release notes
- Thanked in the README

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- **GitHub Issues**: [Report bugs and request features](https://github.com/zidniryi/navix/issues)
- **GitHub Discussions**: [Ask questions and share ideas](https://github.com/zidniryi/navix/discussions)
- **Documentation**: This file and [README.md](README.md)
- **Contributing**: [CONTRIBUTING.md](CONTRIBUTING.md)

---

**Made with ❤️ for developers who value speed and simplicity.**

*Navix - Navigate & Index* 