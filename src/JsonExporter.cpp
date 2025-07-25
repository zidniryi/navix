#include "JsonExporter.hpp"
#include <iostream>
#include <iomanip>
#include <set>
#include <algorithm>

std::string JsonExporter::exportToJson(const SymbolIndex& index, bool pretty) const {
    std::ostringstream json;
    const std::string indent = pretty ? "  " : "";
    const std::string newline = pretty ? "\n" : "";
    
    json << "{" << newline;
    json << indent << "\"metadata\": {" << newline;
    json << indent << indent << "\"timestamp\": \"" << getCurrentTimestamp() << "\"," << newline;
    json << indent << indent << "\"version\": \"0.1\"," << newline;
    json << indent << indent << "\"generator\": \"Navix Code Intelligence Platform\"," << newline;
    json << indent << indent << "\"symbolCount\": " << index.size() << newline;
    json << indent << "}," << newline;
    
    json << indent << "\"symbols\": [" << newline;
    
    const auto& symbols = index.getSymbols();
    for (size_t i = 0; i < symbols.size(); ++i) {
        json << symbolToJson(symbols[i], pretty ? 2 : 0);
        if (i < symbols.size() - 1) {
            json << ",";
        }
        json << newline;
    }
    
    json << indent << "]" << newline;
    json << "}" << newline;
    
    return json.str();
}

bool JsonExporter::exportToFile(const SymbolIndex& index, const std::string& filePath, bool pretty) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "❌ Error: Could not create JSON file: " << filePath << std::endl;
        return false;
    }
    
    file << exportToJson(index, pretty);
    file.close();
    
    std::cout << "✅ Exported " << index.size() << " symbols to JSON: " << filePath << std::endl;
    return true;
}

std::string JsonExporter::exportForLSP(const SymbolIndex& index, const std::string& uri) const {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"jsonrpc\": \"2.0\",\n";
    json << "  \"method\": \"textDocument/documentSymbol\",\n";
    json << "  \"params\": {\n";
    
    if (!uri.empty()) {
        json << "    \"textDocument\": {\n";
        json << "      \"uri\": \"" << escapeJson(uri) << "\"\n";
        json << "    },\n";
    }
    
    json << "    \"symbols\": [\n";
    
    const auto& symbols = index.getSymbols();
    for (size_t i = 0; i < symbols.size(); ++i) {
        json << symbolToLSP(symbols[i]);
        if (i < symbols.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "    ]\n";
    json << "  }\n";
    json << "}\n";
    
    return json.str();
}

std::string JsonExporter::exportDocumentSymbols(const SymbolIndex& index, const std::string& filePath) const {
    std::ostringstream json;
    
    json << "[\n";
    
    const auto& symbols = index.getSymbols();
    std::vector<Symbol> fileSymbols;
    
    // Filter symbols for the specific file
    for (const auto& symbol : symbols) {
        if (symbol.file == filePath) {
            fileSymbols.push_back(symbol);
        }
    }
    
    for (size_t i = 0; i < fileSymbols.size(); ++i) {
        json << symbolToLSP(fileSymbols[i]);
        if (i < fileSymbols.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "]\n";
    
    return json.str();
}

std::string JsonExporter::exportWorkspaceSymbols(const SymbolIndex& index, const std::string& query) const {
    std::ostringstream json;
    
    json << "[\n";
    
    const auto& symbols = index.getSymbols();
    std::vector<Symbol> filteredSymbols;
    
    // Filter symbols by query if provided
    if (query.empty()) {
        filteredSymbols = symbols;
    } else {
        for (const auto& symbol : symbols) {
            if (symbol.name.find(query) != std::string::npos) {
                filteredSymbols.push_back(symbol);
            }
        }
    }
    
    for (size_t i = 0; i < filteredSymbols.size(); ++i) {
        json << "    {\n";
        json << "      \"name\": \"" << escapeJson(filteredSymbols[i].name) << "\",\n";
        json << "      \"kind\": " << symbolTypeToLSPKind(filteredSymbols[i].type) << ",\n";
        json << "      \"location\": {\n";
        json << "        \"uri\": \"file://" << escapeJson(filteredSymbols[i].file) << "\",\n";
        json << "        \"range\": {\n";
        json << "          \"start\": { \"line\": " << (filteredSymbols[i].line - 1) << ", \"character\": 0 },\n";
        json << "          \"end\": { \"line\": " << (filteredSymbols[i].line - 1) << ", \"character\": " << filteredSymbols[i].name.length() << " }\n";
        json << "        }\n";
        json << "      },\n";
        json << "      \"containerName\": \"" << escapeJson(filteredSymbols[i].file) << "\"\n";
        json << "    }";
        if (i < filteredSymbols.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "]\n";
    
    return json.str();
}

std::string JsonExporter::exportCompact(const SymbolIndex& index) const {
    std::ostringstream json;
    
    json << "[";
    
    const auto& symbols = index.getSymbols();
    for (size_t i = 0; i < symbols.size(); ++i) {
        json << "{\"n\":\"" << escapeJson(symbols[i].name) << "\","
             << "\"t\":\"" << symbolTypeToString(symbols[i].type) << "\","
             << "\"f\":\"" << escapeJson(symbols[i].file) << "\","
             << "\"l\":" << symbols[i].line << "}";
        if (i < symbols.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    
    return json.str();
}

std::string JsonExporter::exportWithStats(const SymbolIndex& index, const std::string& projectPath) const {
    std::ostringstream json;
    
    // Gather statistics
    std::map<std::string, int> languageStats;
    std::map<std::string, int> typeStats;
    std::set<std::string> uniqueFiles;
    
    const auto& symbols = index.getSymbols();
    for (const auto& symbol : symbols) {
        // Extract language from file extension
        std::string language = "unknown";
        std::string ext = symbol.file.substr(symbol.file.find_last_of('.') + 1);
        if (ext == "cpp" || ext == "hpp" || ext == "h" || ext == "cc" || ext == "cxx") {
            language = "C++";
        } else if (ext == "ts" || ext == "tsx") {
            language = "TypeScript";
        } else if (ext == "js" || ext == "jsx" || ext == "mjs" || ext == "cjs") {
            language = "JavaScript";
        } else if (ext == "py" || ext == "pyw" || ext == "pyi") {
            language = "Python";
        } else if (ext == "go") {
            language = "Go";
        } else if (ext == "swift") {
            language = "Swift";
        } else if (ext == "txt" || ext == "md" || ext == "rst" || ext == "log") {
            language = "Text";
        }
        
        languageStats[language]++;
        typeStats[symbolTypeToString(symbol.type)]++;
        uniqueFiles.insert(symbol.file);
    }
    
    json << "{\n";
    json << "  \"project\": {\n";
    json << "    \"path\": \"" << escapeJson(projectPath) << "\",\n";
    json << "    \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "    \"symbolCount\": " << symbols.size() << ",\n";
    json << "    \"fileCount\": " << uniqueFiles.size() << "\n";
    json << "  },\n";
    
    json << "  \"statistics\": {\n";
    json << "    \"byLanguage\": {\n";
    bool first = true;
    for (const auto& [lang, count] : languageStats) {
        if (!first) json << ",\n";
        json << "      \"" << escapeJson(lang) << "\": " << count;
        first = false;
    }
    json << "\n    },\n";
    
    json << "    \"byType\": {\n";
    first = true;
    for (const auto& [type, count] : typeStats) {
        if (!first) json << ",\n";
        json << "      \"" << escapeJson(type) << "\": " << count;
        first = false;
    }
    json << "\n    }\n";
    json << "  },\n";
    
    json << "  \"symbols\": [\n";
    for (size_t i = 0; i < symbols.size(); ++i) {
        json << symbolToJson(symbols[i], 2);
        if (i < symbols.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    json << "  ]\n";
    json << "}\n";
    
    return json.str();
}

std::string JsonExporter::exportByLanguage(const SymbolIndex& index) const {
    std::ostringstream json;
    std::map<std::string, std::vector<Symbol>> symbolsByLanguage;
    
    const auto& symbols = index.getSymbols();
    for (const auto& symbol : symbols) {
        // Extract language from file extension
        std::string language = "unknown";
        std::string ext = symbol.file.substr(symbol.file.find_last_of('.') + 1);
        if (ext == "cpp" || ext == "hpp" || ext == "h" || ext == "cc" || ext == "cxx") {
            language = "C++";
        } else if (ext == "ts" || ext == "tsx") {
            language = "TypeScript";
        } else if (ext == "js" || ext == "jsx" || ext == "mjs" || ext == "cjs") {
            language = "JavaScript";
        } else if (ext == "py" || ext == "pyw" || ext == "pyi") {
            language = "Python";
        } else if (ext == "go") {
            language = "Go";
        } else if (ext == "swift") {
            language = "Swift";
        } else if (ext == "txt" || ext == "md" || ext == "rst" || ext == "log") {
            language = "Text";
        }
        symbolsByLanguage[language].push_back(symbol);
    }
    
    json << "{\n";
    bool firstLang = true;
    for (const auto& [language, langSymbols] : symbolsByLanguage) {
        if (!firstLang) json << ",\n";
        json << "  \"" << escapeJson(language) << "\": [\n";
        
        for (size_t i = 0; i < langSymbols.size(); ++i) {
            json << symbolToJson(langSymbols[i], 2);
            if (i < langSymbols.size() - 1) {
                json << ",";
            }
            json << "\n";
        }
        
        json << "  ]";
        firstLang = false;
    }
    json << "\n}\n";
    
    return json.str();
}

std::string JsonExporter::escapeJson(const std::string& str) const {
    std::ostringstream escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c >= 0 && c < 32) {
                    escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    escaped << c;
                }
                break;
        }
    }
    return escaped.str();
}

std::string JsonExporter::symbolToJson(const Symbol& symbol, int indent) const {
    std::ostringstream json;
    const std::string ind = getIndent(indent);
    const std::string indNext = getIndent(indent + 1);
    
    json << ind << "{\n";
    json << indNext << "\"name\": \"" << escapeJson(symbol.name) << "\",\n";
    json << indNext << "\"type\": \"" << symbolTypeToString(symbol.type) << "\",\n";
    json << indNext << "\"file\": \"" << escapeJson(symbol.file) << "\",\n";
    json << indNext << "\"line\": " << symbol.line << ",\n";
    json << indNext << "\"context\": \"" << escapeJson(symbol.context) << "\"\n";
    json << ind << "}";
    
    return json.str();
}

std::string JsonExporter::symbolToLSP(const Symbol& symbol) const {
    std::ostringstream json;
    
    json << "      {\n";
    json << "        \"name\": \"" << escapeJson(symbol.name) << "\",\n";
    json << "        \"kind\": " << symbolTypeToLSPKind(symbol.type) << ",\n";
    json << "        \"location\": {\n";
    json << "          \"uri\": \"file://" << escapeJson(symbol.file) << "\",\n";
    json << "          \"range\": {\n";
    json << "            \"start\": { \"line\": " << (symbol.line - 1) << ", \"character\": 0 },\n";
    json << "            \"end\": { \"line\": " << (symbol.line - 1) << ", \"character\": " << symbol.name.length() << " }\n";
    json << "          }\n";
    json << "        },\n";
    json << "        \"detail\": \"" << escapeJson(symbol.context) << "\"\n";
    json << "      }";
    
    return json.str();
}

int JsonExporter::symbolTypeToLSPKind(SymbolType type) const {
    // LSP SymbolKind constants
    switch (type) {
        case SymbolType::FUNCTION:
        case SymbolType::JS_FUNCTION:
        case SymbolType::PY_FUNCTION:
        case SymbolType::GO_FUNCTION:
        case SymbolType::SWIFT_FUNCTION:
            return 12; // Function
            
        case SymbolType::CLASS:
        case SymbolType::JS_CLASS:
        case SymbolType::PY_CLASS:
        case SymbolType::SWIFT_CLASS:
            return 5; // Class
            
        case SymbolType::STRUCT:
        case SymbolType::GO_STRUCT:
        case SymbolType::SWIFT_STRUCT:
            return 23; // Struct
            
        case SymbolType::ENUM:
        case SymbolType::SWIFT_ENUM:
            return 10; // Enum
            
        case SymbolType::VARIABLE:
        case SymbolType::PY_VARIABLE:
        case SymbolType::GO_VARIABLE:
        case SymbolType::SWIFT_VARIABLE:
            return 13; // Variable
            
        case SymbolType::NAMESPACE:
            return 3; // Namespace
            
        case SymbolType::JS_IMPORT:
        case SymbolType::PY_IMPORT:
        case SymbolType::PY_FROM_IMPORT:
        case SymbolType::GO_IMPORT:
        case SymbolType::SWIFT_IMPORT:
            return 9; // Module
            
        case SymbolType::GO_METHOD:
        case SymbolType::SWIFT_METHOD:
            return 6; // Method
            
        case SymbolType::GO_INTERFACE:
        case SymbolType::SWIFT_PROTOCOL:
            return 11; // Interface
            
        case SymbolType::GO_TYPE:
            return 7; // Property
            
        case SymbolType::GO_CONSTANT:
        case SymbolType::SWIFT_CONSTANT:
            return 14; // Constant
            
        case SymbolType::GO_PACKAGE:
            return 4; // Package
            
        case SymbolType::SWIFT_EXTENSION:
            return 8; // Constructor
            
        case SymbolType::SWIFT_PROPERTY:
            return 7; // Property
            
        case SymbolType::SWIFT_INITIALIZER:
            return 9; // Constructor
            
        case SymbolType::SWIFT_SUBSCRIPT:
            return 7; // Property
            
        case SymbolType::TXT_HEADER:
            return 1; // File
            
        case SymbolType::TXT_TODO:
        case SymbolType::TXT_URL:
        case SymbolType::TXT_EMAIL:
            return 15; // String
            
        default:
            return 1; // File (fallback)
    }
}

std::string JsonExporter::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string JsonExporter::getIndent(int level) const {
    return std::string(level * 2, ' ');
}

std::string JsonExporter::symbolTypeToString(SymbolType type) const {
    switch (type) {
        case SymbolType::FUNCTION: return "function";
        case SymbolType::CLASS: return "class";
        case SymbolType::STRUCT: return "struct";
        case SymbolType::ENUM: return "enum";
        case SymbolType::VARIABLE: return "variable";
        case SymbolType::NAMESPACE: return "namespace";
        case SymbolType::JS_FUNCTION: return "js_function";
        case SymbolType::JS_CLASS: return "js_class";

        case SymbolType::JS_IMPORT: return "js_import";
        case SymbolType::PY_FUNCTION: return "py_function";
        case SymbolType::PY_CLASS: return "py_class";
        case SymbolType::PY_VARIABLE: return "py_variable";
        case SymbolType::PY_IMPORT: return "py_import";
        case SymbolType::PY_FROM_IMPORT: return "py_from_import";
        case SymbolType::GO_FUNCTION: return "go_function";
        case SymbolType::GO_METHOD: return "go_method";
        case SymbolType::GO_STRUCT: return "go_struct";
        case SymbolType::GO_INTERFACE: return "go_interface";
        case SymbolType::GO_TYPE: return "go_type";
        case SymbolType::GO_VARIABLE: return "go_variable";
        case SymbolType::GO_CONSTANT: return "go_constant";
        case SymbolType::GO_PACKAGE: return "go_package";
        case SymbolType::GO_IMPORT: return "go_import";
        case SymbolType::SWIFT_FUNCTION: return "swift_function";
        case SymbolType::SWIFT_METHOD: return "swift_method";
        case SymbolType::SWIFT_CLASS: return "swift_class";
        case SymbolType::SWIFT_STRUCT: return "swift_struct";
        case SymbolType::SWIFT_PROTOCOL: return "swift_protocol";
        case SymbolType::SWIFT_ENUM: return "swift_enum";
        case SymbolType::SWIFT_EXTENSION: return "swift_extension";
        case SymbolType::SWIFT_VARIABLE: return "swift_variable";
        case SymbolType::SWIFT_CONSTANT: return "swift_constant";
        case SymbolType::SWIFT_PROPERTY: return "swift_property";
        case SymbolType::SWIFT_INITIALIZER: return "swift_initializer";
        case SymbolType::SWIFT_SUBSCRIPT: return "swift_subscript";
        case SymbolType::SWIFT_IMPORT: return "swift_import";
        case SymbolType::TXT_HEADER: return "txt_header";
        case SymbolType::TXT_TODO: return "txt_todo";
        case SymbolType::TXT_URL: return "txt_url";
        case SymbolType::TXT_EMAIL: return "txt_email";

        default: return "unknown";
    }
} 