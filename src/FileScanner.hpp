#ifndef FILESCANNER_HPP
#define FILESCANNER_HPP

#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include "Symbol.hpp"

class FileScanner {
public:
    static std::vector<std::string> scanForCppFiles(const std::string& rootPath);
    static std::vector<std::string> scanForAllSupportedFiles(const std::string& rootPath);
    static std::vector<std::string> scanForTypeScriptJavaScript(const std::string& rootPath);
    static std::vector<std::string> scanForPython(const std::string& rootPath);
    static std::vector<std::string> scanForGo(const std::string& rootPath);
    static std::vector<std::string> scanByExtensions(const std::string& rootPath, const std::vector<std::string>& extensions);
    static std::vector<std::string> scanByFilenames(const std::string& rootPath, const std::vector<std::string>& filenames);
    static std::vector<std::string> scanByPattern(const std::string& rootPath, const std::string& pattern);
    
    // Symbol search functionality with loading indicators
    static SymbolIndex buildSymbolIndex(const std::vector<std::string>& files, bool showProgressFlag = false);
    static std::vector<Symbol> searchSymbols(const std::string& rootPath, const std::string& query, bool fuzzy = true, bool showProgressFlag = true);
    
    // Navigation & Cross-Reference functionality
    static bool gotoSymbol(const std::string& rootPath, const std::string& symbolName, const std::string& editor = "");
    static void exportTags(const std::string& rootPath, const std::string& outputFile = "tags.txt");
    static bool openInEditor(const std::string& filePath, int line, const std::string& editor = "");
    static std::string detectEditor();
    static std::string formatSymbolLocation(const Symbol& symbol);
    
    // Loading and progress indicators
    static void showLoadingSpinner(const std::string& message, std::chrono::milliseconds duration);
    static void showProgress(const std::string& operation, int current, int total);
    static void clearLine();
    static void printWithSpinner(const std::string& message);
    
private:
    static void spinnerAnimation(const std::string& message, std::atomic<bool>& running);
    static std::string getSpinnerFrame(int frame);
};

#endif // FILESCANNER_HPP
