#include "TUI.hpp"
#include "FileScanner.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iostream>
#include <set>

#ifdef _WIN32
#include <conio.h>
#include <stdio.h>
#endif

TUI::TUI() : currentMode(Mode::FILES), showingPreview(true), running(false),
             selectedIndex(0), scrollOffset(0), searchCursorPos(0)
#ifdef _WIN32
             , useWindows(true), hConsole(INVALID_HANDLE_VALUE)
#else
             , titleWin(nullptr), searchWin(nullptr), listWin(nullptr),
               previewWin(nullptr), statusWin(nullptr), helpWin(nullptr)
#endif
{
}

TUI::~TUI() {
    cleanupScreen();
}

void TUI::run(const std::string& rootPath) {
    this->rootPath = rootPath;
    initializeScreen();
    loadFiles(rootPath);
    loadSymbols();
    
    running = true;
    while (running) {
        drawInterface();
        handleInput();
    }
    
    cleanupScreen();
}

void TUI::initializeScreen() {
#ifdef _WIN32
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &originalConsoleInfo);
    
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hInput, &originalConsoleMode);
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    
    initializeColorsWindows();
    getScreenSize(screenHeight, screenWidth);
#else
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    initializeColors();
    
    getmaxyx(stdscr, screenHeight, screenWidth);
#endif
    
    // Calculate layout dimensions
    listHeight = screenHeight - 8; // Leave space for title, search, status, help
    previewWidth = showingPreview ? screenWidth / 2 : 0;
    
#ifndef _WIN32
    // Create windows (Unix/Linux only)
    titleWin = newwin(3, screenWidth, 0, 0);
    searchWin = newwin(3, screenWidth, 3, 0);
    listWin = newwin(listHeight, screenWidth - previewWidth, 6, 0);
    previewWin = showingPreview ? newwin(listHeight, previewWidth, 6, screenWidth - previewWidth) : nullptr;
    statusWin = newwin(1, screenWidth, screenHeight - 2, 0);
    helpWin = newwin(1, screenWidth, screenHeight - 1, 0);
    
    // Enable scrolling for list and preview
    scrollok(listWin, TRUE);
    if (previewWin) scrollok(previewWin, TRUE);
#endif
}

void TUI::cleanupScreen() {
#ifdef _WIN32
    if (hConsole != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(hConsole, originalConsoleInfo.wAttributes);
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleMode(hInput, originalConsoleMode);
    }
#else
    if (titleWin) delwin(titleWin);
    if (searchWin) delwin(searchWin);
    if (listWin) delwin(listWin);
    if (previewWin) delwin(previewWin);
    if (statusWin) delwin(statusWin);
    if (helpWin) delwin(helpWin);
    endwin();
#endif
}

#ifdef _WIN32
void TUI::initializeColorsWindows() {
    // Windows console supports basic colors
    // Color pairs will be handled via SetConsoleTextAttribute
}

void TUI::setupColorPairsWindows() {
    // Windows color setup if needed
}

void TUI::clearScreenWindows() {
    system("cls");
}

void TUI::setColorWindows(int colorPair) {
    if (hConsole == INVALID_HANDLE_VALUE) return;
    
    WORD attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Default white
    
    switch (colorPair) {
        case 1: attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Cyan
        case 2: attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Yellow
        case 3: attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Green
        case 4: attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break; // White
        case 5: attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Blue
        case 6: attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Magenta
        case 7: attributes = FOREGROUND_RED | FOREGROUND_INTENSITY; break; // Red
        case 8: attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE; break; // Reverse
    }
    
    SetConsoleTextAttribute(hConsole, attributes);
}

void TUI::resetColorWindows() {
    if (hConsole != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(hConsole, originalConsoleInfo.wAttributes);
    }
}

void TUI::moveCursorWindows(int y, int x) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

void TUI::printTextWindows(const std::string& text) {
    std::cout << text;
}

void TUI::refreshScreenWindows() {
    // Windows console auto-refreshes
}

void TUI::setCursorVisibilityWindows(int visibility) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = (visibility != 0);
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

int TUI::getCharWindows() {
    int ch = _getch();
    
    // Handle special keys
    if (ch == 0 || ch == 224) {
        int second = _getch();
        switch (second) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            default: return second + 256;
        }
    }
    
    // Handle regular keys
    switch (ch) {
        case 13: return KEY_ENTER;
        case 8: return KEY_BACKSPACE;
        default: return ch;
    }
}

void TUI::getScreenSize(int& height, int& width) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
#else
void TUI::initializeColors() {
    if (has_colors()) {
        start_color();
        setupColorPairs();
    }
}

void TUI::setupColorPairs() {
    init_pair(1, COLOR_CYAN, COLOR_BLACK);    // Title
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Search box
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   // Selected item
    init_pair(4, COLOR_WHITE, COLOR_BLACK);   // Normal text
    init_pair(5, COLOR_BLUE, COLOR_BLACK);    // Symbol type
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // File path
    init_pair(7, COLOR_RED, COLOR_BLACK);     // Error/warning
    init_pair(8, COLOR_BLACK, COLOR_WHITE);   // Highlighted match
}
#endif

// Cross-platform wrapper functions
void TUI::clearScreen() {
#ifdef _WIN32
    clearScreenWindows();
#else
    clear();
#endif
}

void TUI::moveCursor(int y, int x) {
#ifdef _WIN32
    moveCursorWindows(y, x);
#else
    move(y, x);
#endif
}

void TUI::printText(const std::string& text) {
#ifdef _WIN32
    printTextWindows(text);
#else
    printw("%s", text.c_str());
#endif
}

void TUI::setColor(int colorPair) {
#ifdef _WIN32
    setColorWindows(colorPair);
#else
    attron(COLOR_PAIR(colorPair));
#endif
}

void TUI::resetColor() {
#ifdef _WIN32
    resetColorWindows();
#else
    attroff(A_COLOR);
#endif
}

int TUI::getChar() {
#ifdef _WIN32
    return getCharWindows();
#else
    return getch();
#endif
}

void TUI::refreshScreen() {
#ifdef _WIN32
    refreshScreenWindows();
#else
    refresh();
#endif
}

void TUI::setCursorVisibility(int visibility) {
#ifdef _WIN32
    setCursorVisibilityWindows(visibility);
#else
    curs_set(visibility);
#endif
}

void TUI::drawInterface() {
    clearScreen();
    
#ifdef _WIN32
    // Windows: Simple console-based interface
    moveCursor(0, 0);
    setColor(1);
    printText("=== NAVIX - Navigate & Index ===\n\n");
    resetColor();
    
    setColor(2);
    printText("Search: " + searchQuery + "\n\n");
    resetColor();
    
    // Show current mode
    setColor(4);
    printText(currentMode == Mode::FILES ? "[FILES MODE]\n" : "[SYMBOLS MODE]\n");
    resetColor();
    
    // Show items
    const auto& items = (currentMode == Mode::FILES) ? filteredFiles : std::vector<std::string>();
    const auto& symbols = (currentMode == Mode::SYMBOLS) ? filteredSymbols : std::vector<Symbol>();
    
    int maxItems = std::min(15, screenHeight - 10); // Leave space for UI elements
    int startIdx = scrollOffset;
    int endIdx = std::min(scrollOffset + maxItems, 
                         currentMode == Mode::FILES ? (int)items.size() : (int)symbols.size());
    
    for (int i = startIdx; i < endIdx; i++) {
        if (i == selectedIndex) {
            setColor(3); // Selected item color
            printText("> ");
        } else {
            printText("  ");
        }
        
        if (currentMode == Mode::FILES) {
            std::string displayPath = truncateString(items[i], screenWidth - 10);
            printText(displayPath + "\n");
        } else {
            const Symbol& symbol = symbols[i];
            SymbolIndex tempIndex;
            std::string typeStr = tempIndex.symbolTypeToString(symbol.type);
            std::string line = symbol.name + " [" + typeStr + "] " + 
                              symbol.file.substr(symbol.file.find_last_of("/\\") + 1) + 
                              ":" + std::to_string(symbol.line);
            printText(truncateString(line, screenWidth - 10) + "\n");
        }
        
        if (i == selectedIndex) {
            resetColor();
        }
    }
    
    // Status line
    printText("\n");
    setColor(4);
    std::string status;
    if (currentMode == Mode::FILES) {
        status = "Files: " + std::to_string(filteredFiles.size()) + "/" + std::to_string(allFiles.size());
    } else {
        status = "Symbols: " + std::to_string(filteredSymbols.size()) + "/" + std::to_string(allSymbols.size());
    }
    printText(status + "\n");
    
    // Help line
    printText("Controls: ↑↓=Navigate, Enter=Open, Tab=Switch Mode, /=Search, q=Quit\n");
    resetColor();
    
#else
    // Unix/Linux: ncurses-based interface
    drawTitle();
    drawSearchBox();
    drawFileList();
    if (showingPreview) drawPreview();
    drawStatusBar();
    drawHelp();
    
    refresh();
    if (titleWin) wrefresh(titleWin);
    if (searchWin) wrefresh(searchWin);
    if (listWin) wrefresh(listWin);
    if (previewWin) wrefresh(previewWin);
    if (statusWin) wrefresh(statusWin);
    if (helpWin) wrefresh(helpWin);
#endif
}

void TUI::drawTitle() {
#ifndef _WIN32
    if (!titleWin) return;
    
    werase(titleWin);
    drawBox(titleWin, "");
    
    wattron(titleWin, COLOR_PAIR(1) | A_BOLD);
    centerText(titleWin, 1, "🚀 NAVIX - Navigate & Index");
    wattroff(titleWin, COLOR_PAIR(1) | A_BOLD);
#endif
}

void TUI::drawSearchBox() {
#ifndef _WIN32
    if (!searchWin) return;
    
    werase(searchWin);
    drawBox(searchWin, currentMode == Mode::FILES ? "Search Files" : "Search Symbols");
    
    wattron(searchWin, COLOR_PAIR(2));
    mvwprintw(searchWin, 1, 2, "Search: %s", searchQuery.c_str());
    wattroff(searchWin, COLOR_PAIR(2));
    
    // Show cursor in search box
    if (!searchQuery.empty() || searchCursorPos >= 0) {
        wmove(searchWin, 1, 10 + searchQuery.length());
        setCursorVisibility(1);
    } else {
        setCursorVisibility(0);
    }
#endif
}

void TUI::drawFileList() {
#ifndef _WIN32
    if (!listWin) return;
    
    werase(listWin);
    drawBox(listWin, currentMode == Mode::FILES ? "Files" : "Symbols");
    
    const auto& items = (currentMode == Mode::FILES) ? filteredFiles : 
                       std::vector<std::string>();
    const auto& symbols = (currentMode == Mode::SYMBOLS) ? filteredSymbols :
                         std::vector<Symbol>();
    
    int maxVisibleItems = listHeight - 2;
    int startIdx = scrollOffset;
    int endIdx = std::min(scrollOffset + maxVisibleItems, 
                         currentMode == Mode::FILES ? (int)items.size() : (int)symbols.size());
    
    for (int i = startIdx; i < endIdx; i++) {
        int y = i - scrollOffset + 1;
        bool isSelected = (i == selectedIndex);
        
        if (isSelected) {
            wattron(listWin, COLOR_PAIR(3) | A_REVERSE);
        }
        
        if (currentMode == Mode::FILES) {
            std::string displayPath = truncateString(items[i], screenWidth - previewWidth - 6);
            mvwprintw(listWin, y, 2, "%s", displayPath.c_str());
        } else {
            const Symbol& symbol = symbols[i];
            SymbolIndex tempIndex;
            std::string typeStr = tempIndex.symbolTypeToString(symbol.type);
            
            // Show symbol with type and file
            wattron(listWin, COLOR_PAIR(4));
            mvwprintw(listWin, y, 2, "%-20s", symbol.name.c_str());
            wattron(listWin, COLOR_PAIR(5));
            mvwprintw(listWin, y, 23, "%-12s", typeStr.c_str());
            wattron(listWin, COLOR_PAIR(6));
            std::string fileName = symbol.file.substr(symbol.file.find_last_of("/\\") + 1);
            mvwprintw(listWin, y, 36, "%s:%d", fileName.c_str(), symbol.line);
            wattroff(listWin, COLOR_PAIR(6));
        }
        
        if (isSelected) {
            wattroff(listWin, COLOR_PAIR(3) | A_REVERSE);
        }
    }
    
    // Show scroll indicators
    if (currentMode == Mode::FILES) {
        if (scrollOffset > 0) {
            mvwprintw(listWin, 1, screenWidth - previewWidth - 4, "↑");
        }
        if (endIdx < (int)items.size()) {
            mvwprintw(listWin, listHeight - 2, screenWidth - previewWidth - 4, "↓");
        }
    } else {
        if (scrollOffset > 0) {
            mvwprintw(listWin, 1, screenWidth - previewWidth - 4, "↑");
        }
        if (endIdx < (int)symbols.size()) {
            mvwprintw(listWin, listHeight - 2, screenWidth - previewWidth - 4, "↓");
        }
    }
#endif
}

void TUI::drawPreview() {
#ifndef _WIN32
    if (!previewWin || !showingPreview) return;
    
    werase(previewWin);
    drawBox(previewWin, "Preview");
    
    if (previewLines.empty()) {
        wattron(previewWin, COLOR_PAIR(4));
        centerText(previewWin, listHeight / 2, "No preview available");
        wattroff(previewWin, COLOR_PAIR(4));
        return;
    }
    
    int maxLines = listHeight - 2;
    for (int i = 0; i < std::min(maxLines, (int)previewLines.size()); i++) {
        std::string line = truncateString(previewLines[i], previewWidth - 4);
        mvwprintw(previewWin, i + 1, 2, "%s", line.c_str());
    }
#endif
}

void TUI::drawStatusBar() {
#ifndef _WIN32
    if (!statusWin) return;
    
    werase(statusWin);
    wattron(statusWin, COLOR_PAIR(4) | A_REVERSE);
    
    std::string status;
    if (currentMode == Mode::FILES) {
        status = "Files: " + std::to_string(filteredFiles.size()) + "/" + std::to_string(allFiles.size());
    } else {
        status = "Symbols: " + std::to_string(filteredSymbols.size()) + "/" + std::to_string(allSymbols.size());
    }
    
    if (!searchQuery.empty()) {
        status += " | Search: " + searchQuery;
    }
    
    mvwprintw(statusWin, 0, 2, "%s", status.c_str());
    
    // Right-align mode indicator
    std::string mode = currentMode == Mode::FILES ? "[FILES]" : "[SYMBOLS]";
    mvwprintw(statusWin, 0, screenWidth - mode.length() - 2, "%s", mode.c_str());
    
    wattroff(statusWin, COLOR_PAIR(4) | A_REVERSE);
#endif
}

void TUI::drawHelp() {
#ifndef _WIN32
    if (!helpWin) return;
    
    werase(helpWin);
    wattron(helpWin, COLOR_PAIR(4));
    
    std::string help = "↑↓:Navigate | Enter:Open | Tab:Files/Symbols | /:Search | p:Preview | q:Quit";
    mvwprintw(helpWin, 0, 2, "%s", help.c_str());
    
    wattroff(helpWin, COLOR_PAIR(4));
#endif
}

void TUI::handleInput() {
    int key = getChar();
    handleKeyPress(key);
}

void TUI::handleKeyPress(int key) {
    switch (key) {
        case 'q':
        case 'Q':
        case 27: // ESC
            running = false;
            break;
            
        case KEY_UP:
            navigateUp();
            break;
            
        case KEY_DOWN:
            navigateDown();
            break;
            
        case '\n':
        case '\r':
        case KEY_ENTER:
            selectItem();
            break;
            
        case '\t':
            // Toggle between files and symbols
            currentMode = (currentMode == Mode::FILES) ? Mode::SYMBOLS : Mode::FILES;
            selectedIndex = 0;
            scrollOffset = 0;
            performSearch();
            break;
            
        case '/':
            // Focus search box
            searchQuery.clear();
            searchCursorPos = 0;
            performSearch();
            break;
            
        case 'p':
        case 'P':
            togglePreview();
            break;
            
        case KEY_BACKSPACE:
        case 127:
        case 8:
            if (!searchQuery.empty()) {
                searchQuery.pop_back();
                performSearch();
            }
            break;
            
        default:
            if (isprint(key)) {
                searchQuery += (char)key;
                performSearch();
            }
            break;
    }
}

void TUI::navigateUp() {
    if (selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < scrollOffset) {
            scrollOffset--;
        }
    }
    updatePreview();
}

void TUI::navigateDown() {
    int maxIndex = (currentMode == Mode::FILES) ? filteredFiles.size() - 1 : filteredSymbols.size() - 1;
    if (selectedIndex < maxIndex) {
        selectedIndex++;
        int maxVisibleItems = listHeight - 2;
        if (selectedIndex >= scrollOffset + maxVisibleItems) {
            scrollOffset++;
        }
    }
    updatePreview();
}

void TUI::selectItem() {
    if (currentMode == Mode::FILES) {
        if (selectedIndex < (int)filteredFiles.size()) {
            openFile();
        }
    } else {
        if (selectedIndex < (int)filteredSymbols.size()) {
            showSymbols();
        }
    }
}

void TUI::openFile() {
    if (selectedIndex >= (int)filteredFiles.size()) return;
    
    std::string filePath = filteredFiles[selectedIndex];
    std::string editor = FileScanner::detectEditor();
    
    // Temporarily cleanup screen for editor
    cleanupScreen();
    
    // Open file in editor
    if (!editor.empty()) {
        FileScanner::openInEditor(filePath, 1, editor);
    } else {
        std::cout << "No editor found. File: " << filePath << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
    
    // Reinitialize screen
    initializeScreen();
}

void TUI::showSymbols() {
    if (selectedIndex >= (int)filteredSymbols.size()) return;
    
    const Symbol& symbol = filteredSymbols[selectedIndex];
    
    // Temporarily cleanup screen for editor
    cleanupScreen();
    
    // Open file at symbol location
    std::string editor = FileScanner::detectEditor();
    if (!editor.empty()) {
        FileScanner::openInEditor(symbol.file, symbol.line, editor);
    } else {
        std::cout << "Found: " << FileScanner::formatSymbolLocation(symbol) << std::endl;
        std::cout << "No editor found. Press Enter to continue...";
        std::cin.get();
    }
    
    // Reinitialize screen
    initializeScreen();
}

void TUI::togglePreview() {
    showingPreview = !showingPreview;
    
    // Recreate windows with new layout
    cleanupScreen();
    initializeScreen();
    updatePreview();
}

void TUI::performSearch() {
    if (currentMode == Mode::FILES) {
        filterFiles();
    } else {
        filterSymbols();
    }
    
    selectedIndex = 0;
    scrollOffset = 0;
    updatePreview();
}

void TUI::filterFiles() {
    if (searchQuery.empty()) {
        filteredFiles = allFiles;
    } else {
        filteredFiles = fuzzyFilter(allFiles, searchQuery);
    }
}

void TUI::filterSymbols() {
    if (searchQuery.empty()) {
        filteredSymbols = allSymbols;
    } else {
        filteredSymbols = fuzzyFilterSymbols(allSymbols, searchQuery);
    }
}

void TUI::updateSearch() {
    performSearch();
}

std::vector<std::string> TUI::fuzzyFilter(const std::vector<std::string>& items, const std::string& query) {
    std::vector<std::pair<std::string, int>> scored;
    
    for (const auto& item : items) {
        int score = calculateScore(item, query);
        if (score > 0) {
            scored.push_back({item, score});
        }
    }
    
    std::sort(scored.begin(), scored.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    for (const auto& item : scored) {
        result.push_back(item.first);
    }
    
    return result;
}

std::vector<Symbol> TUI::fuzzyFilterSymbols(const std::vector<Symbol>& symbols, const std::string& query) {
    std::vector<std::pair<Symbol, int>> scored;
    
    for (const auto& symbol : symbols) {
        int score = calculateScore(symbol.name, query);
        if (score > 0) {
            scored.push_back({symbol, score});
        }
    }
    
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<Symbol> result;
    for (const auto& item : scored) {
        result.push_back(item.first);
    }
    
    return result;
}

int TUI::calculateScore(const std::string& str, const std::string& query) {
    if (query.empty()) return 1;
    
    std::string lowerStr = str;
    std::string lowerQuery = query;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // Exact match gets highest score
    if (lowerStr == lowerQuery) return 1000;
    
    // Prefix match gets high score
    if (lowerStr.find(lowerQuery) == 0) return 500;
    
    // Substring match gets medium score
    size_t pos = lowerStr.find(lowerQuery);
    if (pos != std::string::npos) return 300 - pos;
    
    // Character sequence match gets lower score
    size_t queryIdx = 0;
    for (size_t i = 0; i < lowerStr.length() && queryIdx < lowerQuery.length(); i++) {
        if (lowerStr[i] == lowerQuery[queryIdx]) {
            queryIdx++;
        }
    }
    
    if (queryIdx == lowerQuery.length()) {
        return 100 - (str.length() - query.length());
    }
    
    return 0;
}

void TUI::loadFiles(const std::string& rootPath) {
    allFiles = FileScanner::scanForAllSupportedFiles(rootPath);
    filteredFiles = allFiles;
}

void TUI::loadSymbols() {
    SymbolIndex index = FileScanner::buildSymbolIndex(allFiles);
    
    // Extract symbols from index (this is a workaround since SymbolIndex doesn't expose symbols directly)
    std::vector<std::string> commonPatterns = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
    };
    
    std::set<std::string> addedSymbols;
    for (const auto& pattern : commonPatterns) {
        auto symbols = index.search(pattern, true);
        for (const auto& symbol : symbols) {
            std::string key = symbol.name + symbol.file + std::to_string(symbol.line);
            if (addedSymbols.find(key) == addedSymbols.end()) {
                allSymbols.push_back(symbol);
                addedSymbols.insert(key);
            }
        }
    }
    
    filteredSymbols = allSymbols;
}

void TUI::updatePreview() {
    if (!showingPreview) return;
    
    previewLines.clear();
    
    if (currentMode == Mode::FILES && selectedIndex < (int)filteredFiles.size()) {
        previewFile(filteredFiles[selectedIndex]);
    } else if (currentMode == Mode::SYMBOLS && selectedIndex < (int)filteredSymbols.size()) {
        const Symbol& symbol = filteredSymbols[selectedIndex];
        previewFile(symbol.file);
    }
}

void TUI::previewFile(const std::string& filePath) {
    previewLines = readFileLines(filePath);
}

std::vector<std::string> TUI::readFileLines(const std::string& filePath, int maxLines) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        lines.push_back("Error: Cannot open file");
        return lines;
    }
    
    std::string line;
    int count = 0;
    while (std::getline(file, line) && count < maxLines) {
        lines.push_back(line);
        count++;
    }
    
    if (count >= maxLines) {
        lines.push_back("... (truncated)");
    }
    
    return lines;
}

std::string TUI::truncateString(const std::string& str, int maxWidth) {
    if ((int)str.length() <= maxWidth) return str;
    return str.substr(0, maxWidth - 3) + "...";
}

void TUI::centerText(WINDOW* win, int y, const std::string& text) {
#ifndef _WIN32
    int winWidth = getmaxx(win);
    int x = (winWidth - text.length()) / 2;
    mvwprintw(win, y, x, "%s", text.c_str());
#endif
}

void TUI::drawBox(WINDOW* win, const std::string& title) {
#ifndef _WIN32
    box(win, 0, 0);
    if (!title.empty()) {
        int winWidth = getmaxx(win);
        std::string displayTitle = " " + title + " ";
        int x = (winWidth - displayTitle.length()) / 2;
        mvwprintw(win, 0, x, "%s", displayTitle.c_str());
    }
#endif
} 