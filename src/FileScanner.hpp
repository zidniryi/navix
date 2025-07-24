#ifndef FILESCANNER_HPP
#define FILESCANNER_HPP

#include <vector>
#include <string>
#include "Symbol.hpp"

class FileScanner {
public:
    static std::vector<std::string> scanForCppFiles(const std::string& rootPath);
    static std::vector<std::string> scanByExtensions(const std::string& rootPath, const std::vector<std::string>& extensions);
    static std::vector<std::string> scanByFilenames(const std::string& rootPath, const std::vector<std::string>& filenames);
    static std::vector<std::string> scanByPattern(const std::string& rootPath, const std::string& pattern);
    
    // Symbol search functionality
    static SymbolIndex buildSymbolIndex(const std::vector<std::string>& files);
    static std::vector<Symbol> searchSymbols(const std::string& rootPath, const std::string& query, bool fuzzy = true);
};

#endif // FILESCANNER_HPP
