#include "Symbol.hpp"
#include "PerformanceLogger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>
#include <set> // Added for SymbolIndex::loadSymbols workaround

SymbolIndex::SymbolIndex() : m_logger(nullptr) {
}

void SymbolIndex::setPerformanceLogger(PerformanceLogger* logger) {
    m_logger = logger;
}

void SymbolIndex::addSymbol(const Symbol& symbol) {
    symbols.push_back(symbol);
    
    if (m_logger) {
        m_logger->logSymbol(symbolTypeToString(symbol.type));
    }
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

const std::vector<Symbol>& SymbolIndex::getSymbols() const {
    return symbols;
}

bool SymbolIndex::isTypeScriptOrJavaScript(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".ts" || ext == ".tsx" || ext == ".js" || ext == ".jsx" || 
           ext == ".mjs" || ext == ".cjs" || ext == ".d.ts";
}

bool SymbolIndex::isPython(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".py" || ext == ".pyw" || ext == ".pyi";
}

bool SymbolIndex::isGo(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".go";
}

bool SymbolIndex::isPlainText(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".txt" || ext == ".text" || ext == ".md" || ext == ".rst" || 
           ext == ".log" || ext == ".readme" || ext == ".doc";
}

bool SymbolIndex::isSwift(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".swift";
}

bool SymbolIndex::isKotlin(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".kt";
}

bool SymbolIndex::isJava(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".java";
}

bool SymbolIndex::isPHP(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".php" || ext == ".phtml" || ext == ".php3" || 
           ext == ".php4" || ext == ".php5" || ext == ".phps";
}

bool SymbolIndex::isBash(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".sh" || ext == ".bash" || ext == ".zsh" || 
           ext == ".fish" || ext == ".ksh" || ext == ".csh";
}

bool SymbolIndex::isRuby(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".rb" || ext == ".rbw" || ext == ".rake" || ext == ".gemspec";
}

bool SymbolIndex::isRust(const std::string& filePath) const {
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    return ext == ".rs";
}

void SymbolIndex::parseFile(const std::string& filePath) {
    std::unique_ptr<FileTimer> timer;
    size_t symbolCountBefore = symbols.size();
    
    if (m_logger) {
        timer = std::make_unique<FileTimer>(*m_logger, filePath);
        timer->setLanguage(getLanguageFromPath(filePath));
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        if (m_logger) {
            m_logger->logError(filePath, "Could not open file");
        }
        return;
    }
    
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(file, line)) {
        // Remove leading whitespace for parsing
        std::string trimmed = line;
        trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(),
                      [](unsigned char ch) { return !std::isspace(ch); }));
        
        // Skip comments and empty lines (but not for plain text files)
        if (!isPlainText(filePath) && (trimmed.empty() || trimmed.substr(0, 2) == "//" || 
            (trimmed[0] == '#' && !isPython(filePath) && !isBash(filePath) && !isRuby(filePath)))) {
            lineNumber++;
            continue;
        }
        
        // Parse based on file type
        if (isPlainText(filePath)) {
            parsePlainText(line, filePath, lineNumber);  // Use original line with whitespace
        } else if (isSwift(filePath)) {
            parseSwift(trimmed, filePath, lineNumber);
        } else if (isKotlin(filePath)) {
            parseKotlin(trimmed, filePath, lineNumber);
        } else if (isJava(filePath)) {
            parseJava(trimmed, filePath, lineNumber);
        } else if (isPHP(filePath)) {
            parsePHP(trimmed, filePath, lineNumber);
        } else if (isBash(filePath)) {
            parseBash(trimmed, filePath, lineNumber);
        } else if (isRuby(filePath)) {
            parseRuby(trimmed, filePath, lineNumber);
        } else if (isRust(filePath)) {
            parseRust(trimmed, filePath, lineNumber);
        } else if (isGo(filePath)) {
            parseGo(trimmed, filePath, lineNumber);
        } else if (isPython(filePath)) {
            parsePython(trimmed, filePath, lineNumber);
        } else if (isTypeScriptOrJavaScript(filePath)) {
            parseTypeScriptJavaScript(trimmed, filePath, lineNumber);
        } else {
            parseLineForSymbols(trimmed, filePath, lineNumber);  // Unknown file type - generic parsing
        }
        lineNumber++;
    }
    
    // Update performance metrics
    if (timer) {
        size_t symbolsFound = symbols.size() - symbolCountBefore;
        timer->setSymbolCount(symbolsFound);
    }
}

void SymbolIndex::parseGo(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Function definitions: func functionName( or func (receiver) functionName(
    std::regex functionRegex(R"(\bfunc\s+(?:\([^)]+\)\s+)?(\w+)\s*\()");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        // Check if it's a method (has receiver)
        SymbolType type = line.find("func (") == 0 ? SymbolType::GO_METHOD : SymbolType::GO_FUNCTION;
        addSymbol(Symbol(name, type, filePath, lineNumber, line));
    }
    
    // Struct definitions: type StructName struct
    std::regex structRegex(R"(\btype\s+(\w+)\s+struct\b)");
    if (std::regex_search(line, match, structRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::GO_STRUCT, filePath, lineNumber, line));
    }
    
    // Interface definitions: type InterfaceName interface
    std::regex interfaceRegex(R"(\btype\s+(\w+)\s+interface\b)");
    if (std::regex_search(line, match, interfaceRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::GO_INTERFACE, filePath, lineNumber, line));
    }
    
    // Type definitions: type TypeName = or type TypeName SomeType
    std::regex typeRegex(R"(\btype\s+(\w+)\s+(?!=\s*struct\b|interface\b)(\w+|\[|\*))");
    if (std::regex_search(line, match, typeRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::GO_TYPE, filePath, lineNumber, line));
    }
    
    // Variable declarations: var varName type or var varName = value
    std::regex varRegex(R"(\bvar\s+(\w+)\s+)");
    if (std::regex_search(line, match, varRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::GO_VARIABLE, filePath, lineNumber, line));
    }
    
    // Constant declarations: const constName = value or const constName type = value
    std::regex constRegex(R"(\bconst\s+(\w+)\s+)");
    if (std::regex_search(line, match, constRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::GO_CONSTANT, filePath, lineNumber, line));
    }
    
    // Package declaration: package packageName
    std::regex packageRegex(R"(\bpackage\s+(\w+))");
    if (std::regex_search(line, match, packageRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::GO_PACKAGE, filePath, lineNumber, line));
    }
    
    // Import statements: import "package" or import alias "package"
    std::regex importRegex(R"(\bimport\s+(?:(\w+)\s+)?\"([^\"]+)\")");
    if (std::regex_search(line, match, importRegex)) {
        std::string name = match[1].str().empty() ? match[2].str() : match[1].str();
        if (!name.empty()) {
            addSymbol(Symbol(name, SymbolType::GO_IMPORT, filePath, lineNumber, line));
        }
    }
    
    // Short variable declarations: varName := value
    std::regex shortVarRegex(R"(^\s*(\w+)\s*:=)");
    if (std::regex_search(line, match, shortVarRegex)) {
        std::string name = match[1].str();
        // Filter out common keywords
        if (name != "if" && name != "for" && name != "switch" && name != "select" && 
            name != "range" && name != "go" && name != "defer") {
            addSymbol(Symbol(name, SymbolType::GO_VARIABLE, filePath, lineNumber, line));
        }
    }
}

void SymbolIndex::parsePython(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Function definitions: def function_name(
    std::regex functionRegex(R"(\bdef\s+(\w+)\s*\()");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::PY_FUNCTION, filePath, lineNumber, line));
    }
    
    // Class definitions: class ClassName
    std::regex classRegex(R"(\bclass\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::PY_CLASS, filePath, lineNumber, line));
    }
    
    // Variable assignments: variable_name = 
    std::regex variableRegex(R"(^(\w+)\s*=\s*)");
    if (std::regex_search(line, match, variableRegex)) {
        std::string name = match[1].str();
        // Filter out common keywords and imports
        if (name != "import" && name != "from" && name != "if" && name != "for" && 
            name != "while" && name != "try" && name != "except" && name != "with") {
            addSymbol(Symbol(name, SymbolType::PY_VARIABLE, filePath, lineNumber, line));
        }
    }
    
    // Import statements: import module or from module import name
    std::regex importRegex(R"(\bimport\s+(\w+))");
    if (std::regex_search(line, match, importRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::PY_IMPORT, filePath, lineNumber, line));
    }
    
    // From imports: from module import name
    std::regex fromImportRegex(R"(\bfrom\s+\w+\s+import\s+(\w+))");
    if (std::regex_search(line, match, fromImportRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::PY_FROM_IMPORT, filePath, lineNumber, line));
    }
    
    // Decorators: @decorator_name
    std::regex decoratorRegex(R"(@(\w+))");
    if (std::regex_search(line, match, decoratorRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::PY_DECORATOR, filePath, lineNumber, line));
    }
    
    // Lambda functions: variable = lambda
    std::regex lambdaRegex(R"((\w+)\s*=\s*lambda)");
    if (std::regex_search(line, match, lambdaRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::PY_LAMBDA, filePath, lineNumber, line));
    }
}

void SymbolIndex::parseTypeScriptJavaScript(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Function declarations: function name() or async function name()
    std::regex functionRegex(R"(\b(?:async\s+)?function\s+(\w+)\s*\()");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_FUNCTION, filePath, lineNumber, line));
    }
    
    // Arrow functions: const name = () => or const name = async () =>
    std::regex arrowFunctionRegex(R"(\b(?:const|let|var)\s+(\w+)\s*=\s*(?:async\s+)?\([^)]*\)\s*=>)");
    if (std::regex_search(line, match, arrowFunctionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_ARROW_FUNCTION, filePath, lineNumber, line));
    }
    
    // Class declarations: class ClassName
    std::regex classRegex(R"(\bclass\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_CLASS, filePath, lineNumber, line));
    }
    
    // Interface declarations: interface InterfaceName
    std::regex interfaceRegex(R"(\binterface\s+(\w+))");
    if (std::regex_search(line, match, interfaceRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_INTERFACE, filePath, lineNumber, line));
    }
    
    // Type declarations: type TypeName =
    std::regex typeRegex(R"(\btype\s+(\w+)\s*=)");
    if (std::regex_search(line, match, typeRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_TYPE, filePath, lineNumber, line));
    }
    
    // Const declarations: const varName
    std::regex constRegex(R"(\bconst\s+(\w+)\s*[=:])");
    if (std::regex_search(line, match, constRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_CONST, filePath, lineNumber, line));
    }
    
    // Let declarations: let varName
    std::regex letRegex(R"(\blet\s+(\w+)\s*[=:])");
    if (std::regex_search(line, match, letRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_LET, filePath, lineNumber, line));
    }
    
    // Var declarations: var varName
    std::regex varRegex(R"(\bvar\s+(\w+)\s*[=:])");
    if (std::regex_search(line, match, varRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_VAR, filePath, lineNumber, line));
    }
    
    // Import statements: import { name } from or import name from
    std::regex importRegex(R"(\bimport\s+(?:\{[^}]*(\w+)[^}]*\}|(\w+))\s+from)");
    if (std::regex_search(line, match, importRegex)) {
        std::string name = match[1].str().empty() ? match[2].str() : match[1].str();
        if (!name.empty()) {
            addSymbol(Symbol(name, SymbolType::JS_IMPORT, filePath, lineNumber, line));
        }
    }
    
    // Export statements: export const name or export function name
    std::regex exportRegex(R"(\bexport\s+(?:const|let|var|function|class|interface|type)\s+(\w+))");
    if (std::regex_search(line, match, exportRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::JS_EXPORT, filePath, lineNumber, line));
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

void SymbolIndex::parsePlainText(const std::string& line, const std::string& filePath, int lineNumber) {
    // Skip empty lines
    if (line.empty() || std::all_of(line.begin(), line.end(), ::isspace)) {
        return;
    }
    
    std::string trimmed = line;
    // Remove leading and trailing whitespace
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(),
                  [](unsigned char ch) { return !std::isspace(ch); }));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
                  [](unsigned char ch) { return !std::isspace(ch); }).base(), trimmed.end());
    
    if (trimmed.empty()) return;
    
    // Check for TODO, NOTE, FIXME annotations
    std::smatch match;
    std::regex todoRegex(R"((?:TODO|FIXME|NOTE|HACK|BUG|WARNING)[\s:]+(.+))");
    if (std::regex_search(trimmed, match, todoRegex)) {
        std::string content = match[1].str();
        SymbolType type = SymbolType::TXT_TODO;
        if (trimmed.find("FIXME") != std::string::npos) type = SymbolType::TXT_FIXME;
        else if (trimmed.find("NOTE") != std::string::npos) type = SymbolType::TXT_NOTE;
        
        addSymbol(Symbol(content, type, filePath, lineNumber, trimmed));
        return;
    }
    
    // Check for URLs
    std::regex urlRegex(R"(https?://[^\s]+)");
    if (std::regex_search(trimmed, match, urlRegex)) {
        std::string url = match[0].str();
        addSymbol(Symbol(url, SymbolType::TXT_URL, filePath, lineNumber, trimmed));
    }
    
    // Check for email addresses
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (std::regex_search(trimmed, match, emailRegex)) {
        std::string email = match[0].str();
        addSymbol(Symbol(email, SymbolType::TXT_EMAIL, filePath, lineNumber, trimmed));
    }
    
    // Check for headers and subheaders
    if (isHeader(trimmed)) {
        addSymbol(Symbol(trimmed, SymbolType::TXT_HEADER, filePath, lineNumber, line));
        return;
    }
    
    if (isSubHeader(trimmed)) {
        addSymbol(Symbol(trimmed, SymbolType::TXT_SUBHEADER, filePath, lineNumber, line));
        return;
    }
    
    // Check for significant lines (longer content that might be important)
    if (isSignificantLine(trimmed)) {
        // Extract first few words as the symbol name
        std::istringstream iss(trimmed);
        std::string firstWords;
        std::string word;
        int wordCount = 0;
        while (iss >> word && wordCount < 5) {
            if (!firstWords.empty()) firstWords += " ";
            firstWords += word;
            wordCount++;
        }
        if (trimmed.length() > firstWords.length()) {
            firstWords += "...";
        }
        
        addSymbol(Symbol(firstWords, SymbolType::TXT_LINE, filePath, lineNumber, trimmed));
    }
    
    // Extract important words (capitalized words, longer words)
    std::regex wordRegex(R"(\b[A-Z][a-zA-Z]{3,}\b|\b[a-zA-Z]{6,}\b)");
    std::sregex_iterator wordsBegin(trimmed.begin(), trimmed.end(), wordRegex);
    std::sregex_iterator wordsEnd;
    
    for (std::sregex_iterator i = wordsBegin; i != wordsEnd; ++i) {
        std::string word = (*i).str();
        // Filter out common words
        if (word != "this" && word != "that" && word != "with" && word != "from" && 
            word != "they" && word != "have" && word != "will" && word != "been" &&
            word != "were" && word != "said" && word != "each" && word != "which" &&
            word != "their" && word != "time" && word != "would" && word != "there") {
            addSymbol(Symbol(word, SymbolType::TXT_WORD, filePath, lineNumber, trimmed));
        }
    }
}

// Text parsing helper methods
bool SymbolIndex::isHeader(const std::string& line) const {
    // Check for common header patterns
    if (line.length() < 3) return false;
    
    // All caps line (at least 3 chars)
    if (std::all_of(line.begin(), line.end(), [](char c) { 
        return std::isupper(c) || std::isspace(c) || std::ispunct(c); 
    }) && std::any_of(line.begin(), line.end(), ::isalpha)) {
        return true;
    }
    
    // Line followed by === or --- (we can't check next line here, but we can check if it ends with these)
    if (line.find("===") != std::string::npos || line.find("---") != std::string::npos) {
        return true;
    }
    
    // Markdown style headers
    if (line[0] == '#') {
        return true;
    }
    
    // Chapter/Section patterns
    std::regex headerRegex(R"(^(?:Chapter|Section|Part|Book)\s+\d+|^\d+\.\s+[A-Z])");
    return std::regex_search(line, headerRegex);
}

bool SymbolIndex::isSubHeader(const std::string& line) const {
    if (line.length() < 3) return false;
    
    // Numbered sections like "1.1 Introduction" or "A.1 Overview"
    std::regex subHeaderRegex(R"(^\d+\.\d+\s+\w+|^[A-Z]\.\d+\s+\w+)");
    if (std::regex_search(line, subHeaderRegex)) {
        return true;
    }
    
    // Lines that start with common subheader words
    std::regex subHeaderWords(R"(^(?:Introduction|Overview|Summary|Conclusion|Background|Method|Results|Discussion|Abstract)[\s:]?)");
    return std::regex_search(line, subHeaderWords);
}

bool SymbolIndex::isSignificantLine(const std::string& line) const {
    // Lines that are likely to be important content
    if (line.length() < 10 || line.length() > 200) return false;
    
    // Lines ending with punctuation (sentences)
    if (line.back() == '.' || line.back() == '!' || line.back() == '?') {
        return true;
    }
    
    // Lines that start with bullet points or numbers
    std::regex listRegex(R"(^[\s]*(?:\*|\-|\+|\d+\.)\s+.+)");
    if (std::regex_search(line, listRegex)) {
        return true;
    }
    
    // Lines that contain important keywords
    std::regex importantRegex(R"(\b(?:important|note|warning|error|success|failure|critical|urgent|required|mandatory|optional)\b)");
    return std::regex_search(line, importantRegex);
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
        // C++ symbols
        case SymbolType::FUNCTION: return "function";
        case SymbolType::CLASS: return "class";
        case SymbolType::STRUCT: return "struct";
        case SymbolType::VARIABLE: return "variable";
        case SymbolType::ENUM: return "enum";
        case SymbolType::TYPEDEF: return "typedef";
        case SymbolType::MACRO: return "macro";
        case SymbolType::NAMESPACE: return "namespace";
        
        // TypeScript/JavaScript symbols
        case SymbolType::JS_FUNCTION: return "js-function";
        case SymbolType::JS_ARROW_FUNCTION: return "arrow-func";
        case SymbolType::JS_CLASS: return "js-class";
        case SymbolType::JS_INTERFACE: return "interface";
        case SymbolType::JS_TYPE: return "type";
        case SymbolType::JS_CONST: return "const";
        case SymbolType::JS_LET: return "let";
        case SymbolType::JS_VAR: return "var";
        case SymbolType::JS_IMPORT: return "import";
        case SymbolType::JS_EXPORT: return "export";
        case SymbolType::JS_MODULE: return "module";
        
        // Python symbols
        case SymbolType::PY_FUNCTION: return "py-function";
        case SymbolType::PY_CLASS: return "py-class";
        case SymbolType::PY_METHOD: return "py-method";
        case SymbolType::PY_VARIABLE: return "py-variable";
        case SymbolType::PY_IMPORT: return "py-import";
        case SymbolType::PY_FROM_IMPORT: return "py-from-import";
        case SymbolType::PY_DECORATOR: return "py-decorator";
        case SymbolType::PY_LAMBDA: return "py-lambda";
        
        // Go symbols
        case SymbolType::GO_FUNCTION: return "go_function";
        case SymbolType::GO_METHOD: return "go_method";
        case SymbolType::GO_STRUCT: return "go_struct";
        case SymbolType::GO_INTERFACE: return "go_interface";
        case SymbolType::GO_TYPE: return "go_type";
        case SymbolType::GO_VARIABLE: return "go_variable";
        case SymbolType::GO_CONSTANT: return "go_constant";
        case SymbolType::GO_PACKAGE: return "go_package";
        case SymbolType::GO_IMPORT: return "go_import";
        
        // Swift symbols
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
        
        // Text file symbols
        case SymbolType::TXT_HEADER: return "header";
        case SymbolType::TXT_SUBHEADER: return "subheader";
        case SymbolType::TXT_URL: return "url";
        case SymbolType::TXT_EMAIL: return "email";
        case SymbolType::TXT_TODO: return "todo";
        case SymbolType::TXT_NOTE: return "note";
        case SymbolType::TXT_FIXME: return "fixme";
        case SymbolType::TXT_LINE: return "line";
        case SymbolType::TXT_WORD: return "word";
        
        case SymbolType::UNKNOWN: return "unknown";
        default: return "unknown";
    }
} 

std::string SymbolIndex::getLanguageFromPath(const std::string& filePath) const {
    if (isPlainText(filePath)) return "Text";
    if (isSwift(filePath)) return "Swift";
    if (isGo(filePath)) return "Go";
    if (isPython(filePath)) return "Python";
    if (isTypeScriptOrJavaScript(filePath)) {
        std::string ext = filePath.substr(filePath.find_last_of('.'));
        return (ext == ".ts" || ext == ".tsx") ? "TypeScript" : "JavaScript";
    }
    
    std::string ext = filePath.substr(filePath.find_last_of('.'));
    if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc" || ext == ".cxx") {
        return "C++";
    }
    
    return "Unknown";
} 

void SymbolIndex::parseSwift(const std::string& line, const std::string& filePath, int lineNumber) {
    // Swift function definitions
    std::regex funcRegex(R"(\s*func\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\()");
    std::smatch match;
    
    // Functions
    if (std::regex_search(line, match, funcRegex)) {
        std::string funcName = match[1].str();
        addSymbol(Symbol(funcName, SymbolType::SWIFT_FUNCTION, filePath, lineNumber, line));
        return;
    }
    
    // Class definitions
    std::regex classRegex(R"(\s*class\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(line, match, classRegex)) {
        std::string className = match[1].str();
        addSymbol(Symbol(className, SymbolType::SWIFT_CLASS, filePath, lineNumber, line));
        return;
    }
    
    // Struct definitions
    std::regex structRegex(R"(\s*struct\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(line, match, structRegex)) {
        std::string structName = match[1].str();
        addSymbol(Symbol(structName, SymbolType::SWIFT_STRUCT, filePath, lineNumber, line));
        return;
    }
    
    // Protocol definitions
    std::regex protocolRegex(R"(\s*protocol\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(line, match, protocolRegex)) {
        std::string protocolName = match[1].str();
        addSymbol(Symbol(protocolName, SymbolType::SWIFT_PROTOCOL, filePath, lineNumber, line));
        return;
    }
    
    // Enum definitions
    std::regex enumRegex(R"(\s*enum\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(line, match, enumRegex)) {
        std::string enumName = match[1].str();
        addSymbol(Symbol(enumName, SymbolType::SWIFT_ENUM, filePath, lineNumber, line));
        return;
    }
    
    // Extension definitions
    std::regex extensionRegex(R"(\s*extension\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(line, match, extensionRegex)) {
        std::string extensionName = match[1].str();
        addSymbol(Symbol(extensionName, SymbolType::SWIFT_EXTENSION, filePath, lineNumber, line));
        return;
    }
    
    // Variable declarations (var)
    std::regex varRegex(R"(\s*var\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*[:\=])");
    if (std::regex_search(line, match, varRegex)) {
        std::string varName = match[1].str();
        addSymbol(Symbol(varName, SymbolType::SWIFT_VARIABLE, filePath, lineNumber, line));
        return;
    }
    
    // Constant declarations (let)
    std::regex letRegex(R"(\s*let\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*[:\=])");
    if (std::regex_search(line, match, letRegex)) {
        std::string letName = match[1].str();
        addSymbol(Symbol(letName, SymbolType::SWIFT_CONSTANT, filePath, lineNumber, line));
        return;
    }
    
    // Computed properties (with { get/set })
    std::regex propertyRegex(R"(\s*var\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*[^{]*\s*\{)");
    if (std::regex_search(line, match, propertyRegex)) {
        std::string propName = match[1].str();
        addSymbol(Symbol(propName, SymbolType::SWIFT_PROPERTY, filePath, lineNumber, line));
        return;
    }
    
    // Initializers
    std::regex initRegex(R"(\s*init\s*\()");
    if (std::regex_search(line, match, initRegex)) {
        addSymbol(Symbol("init", SymbolType::SWIFT_INITIALIZER, filePath, lineNumber, line));
        return;
    }
    
    // Subscripts
    std::regex subscriptRegex(R"(\s*subscript\s*\()");
    if (std::regex_search(line, match, subscriptRegex)) {
        addSymbol(Symbol("subscript", SymbolType::SWIFT_SUBSCRIPT, filePath, lineNumber, line));
        return;
    }
    
    // Import statements
    std::regex importRegex(R"(\s*import\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(line, match, importRegex)) {
        std::string importName = match[1].str();
        addSymbol(Symbol(importName, SymbolType::SWIFT_IMPORT, filePath, lineNumber, line));
        return;
    }
} 

void SymbolIndex::parseKotlin(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    // Match top-level functions: fun functionName(
    std::regex functionRegex(R"(\bfun\s+(\w+)\s*\()");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
    }
    // Match classes: class ClassName
    std::regex classRegex(R"(\bclass\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    // Match objects: object ObjectName
    std::regex objectRegex(R"(\bobject\s+(\w+))");
    if (std::regex_search(line, match, objectRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
}

void SymbolIndex::parseJava(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Match classes: public class ClassName, class ClassName
    std::regex classRegex(R"(\b(?:public\s+|private\s+|protected\s+)?(?:abstract\s+|final\s+)?class\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match interfaces: public interface InterfaceName, interface InterfaceName
    std::regex interfaceRegex(R"(\b(?:public\s+)?interface\s+(\w+))");
    if (std::regex_search(line, match, interfaceRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match methods: public void methodName(, private int methodName(
    std::regex methodRegex(R"(\b(?:public\s+|private\s+|protected\s+)?(?:static\s+)?(?:final\s+)?(?:\w+\s+)?(\w+)\s*\()");
    if (std::regex_search(line, match, methodRegex)) {
        std::string name = match[1].str();
        // Skip constructor calls and common keywords
        if (name != "if" && name != "while" && name != "for" && name != "switch" && name != "catch") {
            addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
        }
    }
}

void SymbolIndex::parsePHP(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Match functions: function functionName(
    std::regex functionRegex(R"(\bfunction\s+(\w+)\s*\()");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
    }
    
    // Match classes: class ClassName
    std::regex classRegex(R"(\bclass\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match variables: $variableName
    std::regex variableRegex(R"(\$(\w+))");
    if (std::regex_search(line, match, variableRegex)) {
        std::string name = "$" + match[1].str();
        addSymbol(Symbol(name, SymbolType::VARIABLE, filePath, lineNumber, line));
    }
}

void SymbolIndex::parseBash(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Match functions: function_name() or function function_name()
    std::regex functionRegex(R"((?:function\s+)?(\w+)\s*\(\s*\))");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
    }
    
    // Match variables: VARIABLE_NAME=
    std::regex variableRegex(R"(^([A-Z_][A-Z0-9_]*)\s*=)");
    if (std::regex_search(line, match, variableRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::VARIABLE, filePath, lineNumber, line));
    }
}

void SymbolIndex::parseRuby(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Match methods: def method_name
    std::regex methodRegex(R"(\bdef\s+(\w+))");
    if (std::regex_search(line, match, methodRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
    }
    
    // Match classes: class ClassName
    std::regex classRegex(R"(\bclass\s+(\w+))");
    if (std::regex_search(line, match, classRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match modules: module ModuleName
    std::regex moduleRegex(R"(\bmodule\s+(\w+))");
    if (std::regex_search(line, match, moduleRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
}

void SymbolIndex::parseRust(const std::string& line, const std::string& filePath, int lineNumber) {
    std::smatch match;
    
    // Match functions: fn function_name(, pub fn function_name(
    std::regex functionRegex(R"(\b(?:pub\s+)?fn\s+(\w+)\s*\()");
    if (std::regex_search(line, match, functionRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::FUNCTION, filePath, lineNumber, line));
    }
    
    // Match structs: struct StructName, pub struct StructName
    std::regex structRegex(R"(\b(?:pub\s+)?struct\s+(\w+))");
    if (std::regex_search(line, match, structRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match enums: enum EnumName, pub enum EnumName
    std::regex enumRegex(R"(\b(?:pub\s+)?enum\s+(\w+))");
    if (std::regex_search(line, match, enumRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match traits: trait TraitName, pub trait TraitName
    std::regex traitRegex(R"(\b(?:pub\s+)?trait\s+(\w+))");
    if (std::regex_search(line, match, traitRegex)) {
        std::string name = match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
    
    // Match impls: impl StructName, impl TraitName for StructName
    std::regex implRegex(R"(\bimpl\s+(?:\w+\s+for\s+)?(\w+))");
    if (std::regex_search(line, match, implRegex)) {
        std::string name = "impl " + match[1].str();
        addSymbol(Symbol(name, SymbolType::CLASS, filePath, lineNumber, line));
    }
} 