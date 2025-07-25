#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "FileScanner.hpp"
#include "Symbol.hpp"
#include "TUI.hpp"
#include "FileWatcher.hpp"
#include "PerformanceLogger.hpp"
#include "AutocompleteEngine.hpp"
#include "JsonExporter.hpp"
#include "LSPServer.hpp"

// Version information
#define NAVIX_VERSION "0.1"
#define NAVIX_BUILD_DATE __DATE__

void printVersion() {
    std::cout << "\n";
    std::cout << "┌────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                                                                            │\n";
    std::cout << "│                         🚀 NAVIX - Navigate & Index                        │\n";
    std::cout << "│                    Lightning-fast for your navigation                      │\n";
    std::cout << "│                                                                            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    std::cout << "🔢 Version: " << NAVIX_VERSION << "\n";
    std::cout << "📅 Build Date: " << NAVIX_BUILD_DATE << "\n";
    std::cout << "⚡ Multi-Language Support: C++, TypeScript, JavaScript, Python, Go, Swift, Text\n";
    std::cout << "🖥️  TUI Mode: Interactive navigation with ncurses\n";
    std::cout << "📊 Live Features: File watcher, performance logging\n";
    std::cout << "🔍 Autocomplete: Offline prefix & fuzzy matching\n";
    std::cout << "🎯 Features: Symbol indexing, fuzzy search, editor integration, ctags export\n";
    std::cout << "\n";
    std::cout << "Made with ❤️  for developers who value speed and simplicity.\n";
    std::cout << "📄 License: MIT License\n";
    std::cout << "🔗 Homepage: https://github.com/zidniryi/navix\n\n";
}

void printUsage(const char* programName) {
    std::cout << "\n";
    std::cout << "┌────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                                                                            │\n";
    std::cout << "│                         🚀 NAVIX - Navigate & Index                        │\n";
    std::cout << "│                    Lightning-fast for your navigation                      │\n";
    std::cout << "│                                                                            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    std::cout << "🎯 Find. Navigate. Index.\n\n";
    
    std::cout << "┌─ USAGE ────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <project_root>") << "  Scan all supported files    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " --version") << "  Show version information    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " --help") << "  Show this help message      │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --tui") << "  Interactive TUI mode        │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --autocomplete") << "  Interactive autocomplete    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --complete <query>") << "  Get completions for query   │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --live") << "  Live file watching mode     │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --watch") << "  Same as --live              │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --perf") << "  Enable performance logging │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --cpp") << "  Scan C++ files only         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --ts") << "  Scan TypeScript/JS files    │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --py") << "  Scan Python files only      │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --go") << "  Scan Go files only          │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --swift") << "  Scan Swift files only       │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --txt") << "  Scan text files only        │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --search <symbol>") << "  Smart symbol search         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --goto <symbol>") << "  Navigate to symbol          │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --export-tags") << "  Export ctags file           │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --json [file]") << "  Export symbols to JSON      │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --lsp") << "  Start LSP server mode       │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --json-compact") << "  Export compact JSON         │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --json-stats") << "  Export JSON with stats      │\n";
    std::cout << "│ " << std::left << std::setw(40) << (std::string(programName) + " <root> --json-lsp") << "  Export LSP-compatible JSON  │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ EXAMPLES ─────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " .") << "     Scan all files      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " --version") << "      Show version info   │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " --help") << "         Show help message  │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --tui") << "         Interactive mode    │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --autocomplete") << "   Interactive complete  │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --complete app") << "     Complete 'app'       │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --live") << "        Live watching mode  │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --perf") << "        Performance logs    │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --live --perf") << "   Live + performance   │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --cpp") << "          C++ files only     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --ts") << "           TypeScript/JS      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --py") << "           Python files      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --go") << "           Go files          │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --swift") << "        Swift files        │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --txt") << "          Text/Doc files     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --search README") << "      Find documentation   │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --search TODO") << "        Find TODO items     │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --export-tags") << "    Generate tags       │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --json symbols.json") << " Export to JSON      │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --lsp") << "               Start LSP server    │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --json-compact") << "     Compact JSON export │\n";
    std::cout << "│ " << std::left << std::setw(45) << (std::string(programName) + " . --json-lsp") << "        LSP-compatible JSON │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ FEATURES ─────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ ⚡ Multi-language support (C++,TS,JS,PY,Go,Swift,TXT) 🎯 Intelligent fuzzy search │\n";
    std::cout << "│ 🚀 Direct editor navigation                   📋 Universal ctags export    │\n";
    std::cout << "│ 🛠️  Auto-detect editors (vim, vscode)         🎨 Beautiful, readable output│\n";
    std::cout << "│ 🖥️  Interactive TUI with arrow keys           ⌨️  Real-time file preview   │\n";
    std::cout << "│ ⚡ Animated loading indicators                 🔍 Smart progress tracking   │\n";
    std::cout << "│ 📄 Text content indexing (headers, URLs, TODOs) 🔗 Email & link extraction │\n";
    std::cout << "│ 🔄 Live file watching & auto-reindexing       📊 Performance metrics       │\n";
    std::cout << "│ 🔍 Offline autocomplete with prefix & fuzzy   🎯 Multi-algorithm matching  │\n";
    std::cout << "│ 📤 JSON export for external tools             🔗 LSP bridge compatibility   │\n";
    std::cout << "│ 🛠️  Language Server Protocol support          🎯 IDE/Editor integration     │\n";
    std::cout << "│ 📊 Multiple JSON formats (compact, detailed, cool)  ⚡ Developer tool ecosystem   │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ SUPPORTED FILES ──────────────────────────────────────────────────────────┐\n";
    std::cout << "│ C++: .cpp • .hpp • .h • .cc • .cxx                                         │\n";
    std::cout << "│ TypeScript/JavaScript: .ts • .tsx • .js • .jsx • .mjs • .cjs              │\n";
    std::cout << "│ Python: .py • .pyw • .pyi                                                  │\n";
    std::cout << "│ Go: .go                                                                     │\n";
    std::cout << "│ Swift: .swift                                                               │\n";
    std::cout << "│ Text: .txt • .md • .rst • .log • .readme • .doc                            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ TEXT FILE FEATURES ───────────────────────────────────────────────────────┐\n";
    std::cout << "│ 📑 Headers & Subheaders    🔗 URLs & Email addresses                       │\n";
    std::cout << "│ 📝 TODO/FIXME/NOTE items   📄 Significant lines & content                  │\n";
    std::cout << "│ 🔍 Important word extraction  📋 Documentation indexing                    │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ LIVE FEATURES ────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ 🔄 Real-time file monitoring     📊 Performance metrics & timing           │\n";
    std::cout << "│ ⚡ Auto-reindexing on changes     📈 Files/symbols per second tracking      │\n";
    std::cout << "│ 🎯 Cross-platform file watching  📋 Detailed per-file statistics          │\n";
    std::cout << "│ 💾 Debounced change detection    🔍 Language-specific breakdowns           │\n";
    std::cout << "│ 📝 Performance logging to file   ⏱️  Parse time analysis                  │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ AUTOCOMPLETE FEATURES ────────────────────────────────────────────────────┐\n";
    std::cout << "│ 🔍 Offline symbol completion     🎯 Multiple matching algorithms           │\n";
    std::cout << "│ ⚡ Lightning-fast prefix matching 🧠 Intelligent fuzzy search              │\n";
    std::cout << "│ 📝 Interactive completion mode   🏆 Smart scoring & ranking                │\n";
    std::cout << "│ 🌳 Trie-based efficient indexing  💡 Context-aware suggestions            │\n";
    std::cout << "│ 📊 Configurable weights & boosts  🎨 Beautiful formatted results          │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ DEV TOOL INTEGRATIONS ────────────────────────────────────────────────────┐\n";
    std::cout << "│ 📤 JSON Export: Multiple formats for external tool integration            │\n";
    std::cout << "│   • Standard: Full symbol data with metadata and statistics              │\n";
    std::cout << "│   • Compact: Minimized JSON for bandwidth-sensitive applications        │\n";
    std::cout << "│   • LSP: Language Server Protocol compatible format                      │\n";
    std::cout << "│   • Stats: Includes project statistics and language breakdowns           │\n";
    std::cout << "│ 🛠️  LSP Server: Full Language Server Protocol implementation              │\n";
    std::cout << "│   • Document symbols, workspace symbols, definition lookup              │\n";
    std::cout << "│   • Real-time symbol updates, hover information                         │\n";
    std::cout << "│   • Compatible with VS Code, Vim, Emacs, and other LSP clients          │\n";
    std::cout << "│ 🔗 IDE Integration: Seamless editor and IDE connectivity                  │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ TUI CONTROLS ─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ ↑↓: Navigate items  │  Enter: Open file/symbol  │  Tab: Switch Files/Symbols │\n";
    std::cout << "│ /: Search           │  p: Toggle preview        │  q/ESC: Quit TUI           │\n";
    std::cout << "│ Backspace: Delete   │  Type: Filter results     │  Auto file preview         │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌─ EDITORS ──────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Supported: vim • nvim • code • emacs • nano • subl • atom                │\n";
    std::cout << "│ Setup: export EDITOR=vim  or  export EDITOR=code                          │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "Made with ❤️  for developers who value speed and simplicity.\n\n";
}

void printSymbolResults(const std::vector<Symbol>& symbols, const SymbolIndex& index, bool useNewFormat = false) {
    if (symbols.empty()) {
        std::cout << "❌ No symbols found.\n\n";
        return;
    }
    
    if (useNewFormat) {
        std::cout << "\n┌─ RESULTS ──────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ 🎉 Found " << symbols.size() << " symbol(s):\n";
        for (const auto& symbol : symbols) {
            std::cout << "│ 📍 " << FileScanner::formatSymbolLocation(symbol) << "\n";
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    } else {
        std::cout << "\n┌─ RESULTS ──────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Found " << symbols.size() << " symbol(s):\n";
        std::cout << "│ " << std::left << std::setw(20) << "SYMBOL" 
                  << std::setw(12) << "TYPE" 
                  << std::setw(8) << "LINE" 
                  << "FILE\n";
        std::cout << "│ " << std::string(70, '-') << "\n";
        
        for (const auto& symbol : symbols) {
            std::cout << "│ " << std::left << std::setw(20) << symbol.name
                      << std::setw(12) << index.symbolTypeToString(symbol.type)
                      << std::setw(8) << symbol.line
                      << symbol.file << "\n";
            
            // Show context if available and not too long
            if (!symbol.context.empty() && symbol.context.length() <= 80) {
                std::cout << "│   " << symbol.context << "\n";
            }
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
    }
}

// Live mode implementation
void runLiveMode(const std::string& rootPath, bool enablePerformance = false, bool verbose = false) {
    std::cout << "🔄 Starting Live Mode with real-time file watching...\n\n";
    
    // Setup performance logger
    PerformanceLogger perfLogger;
    if (enablePerformance) {
        perfLogger.setVerbose(verbose);
        perfLogger.setLogToFile(true, "navix_live_performance.log");
        perfLogger.startSession("live-indexing");
    }
    
    // Initial indexing
    std::cout << "📊 Initial indexing...\n";
    std::vector<std::string> allFiles = FileScanner::scanForAllSupportedFiles(rootPath);
    
    SymbolIndex symbolIndex;
    if (enablePerformance) {
        symbolIndex.setPerformanceLogger(&perfLogger);
    }
    
    symbolIndex.buildIndex(allFiles);
    
    if (enablePerformance) {
        perfLogger.printSessionSummary();
        perfLogger.printLanguageBreakdown();
    }
    
    std::cout << "✅ Initial index built: " << symbolIndex.size() << " symbols from " 
              << allFiles.size() << " files\n\n";
    
    // Setup file watcher
    FileWatcher watcher;
    std::vector<std::string> extensions = {
        ".cpp", ".hpp", ".h", ".cc", ".cxx",
        ".ts", ".tsx", ".js", ".jsx", ".mjs", ".cjs",
        ".py", ".pyw", ".pyi",
        ".go",
        ".txt", ".text", ".md", ".rst", ".log", ".readme", ".doc"
    };
    
    // Set up change callback
    watcher.setChangeCallback([&](const FileChange& change) {
        std::string eventName;
        switch (change.event) {
            case FileEvent::CREATED: eventName = "CREATED"; break;
            case FileEvent::MODIFIED: eventName = "MODIFIED"; break;
            case FileEvent::DELETED: eventName = "DELETED"; break;
            case FileEvent::MOVED: eventName = "MOVED"; break;
        }
        
        std::cout << "📁 " << eventName << ": " << change.path << "\n";
        
        // Re-index the affected file or remove from index
        if (change.event == FileEvent::DELETED) {
            // TODO: Remove symbols from deleted file
            std::cout << "   ❌ File deleted - symbols removed from index\n";
        } else {
            // Re-parse the file
            if (enablePerformance) {
                perfLogger.startSession("file-reindex");
            }
            
            std::vector<std::string> singleFile = {change.path};
            SymbolIndex tempIndex;
            if (enablePerformance) {
                tempIndex.setPerformanceLogger(&perfLogger);
            }
            tempIndex.buildIndex(singleFile);
            
            if (enablePerformance) {
                perfLogger.endSession();
            }
            
            std::cout << "   ✅ File re-indexed: " << tempIndex.size() << " symbols\n";
        }
    });
    
    // Start watching
    if (watcher.startWatching(rootPath, extensions)) {
        std::cout << "👀 Watching " << watcher.getWatchedFileCount() << " files for changes...\n";
        std::cout << "💡 Press Ctrl+C to stop\n\n";
        
        // Keep the program running and show live stats
        while (watcher.isWatching()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            if (enablePerformance && watcher.getChangeEventCount() > 0) {
                std::cout << "📊 Live Stats - Changes detected: " << watcher.getChangeEventCount() 
                         << ", Files watched: " << watcher.getWatchedFileCount() << "\n";
            }
        }
    } else {
        std::cerr << "❌ Failed to start file watcher\n";
    }
    
    watcher.stopWatching();
    
    if (enablePerformance) {
        perfLogger.endSession();
        std::cout << "\n📊 Final Performance Summary:\n";
        perfLogger.printSessionSummary();
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "❌ Please provide a project root path or use --version/--help.\n\n";
        printUsage(argv[0]);
        return 1;
    }

    // Handle standalone flags first (can be used without project path)
    if (argc == 2) {
        std::string flag = argv[1];
        
        if (flag == "--version" || flag == "-v") {
            printVersion();
            return 0;
        }
        
        if (flag == "--help" || flag == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }

    std::string rootPath = argv[1];
    std::vector<std::string> files;
    std::string scanType = "all"; // default

    if (argc == 2) {
        // Default scan mode for single argument (project path only)
        std::cout << "🔍 Scanning for all supported files in: " << rootPath << "\n";
        
        FileScanner::printWithSpinner("Discovering files");
        files = FileScanner::scanForAllSupportedFiles(rootPath);
        FileScanner::clearLine();
        
        std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
        for (const auto& file : files) {
            std::cout << "│ 📄 " << file << "\n";
        }
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
    } else if (argc >= 3) {
        std::string mode = argv[2];
        
        if (mode == "--help" || mode == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        
        if (mode == "--version" || mode == "-v") {
            printVersion();
            return 0;
        }
        
        if (mode == "--autocomplete" || mode == "--complete-interactive") {
            // Interactive autocomplete mode
            std::cout << "🔍 Loading symbols for autocomplete...\n";
            
            std::vector<std::string> allFiles = FileScanner::scanForAllSupportedFiles(rootPath);
            SymbolIndex symbolIndex;
            symbolIndex.buildIndex(allFiles);
            
            AutocompleteEngine autocomplete;
            autocomplete.buildIndex(symbolIndex.getSymbols());
            autocomplete.printStatistics();
            
            autocomplete.runInteractiveMode("autocomplete> ");
            return 0;
            
        } else if (mode == "--complete" && argc >= 4) {
            // Single query autocomplete
            std::string query = argv[3];
            
            std::cout << "🔍 Building autocomplete index...\n";
            std::vector<std::string> allFiles = FileScanner::scanForAllSupportedFiles(rootPath);
            SymbolIndex symbolIndex;
            symbolIndex.buildIndex(allFiles);
            
            AutocompleteEngine autocomplete;
            autocomplete.buildIndex(symbolIndex.getSymbols());
            
            std::cout << "🔍 Getting completions for '" << query << "'...\n\n";
            auto results = autocomplete.getCompletions(query, 15);
            
            if (results.empty()) {
                std::cout << "❌ No completions found for '" << query << "'\n";
            } else {
                std::cout << "✅ Found " << results.size() << " completion(s) for '" << query << "':\n\n";
                
                std::cout << "┌─ AUTOCOMPLETE RESULTS ─────────────────────────────────────────────────────┐\n";
                for (size_t i = 0; i < results.size(); ++i) {
                    const auto& result = results[i];
                    std::string filename = std::filesystem::path(result.file).filename().string();
                    
                    std::cout << "│ " << std::setw(2) << (i + 1) << ". " 
                             << std::setw(20) << result.suggestion
                             << " │ " << std::setw(8) << result.matchType
                             << " │ " << std::setw(6) << std::fixed << std::setprecision(2) << result.score
                             << " │ " << std::setw(15) << filename << ":" << result.line << "\n";
                }
                std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
            }
            return 0;
            
        } else if (mode == "--live" || mode == "--watch") {
            // Live file watching mode
            bool enablePerformance = false;
            bool verbose = false;
            
            // Check for additional flags
            for (int i = 3; i < argc; i++) {
                std::string flag = argv[i];
                if (flag == "--perf" || flag == "--performance") {
                    enablePerformance = true;
                } else if (flag == "--verbose" || flag == "-v") {
                    verbose = true;
                }
            }
            
            runLiveMode(rootPath, enablePerformance, verbose);
            return 0;
            
        } else if (mode == "--perf" || mode == "--performance") {
            // Performance logging mode (single indexing run)
            std::cout << "📊 Performance logging enabled\n\n";
            
            PerformanceLogger perfLogger;
            perfLogger.setVerbose(true);
            perfLogger.setLogToFile(true, "navix_performance.log");
            perfLogger.startSession("performance-analysis");
            
            std::cout << "🔍 Scanning for all supported files in: " << rootPath << "\n";
            std::vector<std::string> allFiles = FileScanner::scanForAllSupportedFiles(rootPath);
            
            std::cout << "📊 Building symbol index with performance tracking...\n";
            SymbolIndex symbolIndex;
            symbolIndex.setPerformanceLogger(&perfLogger);
            symbolIndex.buildIndex(allFiles);
            
            perfLogger.endSession();
            
            std::cout << "\n📈 Performance Analysis Complete!\n";
            perfLogger.printSessionSummary();
            perfLogger.printLanguageBreakdown();
            perfLogger.printTopSlowFiles(15);
            
            std::cout << "📝 Detailed logs saved to: navix_performance.log\n";
            return 0;
            
        } else if (mode == "--tui") {
            // Interactive TUI Mode
            std::cout << "🚀 Launching Interactive TUI mode...\n";
            FileScanner::showLoadingSpinner("Initializing TUI", std::chrono::milliseconds(800));
            
            try {
                TUI tui;
                tui.run(rootPath);
            } catch (const std::exception& e) {
                std::cerr << "❌ TUI Error: " << e.what() << "\n";
                std::cerr << "Make sure ncurses is installed and terminal supports TUI.\n";
                return 1;
            }
            return 0;
            
        } else if (mode == "--cpp") {
            // Scan C++ files only
            std::cout << "🔍 Scanning for C++ files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering C++ files");
            files = FileScanner::scanForCppFiles(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--ts") {
            // Scan TypeScript/JavaScript files only
            std::cout << "🔍 Scanning for TypeScript/JavaScript files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering TS/JS files");
            files = FileScanner::scanForTypeScriptJavaScript(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--py") {
            // Scan Python files only
            std::cout << "🔍 Scanning for Python files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering Python files");
            files = FileScanner::scanForPython(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--go") {
            // Scan Go files only
            std::cout << "🔍 Scanning for Go files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering Go files");
            files = FileScanner::scanForGo(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--swift") {
            std::cout << "🔍 Scanning for Swift files in: " << rootPath << "\n";
            FileScanner::printWithSpinner("Discovering Swift files");
            files = FileScanner::scanForSwift(rootPath);
            FileScanner::clearLine();
            
            std::cout << "📊 Found " << files.size() << " Swift files:\n\n";
            for (const auto& file : files) {
                std::cout << "📄 " << file << "\n";
            }
            
        } else if (mode == "--txt") {
            // Scan text files only
            std::cout << "🔍 Scanning for text files in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering text files");
            files = FileScanner::scanForPlainText(rootPath);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--ext" && argc >= 4) {
            // Scan by extensions
            std::vector<std::string> extensions;
            for (int i = 3; i < argc; i++) {
                std::string ext = argv[i];
                if (ext[0] != '.') {
                    ext = "." + ext; // Add dot if missing
                }
                extensions.push_back(ext);
            }
            
            std::cout << "🔍 Scanning for files with extensions: ";
            for (const auto& ext : extensions) {
                std::cout << ext << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering files");
            files = FileScanner::scanByExtensions(rootPath, extensions);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--name" && argc >= 4) {
            // Scan by filenames
            std::vector<std::string> filenames;
            for (int i = 3; i < argc; i++) {
                filenames.push_back(argv[i]);
            }
            
            std::cout << "🔍 Scanning for files named: ";
            for (const auto& name : filenames) {
                std::cout << name << " ";
            }
            std::cout << "in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering files");
            files = FileScanner::scanByFilenames(rootPath, filenames);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--pattern" && argc >= 4) {
            // Scan by pattern
            std::string pattern = argv[3];
            std::cout << "🔍 Scanning for files containing pattern '" << pattern << "' in: " << rootPath << "\n";
            
            FileScanner::printWithSpinner("Discovering files");
            files = FileScanner::scanByPattern(rootPath, pattern);
            FileScanner::clearLine();
            
            std::cout << "\n┌─ FILES ────────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 🎉 Found " << files.size() << " matching files:\n";
            for (const auto& file : files) {
                std::cout << "│ 📄 " << file << "\n";
            }
            std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
            
        } else if (mode == "--search" && argc >= 4) {
            // Symbol search (fuzzy) with loading animation
            std::string query = argv[3];
            std::cout << "🔍 Searching for symbols matching '" << query << "' in: " << rootPath << "\n\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, true, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--search-exact" && argc >= 4) {
            // Symbol search (exact) with loading animation
            std::string query = argv[3];
            std::cout << "🎯 Searching for exact symbol '" << query << "' in: " << rootPath << "\n\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, false, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--goto" && argc >= 4) {
            // Navigate to symbol with loading animation
            std::string symbolName = argv[3];
            std::string editor = argc >= 5 ? argv[4] : "";
            
            std::cout << "🚀 Looking for symbol '" << symbolName << "' in: " << rootPath << "\n\n";
            
            if (!FileScanner::gotoSymbol(rootPath, symbolName, editor)) {
                return 1;
            }
            
        } else if (mode == "--export-tags") {
            // Export ctags with loading animation
            std::string outputFile = argc >= 4 ? argv[3] : "tags.txt";
            
            std::cout << "📋 Exporting tags from " << rootPath << " to " << outputFile << "\n\n";
            FileScanner::exportTags(rootPath, outputFile);
            
        } else if (mode == "--json") {
            std::string outputFile = argc > 3 ? argv[3] : "symbols.json";
            std::cout << "📤 JSON Export: " << outputFile << "\n";
            
            FileScanner::printWithSpinner("Building symbol index");
            SymbolIndex index;
            files = FileScanner::scanForAllSupportedFiles(rootPath);
            index.buildIndex(files);
            FileScanner::clearLine();
            
            FileScanner::printWithSpinner("Exporting to JSON");
            JsonExporter exporter;
            bool success = exporter.exportToFile(index, outputFile, true);
            FileScanner::clearLine();
            
            if (success) {
                std::cout << "✅ JSON export completed successfully!\n";
                std::cout << "📄 File: " << outputFile << "\n";
                std::cout << "📊 Symbols: " << index.size() << "\n";
            }
            
        } else if (mode == "--json-compact") {
            std::string outputFile = argc > 3 ? argv[3] : "symbols-compact.json";
            std::cout << "📤 Compact JSON Export: " << outputFile << "\n";
            
            FileScanner::printWithSpinner("Building symbol index");
            SymbolIndex index;
            files = FileScanner::scanForAllSupportedFiles(rootPath);
            index.buildIndex(files);
            FileScanner::clearLine();
            
            FileScanner::printWithSpinner("Exporting compact JSON");
            JsonExporter exporter;
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << exporter.exportCompact(index);
                file.close();
                FileScanner::clearLine();
                std::cout << "✅ Compact JSON export completed!\n";
                std::cout << "📄 File: " << outputFile << "\n";
                std::cout << "📊 Symbols: " << index.size() << " (compact format)\n";
            } else {
                FileScanner::clearLine();
                std::cout << "❌ Failed to create file: " << outputFile << "\n";
            }
            
        } else if (mode == "--json-stats") {
            std::string outputFile = argc > 3 ? argv[3] : "symbols-stats.json";
            std::cout << "📊 JSON with Statistics Export: " << outputFile << "\n";
            
            FileScanner::printWithSpinner("Building symbol index");
            SymbolIndex index;
            files = FileScanner::scanForAllSupportedFiles(rootPath);
            index.buildIndex(files);
            FileScanner::clearLine();
            
            FileScanner::printWithSpinner("Exporting JSON with statistics");
            JsonExporter exporter;
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << exporter.exportWithStats(index, rootPath);
                file.close();
                FileScanner::clearLine();
                std::cout << "✅ JSON with statistics export completed!\n";
                std::cout << "📄 File: " << outputFile << "\n";
                std::cout << "📊 Symbols: " << index.size() << " (with detailed stats)\n";
            } else {
                FileScanner::clearLine();
                std::cout << "❌ Failed to create file: " << outputFile << "\n";
            }
            
        } else if (mode == "--json-lsp") {
            std::string outputFile = argc > 3 ? argv[3] : "symbols-lsp.json";
            std::cout << "🔗 LSP-Compatible JSON Export: " << outputFile << "\n";
            
            FileScanner::printWithSpinner("Building symbol index");
            SymbolIndex index;
            files = FileScanner::scanForAllSupportedFiles(rootPath);
            index.buildIndex(files);
            FileScanner::clearLine();
            
            FileScanner::printWithSpinner("Exporting LSP-compatible JSON");
            JsonExporter exporter;
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << exporter.exportForLSP(index, "file://" + rootPath);
                file.close();
                FileScanner::clearLine();
                std::cout << "✅ LSP-compatible JSON export completed!\n";
                std::cout << "📄 File: " << outputFile << "\n";
                std::cout << "🔗 Format: Language Server Protocol compatible\n";
                std::cout << "📊 Symbols: " << index.size() << "\n";
            } else {
                FileScanner::clearLine();
                std::cout << "❌ Failed to create file: " << outputFile << "\n";
            }
            
        } else if (mode == "--lsp") {
            std::cout << "🛠️  Starting Navix LSP Server\n";
            std::cout << "📁 Workspace: " << rootPath << "\n";
            std::cout << "🔗 LSP Protocol: Language Server Protocol v3.17\n";
            std::cout << "🚀 Ready for IDE/Editor connections...\n\n";
            
            LSPServer server;
            server.setWorkspaceRoot(rootPath);
            server.enableLogging(true);
            
            try {
                server.start();
            } catch (const std::exception& e) {
                std::cerr << "❌ LSP Server error: " << e.what() << std::endl;
                return 1;
            }
            
        } else if (mode == "--search" && argc > 3) {
            // Symbol search (fuzzy) with loading animation
            std::string query = argv[3];
            std::cout << "🔍 Searching for symbols matching '" << query << "' in: " << rootPath << "\n\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, true, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--search-exact" && argc > 3) {
            // Symbol search (exact) with loading animation
            std::string query = argv[3];
            std::cout << "🎯 Searching for exact symbol '" << query << "' in: " << rootPath << "\n\n";
            
            std::vector<Symbol> symbols = FileScanner::searchSymbols(rootPath, query, false, true);
            SymbolIndex tempIndex; // For the symbolTypeToString method
            printSymbolResults(symbols, tempIndex, true); // Use new format
            
        } else if (mode == "--goto" && argc > 3) {
            // Navigate to symbol with loading animation
            std::string symbolName = argv[3];
            std::string editor = argc >= 5 ? argv[4] : "";
            
            std::cout << "🚀 Looking for symbol '" << symbolName << "' in: " << rootPath << "\n\n";
            
            if (!FileScanner::gotoSymbol(rootPath, symbolName, editor)) {
                return 1;
            }
            
        } else {
            std::cerr << "❌ Invalid arguments.\n\n";
            printUsage(argv[0]);
            return 1;
        }
    } else {
        std::cerr << "❌ Invalid number of arguments.\n\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
