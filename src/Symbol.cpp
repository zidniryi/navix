#include "Symbol.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>

void SymbolIndex::addSymbol(const Symbol& symbol) {
    symbols.push_back(symbol);
}

void SymbolIndex::buildIndex(const std::vector<std::string>& files) {
    clear();
    for (const auto& file : files) {
        parseFile(file);
    }
}

std::vector<Symbol> SymbolIndex::search(const std::string& query, bool fuzzy) const {
    if (fuzzy) {
        return fuzzySearch(query);
    } else {
        return exactSearch(query);
    }
}

std::vector<Symbol> SymbolIndex::exactSearch(const std::string& query) const {
    std::vector<Symbol> results;
    for (const auto& symbol : symbols) {
        if (symbol.name == query) {
            results.push_back(symbol);
        }
    }
    return results;
}

std::vector<Symbol> SymbolIndex::fuzzySearch(const std::string& query) const {
    std::vector<std::pair<Symbol, int>> scored_results;
    
    for (const auto& symbol : symbols) {
        // Check for exact match first
        if (symbol.name == query) {
            scored_results.push_back({symbol, 0});
            continue;
        }
        
        // Check for prefix match
        if (isPrefixMatch(symbol.name, query)) {
            scored_results.push_back({symbol, 1});
            continue;
        }
        
        // Check for substring match
        if (symbol.name.find(query) != std::string::npos) {
            scored_results.push_back({symbol, 2});
            continue;
        }
        
        // Levenshtein distance check (only for reasonable distances)
        int distance = levenshteinDistance(symbol.name, query);
        if (distance <= 3 && distance < static_cast<int>(query.length())) {
            scored_results.push_back({symbol, distance + 10});
        }
    }
    
    // Sort by score (lower is better)
    std::sort(scored_results.begin(), scored_results.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::vector<Symbol> results;
    for (const auto& scored : scored_results) {
        results.push_back(scored.first);
    }
    
    return results;
}

void SymbolIndex::clear() {
    symbols.clear();
}

size_t SymbolIndex::size() const {
    return symbols.size();
}

void SymbolIndex::parseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(file, line)) {
        // Remove leading whitespace for parsing
        std::string trimmed = line;
        trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(),
                      [](unsigned char ch) { return !std::isspace(ch); }));
        
        // Skip comments and empty lines
        if (trimmed.empty() || trimmed.substr(0, 2) == "//" || trimmed[0] == '#') {
            lineNumber++;
            continue;
        }
        
        // Parse different symbol types
        parseLineForSymbols(trimmed, filePath, lineNumber);
        lineNumber++;
    }
}

void SymbolIndex::parseLineForSymbols(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Function declarations/definitions - simplified regex
    std::regex functionRegex(R"(\b(\w+)\s*\([^)]*\)\s*[{;])");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        // Filter out common keywords
        if (name != "if" && name != "while" && name != "for" && name != "switch" && 
            name != "return" && name != "include" && name != "define") {
            addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
        }
    }
    
    // Class declarations - simplified
    std::regex classRegex(R"(\b(class|struct)\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[2].str();
        SymbolType type = (match[1].str() == "struct") ? SymbolType::STRUCT : SymbolType::CLASS;
        addSymbol(Symbol(name, type, filePath, lineNumber, line));
    }
    
    // Enum declarations
    std::regex enumRegex(R"(\benum\s+(?:class\s+)?(\w+))");
    if (std::regex_search(line, match, enumRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::ENUM, filePath, lineNumber, line));
    }
    
    // Namespace declarations
    std::regex namespaceRegex(R"(\bnamespace\s+(\w+))");
    if (std::regex_search(line, match, namespaceRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::NAMESPACE, filePath, lineNumber, line));
    }
    
    // Variable declarations - very simplified
    std::regex varRegex(R"(\b(?:int|float|double|char|bool|string|auto)\s+(\w+)\s*[=;])");
    if (std::regex_search(line, match, varRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::VARIABLE, filePath, lineNumber, line));
    }
    
    // Typedef declarations
    std::regex typedefRegex(R"(\btypedef\s+.+\s+(\w+)\s*;)");
    if (std::regex_search(line, match, typedefRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::TYPEDEF, filePath, lineNumber, line));
    }
    
    // Macro definitions
    std::regex macroRegex(R"(#define\s+(\w+))");
    if (std::regex_search(line, match, macroRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::MACRO, filePath, lineNumber, line));
    }
}

int SymbolIndex::levenshteinDistance(const std::string& s1, const std::string& s2) const {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));
    
    for (size_t i = 0; i <= len1; ++i) d[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) d[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            d[i][j] = std::min({
                d[i - 1][j] + 1,
                d[i][j - 1] + 1,
                d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1)
            });
        }
    }
    
    return d[len1][len2];
}

bool SymbolIndex::isPrefixMatch(const std::string& symbol, const std::string& query) const {
    if (query.length() > symbol.length()) return false;
    return std::equal(query.begin(), query.end(), symbol.begin(),
                     [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

std::string SymbolIndex::symbolTypeToString(SymbolType type) const {
    switch (type) {
        case SymbolType::FUNCTION: return "function";
        case SymbolType::CLASS: return "class";
        case SymbolType::STRUCT: return "struct";
        case SymbolType::VARIABLE: return "variable";
        case SymbolType::ENUM: return "enum";
        case SymbolType::TYPEDEF: return "typedef";
        case SymbolType::MACRO: return "macro";
        case SymbolType::NAMESPACE: return "namespace";
        case SymbolType::UNKNOWN: return "unknown";
        default: return "unknown";
    }
} 