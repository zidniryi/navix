# 🤝 Contributing to Navix

We love your input! We want to make contributing to Navix as easy and transparent as possible, whether it's:

- Reporting a bug
- Discussing the current state of the code
- Submitting a fix
- Proposing new features
- Adding language support
- Becoming a maintainer

## 🚀 Quick Start Guide

### 1. Fork & Clone
```bash
# Fork the repo on GitHub, then:
git clone https://github.com/your-username/navix.git
cd navix
```

### 2. Set Up Development Environment
```bash
# Create build directory
mkdir build && cd build

# Configure with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make

# Test your build
./navix . --search main
```

### 3. Make Your Changes
```bash
# Create a feature branch
git checkout -b feature/your-amazing-feature

# Make your changes
# ... edit code ...

# Test thoroughly
./navix . --search test
./navix . --goto MyFunction

# Commit with clear message
git commit -m "feat: add amazing new feature"
```

### 4. Submit Pull Request
```bash
# Push to your fork
git push origin feature/your-amazing-feature

# Open PR on GitHub with description
```

## 📋 Development Guidelines

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

### Documentation
- **Comment complex algorithms**
- **Document public interfaces**
- **Use clear variable names**
- **Add examples for new features**

### Error Handling
```cpp
// Graceful failure with helpful messages
if (!file.is_open()) {
    std::cerr << "Error: Could not open file: " << filePath << "\n";
    return false;
}

// Use exceptions sparingly, prefer error codes
try {
    // risky operation
} catch (const std::exception& ex) {
    // handle gracefully
}
```

## 🎯 Areas for Contribution

### 🌟 High Priority
- **Language Support**: Python, Rust, Go, Java
- **Performance**: Faster parsing algorithms
- **Testing**: Unit tests, integration tests
- **Documentation**: More examples, tutorials

### 🔧 Medium Priority  
- **Editor Integration**: More editor support
- **Symbol Recognition**: Better parsing accuracy
- **Export Formats**: LSP, JSON output
- **Configuration**: Config file support

### 💡 Nice to Have
- **GUI Interface**: Optional graphical interface
- **Plugins**: VS Code extension
- **Analytics**: Code metrics and insights
- **Themes**: Customizable output styling

## 🧪 Adding Language Support

### Step-by-Step Guide

1. **Extend Symbol Types**
```cpp
// In src/Symbol.hpp
enum class SymbolType {
    // ... existing types ...
    
    // Python symbols
    PY_FUNCTION,
    PY_CLASS,
    PY_VARIABLE,
    // ...
};
```

2. **Add File Detection**
```cpp
// In src/Symbol.cpp
bool SymbolIndex::isPython(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".py" || ext == ".pyw";
}
```

3. **Implement Parser**
```cpp
void SymbolIndex::parsePython(const std::string& line, 
                              const std::string& filePath, 
                              int lineNumber) {
    // Python-specific parsing logic
    std::regex classRegex(R"(\bclass\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        addSymbol(Symbol(match[1].str(), SymbolType::PY_CLASS, 
                        filePath, lineNumber, line));
    }
    // ... more patterns
}
```

4. **Update String Conversion**
```cpp
std::string SymbolIndex::symbolTypeToString(SymbolType type) const {
    switch (type) {
        // ... existing cases ...
        case SymbolType::PY_FUNCTION: return "py-function";
        case SymbolType::PY_CLASS: return "py-class";
        // ...
    }
}
```

5. **Add to File Scanner**
```cpp
// In src/FileScanner.cpp
std::vector<std::string> FileScanner::scanForPython(const std::string& rootPath) {
    std::vector<std::string> pyExtensions = {".py", ".pyw"};
    return scanByExtensions(rootPath, pyExtensions);
}
```

6. **Update CLI Interface**
```cpp
// In src/main.cpp
} else if (mode == "--py") {
    files = FileScanner::scanForPython(rootPath);
    std::cout << "🔍 Scanning for Python files in: " << rootPath << "\n";
    // ... output logic
}
```

## 🐛 Bug Reports

### Before Reporting
- Check [existing issues](https://github.com/your-username/navix/issues)
- Try the latest version
- Search for solutions in documentation

### When Reporting Include:
- **OS and version** (e.g., macOS 13.4, Ubuntu 22.04)
- **Compiler** (e.g., GCC 11.2, Clang 14.0)
- **CMake version**
- **Steps to reproduce**
- **Expected vs actual behavior**
- **Sample files** (if applicable)
- **Error messages** (full output)

### Bug Report Template
```markdown
**Environment:**
- OS: 
- Compiler: 
- CMake: 
- Navix version: 

**Describe the bug:**
A clear description of what the bug is.

**To Reproduce:**
1. Run command: `navix . --search Symbol`
2. Expected: Find symbol
3. Actual: Error message

**Additional context:**
Any other context about the problem.
```

## 💡 Feature Requests

### Before Requesting
- Check if feature already exists
- Search existing feature requests
- Consider implementation complexity

### Great Feature Requests Include:
- **Clear use case**: Why is this needed?
- **Specific behavior**: What should it do?
- **Examples**: How would it work?
- **Alternatives**: What workarounds exist?

## 🧪 Testing

### Manual Testing
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
./navix . --ext .py .rb
```

### Test Coverage Areas
- **File scanning**: Different project structures
- **Symbol parsing**: Various language constructs  
- **Search functionality**: Edge cases, performance
- **Editor integration**: Different editors
- **Error handling**: Invalid inputs, missing files

## 📝 Pull Request Process

### PR Checklist
- [ ] **Descriptive title** and description
- [ ] **Linked issue** (if applicable)
- [ ] **Code follows** style guidelines
- [ ] **Tests pass** (manual testing)
- [ ] **Documentation updated** (if needed)
- [ ] **No breaking changes** (or clearly documented)

### PR Template
```markdown
## Description
Brief description of changes.

## Type of Change
- [ ] Bug fix
- [ ] New feature  
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Tested on multiple file types
- [ ] Verified editor integration
- [ ] Checked error handling

## Screenshots/Examples
(If applicable)
```

## 👥 Code Review

### As a Reviewer
- **Be respectful** and constructive
- **Focus on code**, not the person
- **Suggest improvements** with examples
- **Ask questions** for clarification
- **Approve** when ready or **request changes**

### As an Author
- **Respond to feedback** promptly
- **Make requested changes** or explain why not
- **Keep discussions** technical and professional
- **Thank reviewers** for their time

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

## 🎉 Recognition

Contributors will be:
- **Listed** in CONTRIBUTORS.md
- **Mentioned** in release notes
- **Thanked** in the README

## 📞 Questions?

- **GitHub Discussions**: For general questions
- **GitHub Issues**: For bugs and features  
- **Email maintainers**: For private concerns

---

**Thank you for contributing to Navix! 🚀**

*Every contribution, no matter how small, makes Navix better for everyone.* 