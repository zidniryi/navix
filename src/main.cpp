#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "FileScanner.hpp"
#include "Symbol.hpp"

void printUsage(const char* programName) {
    std::cout << "\n";
    std::cout << "┌────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                                                                            │\n";
    std::cout << "│                         🚀 NAVIX - Navigate & Index                        │\n";
    std::cout << "│                    Lightning-fast for your navigation                      │\n";
    std::cout << "│                                                                            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    std::cout << "🎯 Find. Navigate. Code.\n\n";
    
    std::cout << "┌─ USAGE ────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <project_root>") << "  Scan all supported files    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --cpp") << "  Scan C++ files only         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --ts") << "  Scan TypeScript/JS files    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --search <symbol>") << "  Smart symbol search         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --goto <symbol>") << "  Navigate to symbol          │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --export-tags") << "  Export ctags file           │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ EXAMPLES ─────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " .") << "     Scan all files      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --cpp") << "          C++ files only     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --ts") << "           TypeScript/JS      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --search App") << "       Find App symbols     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --goto main") << "        Jump to function    │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --export-tags") << "    Generate tags       │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ FEATURES ─────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ ⚡ Multi-language support (C++, TS, JS)  🎯 Intelligent fuzzy search      │\n";
    std::cout << "│ 🚀 Direct editor navigation             📋 Universal ctags export        │\n";
    std::cout << "│ 🛠️  Auto-detect editors (vim, vscode)   🎨 Beautiful, readable output    │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ SUPPORTED FILES ──────────────────────────────────────────────────────────┐\n";
    std::cout << "│ C++: .cpp • .hpp • .h • .cc • .cxx                                         │\n";
    std::cout << "│ TypeScript/JavaScript: .ts • .tsx • .js • .jsx • .mjs • .cjs              │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ EDITORS ──────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Supported: vim • nvim • code • emacs • nano • subl • atom                │\n";
    std::cout << "│ Setup: export EDITOR=vim  or  export EDITOR=code                          │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "Made with ❤️  for developers who value speed and simplicity.\n\n";
}

void printSymbolResults(const std::vector<Symbol>& symbols, const SymbolIndex& index, bool useNewFormat = false) {
    if (symbols.empty()) {
        std::cout << "❌ No symbols found.\n\n";
        return;
    }
    
    if (useNewFormat) {
        std::cout << "\n┌─ RESULTS ──────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Found " << symbols.size() << " symbol(s):\n";
        for (const auto& symbol : symbols) {
            std::cout << "│ 📍 " << FileScanner::formatSymbolLocation(symbol) << "\n";
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    } else {
        std::cout << "\n┌─ RESULTS ──────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Found " << symbols.size() << " symbol(s):\n";
        std::cout << "│ " << std::left << std::setw(20) << "SYMBOL" 
                  << std::setw(12) << "TYPE" 
                  << std::setw(8) << "LINE" 
                  << "FILE\n";
        std::cout << "│ " << std::string(70, '-') << "\n";
        
        for (const auto& symbol : symbols) {
            std::cout << "│ " << std::left << std::setw(20) << symbol.name
                      << std::setw(12) << index.symbolTypeToString(symbol.type)
                      << std::setw(8) << symbol.line
                      << symbol.file << "\n";
            
            // Show context if available and not too long
            if (!symbol.context.empty() && symbol.context.length() <= 80) {
                std::cout << "│   " << symbol.context << "\n";
            }
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string rootPath = argv[1];
    std::vector<std::string> files;
    std::string scanType = "all"; // default

    if (argc == 2) {
        // Default: scan for all supported files
        files = FileScanner::scanForAllSupportedFiles(rootPath);
        std::cout << "🔍 Scanning for all supported files in: " << rootPath << "\n";
        std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Found " << files.size() << " matching files:\n";
        for (const auto& file : files) {
            std::cout << "│ 📄 " << file << "\n";
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    } else if (argc >= 3) {
        std::string mode = argv[2];
        
        if (mode == "--cpp") {
            // Scan C++ files only
            files = FileScanner::scanForCppFiles(rootPath);
            std::cout << "🔍 Scanning for C++ files in: " << rootPath << "\n";
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--ts") {
            // Scan TypeScript/JavaScript files only
            files = FileScanner::scanForTypeScriptJavaScript(rootPath);
            std::cout << "🔍 Scanning for TypeScript/JavaScript files in: " << rootPath << "\n";
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--ext" && argc >= 4) {
            // Scan by extensions
            std::vector<std::string> extensions;
            for (int i = 3; i < argc; i++) {
                std::string ext = argv[i];
                if (ext[0] != '.') {
                    ext = "." + ext; // Add dot if missing
                }
                extensions.push_back(ext);
            }
            files = FileScanner::scanByExtensions(rootPath, extensions);
            std::cout << "🔍 Scanning for files with extensions: ";
            for (const auto& ext : extensions) {
                std::cout << ext << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--name" && argc >= 4) {
            // Scan by filenames
            std::vector<std::string> filenames;
            for (int i = 3; i < argc; i++) {
                filenames.push_back(argv[i]);
            }
            files = FileScanner::scanByFilenames(rootPath, filenames);
            std::cout << "🔍 Scanning for files named: ";
            for (const auto& name : filenames) {
                std::cout << name << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--pattern" && argc >= 4) {
            // Scan by pattern
            std::string pattern = argv[3];
            files = FileScanner::scanByPattern(rootPath, pattern);
            std::cout << "🔍 Scanning for files containing pattern '" << pattern << "' in: " << rootPath << "\n";
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--search" && argc >= 4) {
            // Symbol search (fuzzy)
            std::string query = argv[3];
            std::cout << "🔍 Searching for symbols matching '" << query << "' in: " << rootPath << "\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--search-exact" && argc >= 4) {
            // Symbol search (exact)
            std::string query = argv[3];
            std::cout << "🎯 Searching for exact symbol '" << query << "' in: " << rootPath << "\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, false);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--goto" && argc >= 4) {
            // Navigate to symbol
            std::string symbolName = argv[3];
            std::string editor = argc >= 5 ? argv[4] : "";
            
            std::cout << "🚀 Looking for symbol '" << symbolName << "' in: " << rootPath << "\n";
            
            if (!FileScanner::gotoSymbol(rootPath, symbolName, editor)) {
                return 1;
            }
            
        } else if (mode == "--export-tags") {
            // Export ctags
            std::string outputFile = argc >= 4 ? argv[3] : "tags.txt";
            
            std::cout << "📋 Exporting tags from " << rootPath << " to " << outputFile << "\n";
            FileScanner::exportTags(rootPath, outputFile);
            
        } else {
            std::cerr << "❌ Invalid arguments.\n\n";
            printUsage(argv[0]);
            return 1;
        }
    } else {
        std::cerr << "❌ Invalid number of arguments.\n\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
