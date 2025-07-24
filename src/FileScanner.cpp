#include "FileScanner.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

std::vector<std::string> FileScanner::scanForCppFiles(const std::string& rootPath) {
    std::vector<std::string> defaultExtensions = {".cpp", ".hpp", ".h", ".cc", ".cxx"};
    return scanByExtensions(rootPath, defaultExtensions);
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

SymbolIndex FileScanner::buildSymbolIndex(const std::vector<std::string>& files) {
    SymbolIndex index;
    index.buildIndex(files);
    return index;
}

std::vector<Symbol> FileScanner::searchSymbols(const std::string& rootPath, const std::string& query, bool fuzzy) {
    // Get all C++ files in the directory
    std::vector<std::string> cppFiles = scanForCppFiles(rootPath);
    
    // Build symbol index
    SymbolIndex index = buildSymbolIndex(cppFiles);
    
    // Search for symbols
    return index.search(query, fuzzy);
}