#ifndef TUI_HPP
#define TUI_HPP

#include <vector>
#include <string>
#include <memory>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #include <io.h>
    #include <fcntl.h>
    
    // Define ncurses-like types for Windows
    typedef HANDLE WINDOW;
    #define COLOR_BLACK 0
    #define COLOR_RED 1
    #define COLOR_GREEN 2
    #define COLOR_YELLOW 3
    #define COLOR_BLUE 4
    #define COLOR_MAGENTA 5
    #define COLOR_CYAN 6
    #define COLOR_WHITE 7
    
    #define A_BOLD 0x1
    #define A_REVERSE 0x2
    
    #define KEY_UP 256
    #define KEY_DOWN 257
    #define KEY_ENTER 258
    #define KEY_BACKSPACE 259
#else
    #include <ncurses.h>
#endif

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
    
    // Platform-specific functions
#ifdef _WIN32
    void initializeColorsWindows();
    void setupColorPairsWindows();
    int getCharWindows();
    void clearScreenWindows();
    void setColorWindows(int colorPair);
    void resetColorWindows();
    void moveCursorWindows(int y, int x);
    void printTextWindows(const std::string& text);
    void refreshScreenWindows();
    void setCursorVisibilityWindows(int visibility);
    
    // Windows console state
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO originalConsoleInfo;
    DWORD originalConsoleMode;
#else
    void initializeColors();
    void setupColorPairs();
#endif
    
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
    
    // Windows (platform-agnostic)
#ifdef _WIN32
    // For Windows, we'll use simplified console approach
    bool useWindows;
#else
    WINDOW* titleWin;
    WINDOW* searchWin;
    WINDOW* listWin;
    WINDOW* previewWin;
    WINDOW* statusWin;
    WINDOW* helpWin;
#endif
    
    // Helper functions
    std::string truncateString(const std::string& str, int maxWidth);
    std::string highlightMatch(const std::string& str, const std::string& query);
    int calculateScore(const std::string& str, const std::string& query);
    void centerText(WINDOW* win, int y, const std::string& text);
    void drawBox(WINDOW* win, const std::string& title);
    
    // Cross-platform helper functions
    void clearScreen();
    void moveCursor(int y, int x);
    void printText(const std::string& text);
    void setColor(int colorPair);
    void resetColor();
    int getChar();
    void refreshScreen();
    void setCursorVisibility(int visibility);
    void getScreenSize(int& height, int& width);
};

#endif // TUI_HPP 