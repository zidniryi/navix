#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "FileScanner.hpp"
#include "Symbol.hpp"

void printUsage(const char* programName) {
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " <project_root_path>                     # Scan for C++ files\n";
    std::cout << "  " << programName << " <project_root_path> --ext <ext1> <ext2> # Scan by extensions\n";
    std::cout << "  " << programName << " <project_root_path> --name <name1> <name2> # Scan by filenames\n";
    std::cout << "  " << programName << " <project_root_path> --pattern <pattern> # Scan by pattern\n";
    std::cout << "  " << programName << " <project_root_path> --search <symbol>   # Search for symbols\n";
    std::cout << "  " << programName << " <project_root_path> --search-exact <symbol> # Exact symbol search\n";
    std::cout << "  " << programName << " <project_root_path> --goto <symbol>     # Find and open symbol in editor\n";
    std::cout << "  " << programName << " <project_root_path> --export-tags [file] # Export ctags file\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " /path/to/project\n";
    std::cout << "  " << programName << " /path/to/project --ext .cpp .hpp .h\n";
    std::cout << "  " << programName << " /path/to/project --name main.cpp config.json\n";
    std::cout << "  " << programName << " /path/to/project --pattern test\n";
    std::cout << "  " << programName << " /path/to/project --search FileScanner\n";
    std::cout << "  " << programName << " /path/to/project --search-exact main\n";
    std::cout << "  " << programName << " /path/to/project --goto main\n";
    std::cout << "  " << programName << " /path/to/project --export-tags tags.txt\n";
    std::cout << "\nEditor Integration:\n";
    std::cout << "  Set EDITOR environment variable or use: vim, code, emacs, nano, subl\n";
    std::cout << "  Examples: export EDITOR=vim  or  export EDITOR=code\n";
}

void printSymbolResults(const std::vector<Symbol>& symbols, const SymbolIndex& index, bool useNewFormat = false) {
    if (symbols.empty()) {
        std::cout << "No symbols found.\n";
        return;
    }
    
    if (useNewFormat) {
        std::cout << "\nFound " << symbols.size() << " symbol(s):\n";
        for (const auto& symbol : symbols) {
            std::cout << " - " << FileScanner::formatSymbolLocation(symbol) << "\n";
        }
    } else {
        std::cout << "\nFound " << symbols.size() << " symbol(s):\n";
        std::cout << std::left << std::setw(20) << "SYMBOL" 
                  << std::setw(12) << "TYPE" 
                  << std::setw(8) << "LINE" 
                  << "FILE" << "\n";
        std::cout << std::string(80, '-') << "\n";
        
        for (const auto& symbol : symbols) {
            std::cout << std::left << std::setw(20) << symbol.name
                      << std::setw(12) << index.symbolTypeToString(symbol.type)
                      << std::setw(8) << symbol.line
                      << symbol.file << "\n";
            
            // Show context if available and not too long
            if (!symbol.context.empty() && symbol.context.length() <= 100) {
                std::cout << "    " << symbol.context << "\n";
            }
            std::cout << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string rootPath = argv[1];
    std::vector<std::string> files;
    std::string scanType = "cpp"; // default

    if (argc == 2) {
        // Default: scan for C++ files
        files = FileScanner::scanForCppFiles(rootPath);
        std::cout << "Scanning for C++ files in: " << rootPath << "\n";
        std::cout << "\nFound " << files.size() << " matching files:\n";
        for (const auto& file : files) {
            std::cout << " - " << file << '\n';
        }
    } else if (argc >= 3) {
        std::string mode = argv[2];
        
        if (mode == "--ext" && argc >= 4) {
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
            std::cout << "Scanning for files with extensions: ";
            for (const auto& ext : extensions) {
                std::cout << ext << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            std::cout << "\nFound " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << " - " << file << '\n';
            }
            
        } else if (mode == "--name" && argc >= 4) {
            // Scan by filenames
            std::vector<std::string> filenames;
            for (int i = 3; i < argc; i++) {
                filenames.push_back(argv[i]);
            }
            files = FileScanner::scanByFilenames(rootPath, filenames);
            std::cout << "Scanning for files named: ";
            for (const auto& name : filenames) {
                std::cout << name << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            std::cout << "\nFound " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << " - " << file << '\n';
            }
            
        } else if (mode == "--pattern" && argc >= 4) {
            // Scan by pattern
            std::string pattern = argv[3];
            files = FileScanner::scanByPattern(rootPath, pattern);
            std::cout << "Scanning for files containing pattern '" << pattern << "' in: " << rootPath << "\n";
            
            std::cout << "\nFound " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << " - " << file << '\n';
            }
            
        } else if (mode == "--search" && argc >= 4) {
            // Symbol search (fuzzy)
            std::string query = argv[3];
            std::cout << "Searching for symbols matching '" << query << "' in: " << rootPath << "\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--search-exact" && argc >= 4) {
            // Symbol search (exact)
            std::string query = argv[3];
            std::cout << "Searching for exact symbol '" << query << "' in: " << rootPath << "\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, false);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--goto" && argc >= 4) {
            // Navigate to symbol
            std::string symbolName = argv[3];
            std::string editor = argc >= 5 ? argv[4] : "";
            
            std::cout << "Looking for symbol '" << symbolName << "' in: " << rootPath << "\n";
            
            if (!FileScanner::gotoSymbol(rootPath, symbolName, editor)) {
                return 1;
            }
            
        } else if (mode == "--export-tags") {
            // Export ctags
            std::string outputFile = argc >= 4 ? argv[3] : "tags.txt";
            
            std::cout << "Exporting tags from " << rootPath << " to " << outputFile << "\n";
            FileScanner::exportTags(rootPath, outputFile);
            
        } else {
            std::cerr << "Invalid arguments.\n";
            printUsage(argv[0]);
            return 1;
        }
    } else {
        std::cerr << "Invalid number of arguments.\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
