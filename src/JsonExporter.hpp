#pragma once

#include "Symbol.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <map>

class JsonExporter {
public:
    JsonExporter() = default;
    ~JsonExporter() = default;
    
    // Core export methods
    std::string exportToJson(const SymbolIndex& index, bool pretty = true) const;
    bool exportToFile(const SymbolIndex& index, const std::string& filePath, bool pretty = true) const;
    
    // LSP-compatible exports
    std::string exportForLSP(const SymbolIndex& index, const std::string& uri = "") const;
    std::string exportDocumentSymbols(const SymbolIndex& index, const std::string& filePath) const;
    std::string exportWorkspaceSymbols(const SymbolIndex& index, const std::string& query = "") const;
    
    // Specialized formats
    std::string exportCompact(const SymbolIndex& index) const;
    std::string exportWithStats(const SymbolIndex& index, const std::string& projectPath) const;
    std::string exportByLanguage(const SymbolIndex& index) const;

private:
    // JSON formatting helpers
    std::string escapeJson(const std::string& str) const;
    std::string symbolToJson(const Symbol& symbol, int indent = 0) const;
    std::string symbolToLSP(const Symbol& symbol) const;
    
    // LSP helpers
    int symbolTypeToLSPKind(SymbolType type) const;
    std::string getCurrentTimestamp() const;
    std::string getIndent(int level) const;
    
    // Symbol type conversion
    std::string symbolTypeToString(SymbolType type) const;
}; 