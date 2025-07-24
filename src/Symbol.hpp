#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>

enum class SymbolType {
    FUNCTION,
    CLASS,
    STRUCT,
    VARIABLE,
    ENUM,
    TYPEDEF,
    MACRO,
    NAMESPACE,
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
    
public:
    void addSymbol(const Symbol& symbol);
    void buildIndex(const std::vector<std::string>& files);
    std::vector<Symbol> search(const std::string& query, bool fuzzy = true) const;
    std::vector<Symbol> exactSearch(const std::string& query) const;
    std::vector<Symbol> fuzzySearch(const std::string& query) const;
    void clear();
    size_t size() const;
    std::string symbolTypeToString(SymbolType type) const;
    
private:
    void parseFile(const std::string& filePath);
    void parseLineForSymbols(const std::string& line, const std::string& filePath, int lineNumber);
    int levenshteinDistance(const std::string& s1, const std::string& s2) const;
    bool isPrefixMatch(const std::string& symbol, const std::string& query) const;
    
    // C++ parsing helpers
    bool isFunction(const std::string& line, size_t pos) const;
    bool isClass(const std::string& line, size_t pos) const;
    bool isVariable(const std::string& line, size_t pos) const;
    std::string extractSymbolName(const std::string& line, size_t startPos) const;
};

#endif // SYMBOL_HPP 