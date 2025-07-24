#include "FileScanner.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <set>
#include <iomanip>
#include <thread>
#include <atomic>

namespace fs = std::filesystem;

std::vector<std::string> FileScanner::scanForCppFiles(const std::string& rootPath) {
    std::vector<std::string> defaultExtensions = {".cpp", ".hpp", ".h", ".cc", ".cxx"};
    return scanByExtensions(rootPath, defaultExtensions);
}

std::vector<std::string> FileScanner::scanForAllSupportedFiles(const std::string& rootPath) {
    std::vector<std::string> allExtensions = {
        // C++ files
        ".cpp", ".hpp", ".h", ".cc", ".cxx",
        // TypeScript/JavaScript files
        ".ts", ".tsx", ".js", ".jsx", ".mjs", ".cjs",
        // Python files
        ".py", ".pyw", ".pyi",
        // Go files
        ".go"
    };
    return scanByExtensions(rootPath, allExtensions);
}

std::vector<std::string> FileScanner::scanForTypeScriptJavaScript(const std::string& rootPath) {
    std::vector<std::string> tsJsExtensions = {".ts", ".tsx", ".js", ".jsx", ".mjs", ".cjs"};
    return scanByExtensions(rootPath, tsJsExtensions);
}

std::vector<std::string> FileScanner::scanForPython(const std::string& rootPath) {
    std::vector<std::string> pythonExtensions = {".py", ".pyw", ".pyi"};
    return scanByExtensions(rootPath, pythonExtensions);
}

std::vector<std::string> FileScanner::scanForGo(const std::string& rootPath) {
    std::vector<std::string> goExtensions = {".go"};
    return scanByExtensions(rootPath, goExtensions);
}

std::vector<std::string> FileScanner::scanByExtensions(const std::string& rootPath, const std::vector<std::string>& extensions) {
    std::vector<std::string> files;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& ex) {
        // Handle filesystem errors gracefully
    }

    return files;
}

std::vector<std::string> FileScanner::scanByFilenames(const std::string& rootPath, const std::vector<std::string>& filenames) {
    std::vector<std::string> files;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (std::find(filenames.begin(), filenames.end(), filename) != filenames.end()) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& ex) {
        // Handle filesystem errors gracefully
    }

    return files;
}

std::vector<std::string> FileScanner::scanByPattern(const std::string& rootPath, const std::string& pattern) {
    std::vector<std::string> files;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find(pattern) != std::string::npos) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& ex) {
        // Handle filesystem errors gracefully
    }

    return files;
}

SymbolIndex FileScanner::buildSymbolIndex(const std::vector<std::string>& files, bool showProgressFlag) {
    SymbolIndex index;
    
    if (showProgressFlag && !files.empty()) {
        std::cout << "🔨 Building symbol index...\n";
        
        for (size_t i = 0; i < files.size(); i++) {
            showProgress("Indexing", i + 1, files.size());
            
            // Create a temporary index for this file
            SymbolIndex tempIndex;
            tempIndex.buildIndex({files[i]});
            
            // Small delay to show progress (remove in production if too slow)
            if (files.size() < 100) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
        
        clearLine();
        std::cout << "✅ Symbol index built successfully! (" << files.size() << " files processed)\n";
    }
    
    index.buildIndex(files);
    return index;
}

std::vector<Symbol> FileScanner::searchSymbols(const std::string& rootPath, const std::string& query, bool fuzzy, bool showProgressFlag) {
    if (showProgressFlag) {
        // Show loading animation
        printWithSpinner("🔍 Scanning files");
    }
    
    // Get all supported files in the directory (C++, TypeScript, JavaScript, Python)
    std::vector<std::string> allFiles = scanForAllSupportedFiles(rootPath);
    
    if (showProgressFlag) {
        clearLine();
        std::cout << "📁 Found " << allFiles.size() << " files\n";
        
        printWithSpinner("🔍 Building symbol index");
    }
    
    // Build symbol index with progress
    SymbolIndex index = buildSymbolIndex(allFiles, false); // Don't show double progress
    
    if (showProgressFlag) {
        clearLine();
        printWithSpinner("🔍 Searching for '" + query + "'");
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Brief pause for effect
        clearLine();
    }
    
    // Search for symbols
    return index.search(query, fuzzy);
}

bool FileScanner::gotoSymbol(const std::string& rootPath, const std::string& symbolName, const std::string& editor) {
    // Search for the symbol
    std::vector<Symbol> symbols = searchSymbols(rootPath, symbolName, false); // Use exact search for goto
    
    if (symbols.empty()) {
        // Try fuzzy search if exact fails
        symbols = searchSymbols(rootPath, symbolName, true);
        if (symbols.empty()) {
            std::cout << "❌ Symbol '" << symbolName << "' not found.\n";
            return false;
        }
    }
    
    // Use the first match (best match)
    const Symbol& symbol = symbols[0];
    
    std::cout << "✅ Found: " << formatSymbolLocation(symbol) << "\n";
    
    // Open in editor
    return openInEditor(symbol.file, symbol.line, editor);
}

void FileScanner::exportTags(const std::string& rootPath, const std::string& outputFile) {
    // Show progress
    printWithSpinner("🔍 Scanning all supported files");
    
    // Get all supported files
    std::vector<std::string> allFiles = scanForAllSupportedFiles(rootPath);
    
    clearLine();
    std::cout << "📁 Found " << allFiles.size() << " files\n";
    
    // Build symbol index
    SymbolIndex index = buildSymbolIndex(allFiles, true);
    
    printWithSpinner("📋 Generating ctags file");
    
    // Open output file
    std::ofstream tagsFile(outputFile);
    if (!tagsFile.is_open()) {
        clearLine();
        std::cerr << "❌ Error: Could not create tags file: " << outputFile << "\n";
        return;
    }
    
    // Write ctags header
    tagsFile << "!_TAG_FILE_FORMAT\t2\t/extended format; --format=1 will not append ;\" to lines/\n";
    tagsFile << "!_TAG_FILE_SORTED\t1\t/0=unsorted, 1=sorted, 2=foldcase/\n";
    tagsFile << "!_TAG_PROGRAM_AUTHOR\tNavix\t/navix@github.com/\n";
    tagsFile << "!_TAG_PROGRAM_NAME\tnavix\t//\n";
    tagsFile << "!_TAG_PROGRAM_VERSION\t1.0\t//\n";
    
    // Collect all symbols and sort them
    std::vector<Symbol> allSymbols;
    
    // We need to manually collect symbols since SymbolIndex doesn't expose them
    // Let's rebuild and extract
    SymbolIndex tempIndex;
    tempIndex.buildIndex(allFiles);
    
    // Search for common patterns to get symbols (this is a workaround)
    std::vector<std::string> commonPatterns = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
    };
    
    std::set<std::string> addedSymbols; // To avoid duplicates
    
    for (const auto& pattern : commonPatterns) {
        auto symbols = tempIndex.search(pattern, true);
        for (const auto& symbol : symbols) {
            std::string key = symbol.name + symbol.file + std::to_string(symbol.line);
            if (addedSymbols.find(key) == addedSymbols.end()) {
                allSymbols.push_back(symbol);
                addedSymbols.insert(key);
            }
        }
    }
    
    // Sort symbols by name
    std::sort(allSymbols.begin(), allSymbols.end(),
              [](const Symbol& a, const Symbol& b) {
                  return a.name < b.name;
              });
    
    // Write tags in ctags format
    for (const auto& symbol : allSymbols) {
        // Format: symbol\tfile\t/^pattern$/;\"\tkind
        std::string kind;
        switch (symbol.type) {
            case SymbolType::FUNCTION: 
            case SymbolType::JS_FUNCTION: 
            case SymbolType::JS_ARROW_FUNCTION:
            case SymbolType::PY_FUNCTION:
            case SymbolType::GO_FUNCTION:
            case SymbolType::GO_METHOD: kind = "f"; break;
            case SymbolType::CLASS: 
            case SymbolType::JS_CLASS:
            case SymbolType::PY_CLASS: kind = "c"; break;
            case SymbolType::STRUCT:
            case SymbolType::GO_STRUCT: kind = "s"; break;
            case SymbolType::VARIABLE: 
            case SymbolType::JS_CONST: 
            case SymbolType::JS_LET: 
            case SymbolType::JS_VAR:
            case SymbolType::PY_VARIABLE:
            case SymbolType::GO_VARIABLE: kind = "v"; break;
            case SymbolType::ENUM: kind = "e"; break;
            case SymbolType::TYPEDEF: 
            case SymbolType::JS_TYPE:
            case SymbolType::GO_TYPE: kind = "t"; break;
            case SymbolType::MACRO: kind = "d"; break;
            case SymbolType::NAMESPACE: kind = "n"; break;
            case SymbolType::JS_INTERFACE:
            case SymbolType::GO_INTERFACE: kind = "i"; break;
            case SymbolType::JS_IMPORT: 
            case SymbolType::JS_EXPORT:
            case SymbolType::PY_IMPORT:
            case SymbolType::PY_FROM_IMPORT:
            case SymbolType::GO_IMPORT: kind = "m"; break;
            case SymbolType::PY_DECORATOR: kind = "a"; break;
            case SymbolType::PY_LAMBDA: kind = "l"; break;
            case SymbolType::GO_CONSTANT: kind = "d"; break;
            case SymbolType::GO_PACKAGE: kind = "p"; break;
            default: kind = "x"; break;
        }
        
        // Convert absolute path to relative
        std::string relativeFile = symbol.file;
        if (relativeFile.find(rootPath) == 0) {
            relativeFile = relativeFile.substr(rootPath.length());
            if (relativeFile[0] == '/') relativeFile = relativeFile.substr(1);
        }
        
        tagsFile << symbol.name << "\t" << relativeFile << "\t" << symbol.line << ";\"\t" << kind << "\n";
    }
    
    tagsFile.close();
    
    clearLine();
    std::cout << "✅ Exported " << allSymbols.size() << " symbols to " << outputFile << "\n";
}

bool FileScanner::openInEditor(const std::string& filePath, int line, const std::string& editor) {
    std::string editorCmd = editor.empty() ? detectEditor() : editor;
    
    if (editorCmd.empty()) {
        std::cout << "❌ No editor found. Please specify an editor or set EDITOR environment variable.\n";
        std::cout << "📄 File: " << filePath << " at line " << line << "\n";
        return false;
    }
    
    // Build command based on editor
    std::string command;
    
    if (editorCmd.find("vim") != std::string::npos || editorCmd.find("nvim") != std::string::npos) {
        command = editorCmd + " +" + std::to_string(line) + " \"" + filePath + "\"";
    } else if (editorCmd.find("code") != std::string::npos) {
        // VS Code
        command = editorCmd + " --goto \"" + filePath + ":" + std::to_string(line) + "\"";
    } else if (editorCmd.find("emacs") != std::string::npos) {
        command = editorCmd + " +" + std::to_string(line) + " \"" + filePath + "\"";
    } else if (editorCmd.find("nano") != std::string::npos) {
        command = editorCmd + " +" + std::to_string(line) + " \"" + filePath + "\"";
    } else if (editorCmd.find("subl") != std::string::npos) {
        // Sublime Text
        command = editorCmd + " \"" + filePath + ":" + std::to_string(line) + "\"";
    } else {
        // Generic editor - try common format
        command = editorCmd + " \"" + filePath + "\"";
    }
    
    std::cout << "🚀 Opening with: " << command << "\n";
    
    // Execute the command
    int result = std::system(command.c_str());
    return result == 0;
}

std::string FileScanner::detectEditor() {
    // Check environment variables
    const char* editor = std::getenv("EDITOR");
    if (editor && strlen(editor) > 0) {
        return std::string(editor);
    }
    
    const char* visual = std::getenv("VISUAL");
    if (visual && strlen(visual) > 0) {
        return std::string(visual);
    }
    
    // Try common editors
    std::vector<std::string> editors = {
        "code",     // VS Code
        "vim",      // Vim
        "nvim",     // Neovim
        "emacs",    // Emacs
        "nano",     // Nano
        "subl",     // Sublime Text
        "gedit",    // GEdit
        "atom"      // Atom
    };
    
    for (const auto& ed : editors) {
        std::string checkCmd = "which " + ed + " >/dev/null 2>&1";
        if (std::system(checkCmd.c_str()) == 0) {
            return ed;
        }
    }
    
    return "";
}

std::string FileScanner::formatSymbolLocation(const Symbol& symbol) {
    SymbolIndex tempIndex;
    std::string typeStr = tempIndex.symbolTypeToString(symbol.type);
    
    // Extract just the filename from the full path
    std::string filename = symbol.file;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = filename.substr(lastSlash + 1);
    }
    
    return symbol.name + "(" + typeStr + ") in " + filename + ":" + std::to_string(symbol.line);
}

// Loading and animation functions
void FileScanner::showLoadingSpinner(const std::string& message, std::chrono::milliseconds duration) {
    std::atomic<bool> running(true);
    std::thread spinnerThread(spinnerAnimation, message, std::ref(running));
    
    std::this_thread::sleep_for(duration);
    running = false;
    
    if (spinnerThread.joinable()) {
        spinnerThread.join();
    }
    
    clearLine();
}

void FileScanner::spinnerAnimation(const std::string& message, std::atomic<bool>& running) {
    int frame = 0;
    while (running) {
        std::cout << "\r" << getSpinnerFrame(frame) << " " << message << std::flush;
        frame = (frame + 1) % 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::string FileScanner::getSpinnerFrame(int frame) {
    const std::vector<std::string> frames = {
        "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
    };
    return frames[frame % frames.size()];
}

void FileScanner::showProgress(const std::string& operation, int current, int total) {
    const int barWidth = 30;
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r" << operation << " [";
    for (int i = 0; i < barWidth; i++) {
        if (i < pos) std::cout << "█";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(progress * 100) << "% (" 
              << current << "/" << total << ")" << std::flush;
    
    if (current == total) {
        std::cout << "\n";
    }
}

void FileScanner::clearLine() {
    std::cout << "\r\033[K";
}

void FileScanner::printWithSpinner(const std::string& message) {
    static int frame = 0;
    std::cout << "\r" << getSpinnerFrame(frame) << " " << message << std::flush;
    frame = (frame + 1) % 10;
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}