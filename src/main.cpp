#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "FileScanner.hpp"
#include "Symbol.hpp"
#include "TUI.hpp"

void printUsage(const char* programName) {
    std::cout << "\n";
    std::cout << "┌────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                                                                            │\n";
    std::cout << "│                         🚀 NAVIX - Navigate & Index                        │\n";
    std::cout << "│                    Lightning-fast for your navigation                      │\n";
    std::cout << "│                                                                            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    std::cout << "🎯 Find. Navigate. Index.\n\n";
    
    std::cout << "┌─ USAGE ────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <project_root>") << "  Scan all supported files    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --tui") << "  Interactive TUI mode        │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --cpp") << "  Scan C++ files only         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --ts") << "  Scan TypeScript/JS files    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --py") << "  Scan Python files only      │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --go") << "  Scan Go files only          │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --txt") << "  Scan text files only        │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --search <symbol>") << "  Smart symbol search         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --goto <symbol>") << "  Navigate to symbol          │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --export-tags") << "  Export ctags file           │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ EXAMPLES ─────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " .") << "     Scan all files      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --tui") << "         Interactive mode    │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --cpp") << "          C++ files only     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --ts") << "           TypeScript/JS      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --py") << "           Python files      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --go") << "           Go files          │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --txt") << "          Text/Doc files     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --search README") << "      Find documentation   │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --search TODO") << "        Find TODO items     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --export-tags") << "    Generate tags       │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ FEATURES ─────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ ⚡ Multi-language support (C++,TS,JS,PY,Go,TXT) 🎯 Intelligent fuzzy search │\n";
    std::cout << "│ 🚀 Direct editor navigation                   📋 Universal ctags export    │\n";
    std::cout << "│ 🛠️  Auto-detect editors (vim, vscode)         🎨 Beautiful, readable output│\n";
    std::cout << "│ 🖥️  Interactive TUI with arrow keys           ⌨️  Real-time file preview   │\n";
    std::cout << "│ ⚡ Animated loading indicators                 🔍 Smart progress tracking   │\n";
    std::cout << "│ 📄 Text content indexing (headers, URLs, TODOs) 🔗 Email & link extraction │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ SUPPORTED FILES ──────────────────────────────────────────────────────────┐\n";
    std::cout << "│ C++: .cpp • .hpp • .h • .cc • .cxx                                         │\n";
    std::cout << "│ TypeScript/JavaScript: .ts • .tsx • .js • .jsx • .mjs • .cjs              │\n";
    std::cout << "│ Python: .py • .pyw • .pyi                                                  │\n";
    std::cout << "│ Go: .go                                                                     │\n";
    std::cout << "│ Text: .txt • .md • .rst • .log • .readme • .doc                            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ TEXT FILE FEATURES ───────────────────────────────────────────────────────┐\n";
    std::cout << "│ 📑 Headers & Subheaders    🔗 URLs & Email addresses                       │\n";
    std::cout << "│ 📝 TODO/FIXME/NOTE items   📄 Significant lines & content                  │\n";
    std::cout << "│ 🔍 Important word extraction  📋 Documentation indexing                    │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ TUI CONTROLS ─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ ↑↓: Navigate items  │  Enter: Open file/symbol  │  Tab: Switch Files/Symbols │\n";
    std::cout << "│ /: Search           │  p: Toggle preview        │  q/ESC: Quit TUI           │\n";
    std::cout << "│ Backspace: Delete   │  Type: Filter results     │  Auto file preview         │\n";
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
        std::cout << "│ 🎉 Found " << symbols.size() << " symbol(s):\n";
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
        std::cout << "🔍 Scanning for all supported files in: " << rootPath << "\n";
        
        // Show loading spinner while scanning
        FileScanner::printWithSpinner("Discovering files");
        files = FileScanner::scanForAllSupportedFiles(rootPath);
        FileScanner::clearLine();
        
        std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
        for (const auto& file : files) {
            std::cout << "│ 📄 " << file << "\n";
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    } else if (argc >= 3) {
        std::string mode = argv[2];
        
        if (mode == "--tui") {
            // Launch TUI mode
            std::cout << "🚀 Launching Interactive TUI mode...\n";
            FileScanner::showLoadingSpinner("Initializing TUI", std::chrono::milliseconds(800));
            
            try {
                TUI tui;
                tui.run(rootPath);
            } catch (const std::exception& e) {
                std::cerr << "❌ TUI Error: " << e.what() << "\n";
                std::cerr << "Make sure ncurses is installed and terminal supports TUI.\n";
                return 1;
            }
            return 0;
            
        } else if (mode == "--cpp") {
            // Scan C++ files only
            std::cout << "🔍 Scanning for C++ files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering C++ files");
            files = FileScanner::scanForCppFiles(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--ts") {
            // Scan TypeScript/JavaScript files only
            std::cout << "🔍 Scanning for TypeScript/JavaScript files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering TS/JS files");
            files = FileScanner::scanForTypeScriptJavaScript(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--py") {
            // Scan Python files only
            std::cout << "🔍 Scanning for Python files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering Python files");
            files = FileScanner::scanForPython(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--go") {
            // Scan Go files only
            std::cout << "🔍 Scanning for Go files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering Go files");
            files = FileScanner::scanForGo(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--txt") {
            // Scan text files only
            std::cout << "🔍 Scanning for text files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering text files");
            files = FileScanner::scanForPlainText(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
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
            
            std::cout << "🔍 Scanning for files with extensions: ";
            for (const auto& ext : extensions) {
                std::cout << ext << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering files");
            files = FileScanner::scanByExtensions(rootPath, extensions);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
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
            
            std::cout << "🔍 Scanning for files named: ";
            for (const auto& name : filenames) {
                std::cout << name << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering files");
            files = FileScanner::scanByFilenames(rootPath, filenames);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--pattern" && argc >= 4) {
            // Scan by pattern
            std::string pattern = argv[3];
            std::cout << "🔍 Scanning for files containing pattern '" << pattern << "' in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering files");
            files = FileScanner::scanByPattern(rootPath, pattern);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--search" && argc >= 4) {
            // Symbol search (fuzzy) with loading animation
            std::string query = argv[3];
            std::cout << "🔍 Searching for symbols matching '" << query << "' in: " << rootPath << "\n\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, true, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--search-exact" && argc >= 4) {
            // Symbol search (exact) with loading animation
            std::string query = argv[3];
            std::cout << "🎯 Searching for exact symbol '" << query << "' in: " << rootPath << "\n\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, false, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--goto" && argc >= 4) {
            // Navigate to symbol with loading animation
            std::string symbolName = argv[3];
            std::string editor = argc >= 5 ? argv[4] : "";
            
            std::cout << "🚀 Looking for symbol '" << symbolName << "' in: " << rootPath << "\n\n";
            
            if (!FileScanner::gotoSymbol(rootPath, symbolName, editor)) {
                return 1;
            }
            
        } else if (mode == "--export-tags") {
            // Export ctags with loading animation
            std::string outputFile = argc >= 4 ? argv[3] : "tags.txt";
            
            std::cout << "📋 Exporting tags from " << rootPath << " to " << outputFile << "\n\n";
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
