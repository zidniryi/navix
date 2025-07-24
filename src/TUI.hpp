#ifndef TUI_HPP
#define TUI_HPP

#include <vector>
#include <string>
#include <memory>
#include <ncurses.h>
#include "Symbol.hpp"

class TUI {
public:
    TUI();
    ~TUI();
    
    // Main TUI entry point
    void run(const std::string& rootPath);
    
private:
    // Core TUI functionality
    void initializeScreen();
    void cleanupScreen();
    void drawInterface();
    void drawTitle();
    void drawSearchBox();
    void drawFileList();
    void drawPreview();
    void drawStatusBar();
    void drawHelp();
    
    // Input handling
    void handleInput();
    void handleKeyPress(int key);
    void updateSearch();
    void updatePreview();
    void navigateUp();
    void navigateDown();
    void selectItem();
    void openFile();
    void showSymbols();
    void togglePreview();
    
    // Search and filtering
    void performSearch();
    void filterFiles();
    void filterSymbols();
    std::vector<std::string> fuzzyFilter(const std::vector<std::string>& items, const std::string& query);
    std::vector<Symbol> fuzzyFilterSymbols(const std::vector<Symbol>& symbols, const std::string& query);
    
    // File operations
    void loadFiles(const std::string& rootPath);
    void loadSymbols();
    void previewFile(const std::string& filePath);
    std::vector<std::string> readFileLines(const std::string& filePath, int maxLines = 50);
    
    // UI state
    enum class Mode {
        FILES,
        SYMBOLS
    };
    
    Mode currentMode;
    std::string rootPath;
    std::string searchQuery;
    bool showingPreview;
    bool running;
    
    // Data
    std::vector<std::string> allFiles;
    std::vector<std::string> filteredFiles;
    std::vector<Symbol> allSymbols;
    std::vector<Symbol> filteredSymbols;
    std::vector<std::string> previewLines;
    
    // UI state
    int selectedIndex;
    int scrollOffset;
    int searchCursorPos;
    
    // Screen dimensions
    int screenHeight;
    int screenWidth;
    int listHeight;
    int previewWidth;
    
    // Windows
    WINDOW* titleWin;
    WINDOW* searchWin;
    WINDOW* listWin;
    WINDOW* previewWin;
    WINDOW* statusWin;
    WINDOW* helpWin;
    
    // Colors
    void initializeColors();
    void setupColorPairs();
    
    // Helper functions
    std::string truncateString(const std::string& str, int maxWidth);
    std::string highlightMatch(const std::string& str, const std::string& query);
    int calculateScore(const std::string& str, const std::string& query);
    void centerText(WINDOW* win, int y, const std::string& text);
    void drawBox(WINDOW* win, const std::string& title);
};

#endif // TUI_HPP 