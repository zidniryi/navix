#ifndef FILESCANNER_HPP
#define FILESCANNER_HPP

#include <vector>
#include <string>
#include "Symbol.hpp"

class FileScanner {
public:
    static std::vector<std::string> scanForCppFiles(const std::string& rootPath);
    static std::vector<std::string> scanForAllSupportedFiles(const std::string& rootPath);
    static std::vector<std::string> scanForTypeScriptJavaScript(const std::string& rootPath);
    static std::vector<std::string> scanForPython(const std::string& rootPath);
    static std::vector<std::string> scanByExtensions(const std::string& rootPath, const std::vector<std::string>& extensions);
    static std::vector<std::string> scanByFilenames(const std::string& rootPath, const std::vector<std::string>& filenames);
    static std::vector<std::string> scanByPattern(const std::string& rootPath, const std::string& pattern);
    
    // Symbol search functionality
    static SymbolIndex buildSymbolIndex(const std::vector<std::string>& files);
    static std::vector<Symbol> searchSymbols(const std::string& rootPath, const std::string& query, bool fuzzy = true);
    
    // Navigation & Cross-Reference functionality
    static bool gotoSymbol(const std::string& rootPath, const std::string& symbolName, const std::string& editor = "");
    static void exportTags(const std::string& rootPath, const std::string& outputFile = "tags.txt");
    static bool openInEditor(const std::string& filePath, int line, const std::string& editor = "");
    static std::string detectEditor();
    static std::string formatSymbolLocation(const Symbol& symbol);
};

#endif // FILESCANNER_HPP
