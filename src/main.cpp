#include <iostream>
#include <vector>
#include <string>
#include "FileScanner.hpp"

void printUsage(const char* programName) {
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " <project_root_path>                     # Scan for C++ files\n";
    std::cout << "  " << programName << " <project_root_path> --ext <ext1> <ext2> # Scan by extensions\n";
    std::cout << "  " << programName << " <project_root_path> --name <name1> <name2> # Scan by filenames\n";
    std::cout << "  " << programName << " <project_root_path> --pattern <pattern> # Scan by pattern\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " /path/to/project\n";
    std::cout << "  " << programName << " /path/to/project --ext .cpp .hpp .h\n";
    std::cout << "  " << programName << " /path/to/project --name main.cpp config.json\n";
    std::cout << "  " << programName << " /path/to/project --pattern test\n";
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
    } else if (argc >= 4) {
        std::string mode = argv[2];
        
        if (mode == "--ext") {
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
            
        } else if (mode == "--name") {
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
            
        } else if (mode == "--pattern" && argc >= 4) {
            // Scan by pattern
            std::string pattern = argv[3];
            files = FileScanner::scanByPattern(rootPath, pattern);
            std::cout << "Scanning for files containing pattern '" << pattern << "' in: " << rootPath << "\n";
            
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

    std::cout << "\nFound " << files.size() << " matching files:\n";
    for (const auto& file : files) {
        std::cout << " - " << file << '\n';
    }

    return 0;
}
