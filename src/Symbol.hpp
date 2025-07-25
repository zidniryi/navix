#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>

// Forward declaration
class PerformanceLogger;

enum class SymbolType {
    // C++ symbols
    FUNCTION,
    CLASS,
    STRUCT,
    VARIABLE,
    ENUM,
    TYPEDEF,
    MACRO,
    NAMESPACE,
    
    // TypeScript/JavaScript symbols
    JS_FUNCTION,
    JS_ARROW_FUNCTION,
    JS_CLASS,
    JS_INTERFACE,
    JS_TYPE,
    JS_CONST,
    JS_LET,
    JS_VAR,
    JS_IMPORT,
    JS_EXPORT,
    JS_MODULE,
    
    // Python symbols
    PY_FUNCTION,
    PY_CLASS,
    PY_METHOD,
    PY_VARIABLE,
    PY_IMPORT,
    PY_FROM_IMPORT,
    PY_DECORATOR,
    PY_LAMBDA,
    
    // Go symbols
    GO_FUNCTION,
    GO_METHOD,
    GO_STRUCT,
    GO_INTERFACE,
    GO_TYPE,
    GO_VARIABLE,
    GO_CONSTANT,
    GO_PACKAGE,
    GO_IMPORT,
    
    // Swift symbols
    SWIFT_FUNCTION,
    SWIFT_METHOD,
    SWIFT_CLASS,
    SWIFT_STRUCT,
    SWIFT_PROTOCOL,
    SWIFT_ENUM,
    SWIFT_EXTENSION,
    SWIFT_VARIABLE,
    SWIFT_CONSTANT,
    SWIFT_PROPERTY,
    SWIFT_INITIALIZER,
    SWIFT_SUBSCRIPT,
    SWIFT_IMPORT,
    
    // Text file symbols
    TXT_HEADER,
    TXT_SUBHEADER,
    TXT_URL,
    TXT_EMAIL,
    TXT_TODO,
    TXT_NOTE,
    TXT_FIXME,
    TXT_LINE,
    TXT_WORD,
    
    UNKNOWN
};

struct Symbol {
    std::string name;
    SymbolType type;
    std::string file;
    int line;
    std::string context; // Optional: surrounding context or signature
    
    Symbol(const std::string& n, SymbolType t, const std::string& f, int l, const std::string& c = "")
        : name(n), type(t), file(f), line(l), context(c) {}
};

class SymbolIndex {
private:
    std::vector<Symbol> symbols;
    PerformanceLogger* m_logger; // Optional performance logger
    
public:
    SymbolIndex();
    ~SymbolIndex() = default;
    
    // Performance logging
    void setPerformanceLogger(PerformanceLogger* logger);
    
    // Core functionality
    void addSymbol(const Symbol& symbol);
    void buildIndex(const std::vector<std::string>& files);
    std::vector<Symbol> search(const std::string& query, bool fuzzy = true) const;
    std::vector<Symbol> exactSearch(const std::string& query) const;
    std::vector<Symbol> fuzzySearch(const std::string& query) const;
    void clear();
    size_t size() const;
    std::string symbolTypeToString(SymbolType type) const;
    
    // Access to symbols for autocomplete
    const std::vector<Symbol>& getSymbols() const;
    
private:
    void parseFile(const std::string& filePath);
    void parseLineForSymbols(const std::string& line, const std::string& filePath, int lineNumber);
    void parseTypeScriptJavaScript(const std::string& line, const std::string& filePath, int lineNumber);
    void parsePython(const std::string& line, const std::string& filePath, int lineNumber);
    void parseGo(const std::string& line, const std::string& filePath, int lineNumber);
    void parsePlainText(const std::string& line, const std::string& filePath, int lineNumber);
    void parseSwift(const std::string& line, const std::string& filePath, int lineNumber);
    bool isTypeScriptOrJavaScript(const std::string& filePath) const;
    bool isPython(const std::string& filePath) const;
    bool isGo(const std::string& filePath) const;
    bool isPlainText(const std::string& filePath) const;
    bool isSwift(const std::string& filePath) const;
    int levenshteinDistance(const std::string& s1, const std::string& s2) const;
    bool isPrefixMatch(const std::string& symbol, const std::string& query) const;
    
    // C++ parsing helpers
    bool isFunction(const std::string& line, size_t pos) const;
    bool isClass(const std::string& line, size_t pos) const;
    bool isVariable(const std::string& line, size_t pos) const;
    std::string extractSymbolName(const std::string& line, size_t startPos) const;
    
    // Text parsing helpers
    bool isHeader(const std::string& line) const;
    bool isSubHeader(const std::string& line) const;
    bool isSignificantLine(const std::string& line) const;
    std::string extractUrlOrEmail(const std::string& line, bool isEmail = false) const;
    
    // Performance helpers
    std::string getLanguageFromPath(const std::string& filePath) const;
};

#endif // SYMBOL_HPP 