#ifndef FILESCANNER_HPP
#define FILESCANNER_HPP

#include <vector>
#include <string>

class FileScanner {
public:
    static std::vector<std::string> scanForCppFiles(const std::string& rootPath);
    static std::vector<std::string> scanByExtensions(const std::string& rootPath, const std::vector<std::string>& extensions);
    static std::vector<std::string> scanByFilenames(const std::string& rootPath, const std::vector<std::string>& filenames);
    static std::vector<std::string> scanByPattern(const std::string& rootPath, const std::string& pattern);
};

#endif // FILESCANNER_HPP
