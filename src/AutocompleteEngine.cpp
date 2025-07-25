#include "AutocompleteEngine.hpp"
#include "PerformanceLogger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cctype>
#include <filesystem>

AutocompleteEngine::AutocompleteEngine()
    : m_trieRoot(std::make_unique<TrieNode>())
    , m_fuzzyThreshold(0.3)
    , m_prefixWeight(1.0)
    , m_fuzzyWeight(0.7)
    , m_substringWeight(0.5)
    , m_logger(nullptr)
{
    // Default type boosts
    m_typeBoosts[SymbolType::FUNCTION] = 1.2;
    m_typeBoosts[SymbolType::CLASS] = 1.1;
    m_typeBoosts[SymbolType::JS_FUNCTION] = 1.2;
    m_typeBoosts[SymbolType::JS_CLASS] = 1.1;
    m_typeBoosts[SymbolType::PY_FUNCTION] = 1.2;
    m_typeBoosts[SymbolType::PY_CLASS] = 1.1;
    m_typeBoosts[SymbolType::GO_FUNCTION] = 1.2;
    m_typeBoosts[SymbolType::GO_STRUCT] = 1.1;
}

void AutocompleteEngine::buildIndex(const std::vector<Symbol>& symbols) {
    clear();
    
    if (m_logger) {
        m_logger->logFileStart("autocomplete-index-build");
    }
    
    m_symbols = symbols;
    
    // Build trie and symbol map
    for (size_t i = 0; i < symbols.size(); ++i) {
        const Symbol& symbol = symbols[i];
        insertIntoTrie(symbol.name, &symbol);
        m_symbolMap[toLowerCase(symbol.name)].push_back(i);
    }
    
    if (m_logger) {
        m_logger->logFileEnd("autocomplete-index-build", symbols.size(), "autocomplete");
    }
    
    std::cout << "🔍 Autocomplete index built: " << symbols.size() 
              << " symbols indexed for fast completion\n";
}

void AutocompleteEngine::addSymbol(const Symbol& symbol) {
    m_symbols.push_back(symbol);
    insertIntoTrie(symbol.name, &m_symbols.back());
    m_symbolMap[toLowerCase(symbol.name)].push_back(m_symbols.size() - 1);
}

void AutocompleteEngine::clear() {
    m_trieRoot = std::make_unique<TrieNode>();
    m_symbols.clear();
    m_symbolMap.clear();
}

std::vector<AutocompleteResult> AutocompleteEngine::getCompletions(const std::string& query, 
                                                                   size_t maxResults) const {
    if (query.empty()) {
        return {};
    }
    
    std::vector<AutocompleteResult> results;
    
    // 1. Get prefix matches (highest priority)
    auto prefixResults = getPrefixMatches(query, maxResults / 2);
    for (auto& result : prefixResults) {
        result.score *= m_prefixWeight;
        results.push_back(result);
    }
    
    // 2. Get substring matches (medium priority)
    auto substringResults = getSubstringMatches(query, maxResults / 3);
    for (auto& result : substringResults) {
        result.score *= m_substringWeight;
        results.push_back(result);
    }
    
    // 3. Get fuzzy matches (lower priority but more flexible)
    auto fuzzyResults = getFuzzyMatches(query, maxResults / 2);
    for (auto& result : fuzzyResults) {
        result.score *= m_fuzzyWeight;
        results.push_back(result);
    }
    
    // Remove duplicates and sort by score
    deduplicateResults(results);
    sortAndLimitResults(results, maxResults);
    
    return results;
}

std::vector<AutocompleteResult> AutocompleteEngine::getPrefixMatches(const std::string& prefix, 
                                                                     size_t maxResults) const {
    std::vector<AutocompleteResult> results;
    
    // Navigate to the prefix node
    TrieNode* current = m_trieRoot.get();
    std::string lowerPrefix = toLowerCase(prefix);
    
    for (char c : lowerPrefix) {
        if (current->children.find(c) == current->children.end()) {
            return results; // No matches
        }
        current = current->children[c].get();
    }
    
    // Collect all symbols with this prefix
    collectTrieMatches(current, prefix, results, maxResults);
    
    return results;
}

std::vector<AutocompleteResult> AutocompleteEngine::getFuzzyMatches(const std::string& query, 
                                                                    size_t maxResults, 
                                                                    double minScore) const {
    std::vector<AutocompleteResult> results;
    
    for (const Symbol& symbol : m_symbols) {
        double score = calculateFuzzyScore(symbol.name, query);
        
        if (score >= minScore) {
            AutocompleteResult result(symbol.name, symbol.type, symbol.file, 
                                    symbol.line, score, symbol.context, "fuzzy");
            result.score = applyContextBoosts(score, symbol);
            results.push_back(result);
        }
    }
    
    sortAndLimitResults(results, maxResults);
    return results;
}

std::vector<AutocompleteResult> AutocompleteEngine::getSubstringMatches(const std::string& substring, 
                                                                        size_t maxResults) const {
    std::vector<AutocompleteResult> results;
    std::string lowerSubstring = toLowerCase(substring);
    
    for (const Symbol& symbol : m_symbols) {
        std::string lowerSymbol = toLowerCase(symbol.name);
        
        if (lowerSymbol.find(lowerSubstring) != std::string::npos) {
            double score = calculateBaseScore(symbol.name, substring, "substring");
            AutocompleteResult result(symbol.name, symbol.type, symbol.file, 
                                    symbol.line, score, symbol.context, "substring");
            result.score = applyContextBoosts(score, symbol);
            results.push_back(result);
        }
    }
    
    sortAndLimitResults(results, maxResults);
    return results;
}

void AutocompleteEngine::insertIntoTrie(const std::string& word, const Symbol* symbol) {
    TrieNode* current = m_trieRoot.get();
    std::string lowerWord = toLowerCase(word);
    
    for (char c : lowerWord) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = std::make_unique<TrieNode>();
        }
        current = current->children[c].get();
        current->symbols.push_back(symbol);
    }
    
    current->isEndOfWord = true;
}

void AutocompleteEngine::collectTrieMatches(TrieNode* node, const std::string& prefix, 
                                           std::vector<AutocompleteResult>& results, 
                                           size_t maxResults) const {
    if (results.size() >= maxResults) {
        return;
    }
    
    // Add symbols at this node
    for (const Symbol* symbol : node->symbols) {
        if (results.size() >= maxResults) break;
        
        double score = calculateBaseScore(symbol->name, prefix, "prefix");
        AutocompleteResult result(symbol->name, symbol->type, symbol->file, 
                                symbol->line, score, symbol->context, "prefix");
        result.score = applyContextBoosts(score, *symbol);
        results.push_back(result);
    }
    
    // Recursively collect from children
    for (const auto& [ch, child] : node->children) {
        if (results.size() >= maxResults) break;
        collectTrieMatches(child.get(), prefix, results, maxResults);
    }
}

double AutocompleteEngine::calculateFuzzyScore(const std::string& target, const std::string& query) const {
    if (target.empty() || query.empty()) {
        return 0.0;
    }
    
    // Combine multiple fuzzy matching algorithms
    double levenshtein = calculateLevenshteinScore(target, query);
    double jaroWinkler = calculateJaroWinklerScore(target, query);
    double lcs = calculateLCSScore(target, query);
    
    // Weighted combination
    return (levenshtein * 0.4 + jaroWinkler * 0.4 + lcs * 0.2);
}

double AutocompleteEngine::calculateLevenshteinScore(const std::string& s1, const std::string& s2) const {
    std::string lower1 = toLowerCase(s1);
    std::string lower2 = toLowerCase(s2);
    
    size_t len1 = lower1.length();
    size_t len2 = lower2.length();
    
    if (len1 == 0) return len2 == 0 ? 1.0 : 0.0;
    if (len2 == 0) return 0.0;
    
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
    
    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (lower1[i-1] == lower2[j-1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i-1][j] + 1,      // deletion
                dp[i][j-1] + 1,      // insertion
                dp[i-1][j-1] + cost  // substitution
            });
        }
    }
    
    int distance = dp[len1][len2];
    int maxLen = std::max(len1, len2);
    return 1.0 - (static_cast<double>(distance) / maxLen);
}

double AutocompleteEngine::calculateJaroWinklerScore(const std::string& s1, const std::string& s2) const {
    std::string lower1 = toLowerCase(s1);
    std::string lower2 = toLowerCase(s2);
    
    if (lower1 == lower2) return 1.0;
    
    size_t len1 = lower1.length();
    size_t len2 = lower2.length();
    
    if (len1 == 0 || len2 == 0) return 0.0;
    
    int matchWindow = std::max(len1, len2) / 2 - 1;
    if (matchWindow < 0) matchWindow = 0;
    
    std::vector<bool> s1Matches(len1, false);
    std::vector<bool> s2Matches(len2, false);
    
    int matches = 0;
    int transpositions = 0;
    
    // Find matches
    for (size_t i = 0; i < len1; ++i) {
        int start = std::max(0, static_cast<int>(i) - matchWindow);
        int end = std::min(static_cast<int>(i) + matchWindow + 1, static_cast<int>(len2));
        
        for (int j = start; j < end; ++j) {
            if (s2Matches[j] || lower1[i] != lower2[j]) continue;
            s1Matches[i] = s2Matches[j] = true;
            matches++;
            break;
        }
    }
    
    if (matches == 0) return 0.0;
    
    // Count transpositions
    int k = 0;
    for (size_t i = 0; i < len1; ++i) {
        if (!s1Matches[i]) continue;
        while (!s2Matches[k]) k++;
        if (lower1[i] != lower2[k]) transpositions++;
        k++;
    }
    
    double jaro = (static_cast<double>(matches) / len1 + 
                   static_cast<double>(matches) / len2 + 
                   static_cast<double>(matches - transpositions/2) / matches) / 3.0;
    
    // Winkler prefix bonus
    int prefix = 0;
    for (size_t i = 0; i < std::min(len1, len2) && i < 4; ++i) {
        if (lower1[i] == lower2[i]) prefix++;
        else break;
    }
    
    return jaro + (0.1 * prefix * (1.0 - jaro));
}

double AutocompleteEngine::calculateLCSScore(const std::string& s1, const std::string& s2) const {
    std::string lower1 = toLowerCase(s1);
    std::string lower2 = toLowerCase(s2);
    
    size_t len1 = lower1.length();
    size_t len2 = lower2.length();
    
    if (len1 == 0 || len2 == 0) return 0.0;
    
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            if (lower1[i-1] == lower2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    
    int lcs = dp[len1][len2];
    return static_cast<double>(lcs) / std::max(len1, len2);
}

double AutocompleteEngine::calculateBaseScore(const std::string& symbol, const std::string& query, 
                                             const std::string& matchType) const {
    if (symbol.empty() || query.empty()) return 0.0;
    
    std::string lowerSymbol = toLowerCase(symbol);
    std::string lowerQuery = toLowerCase(query);
    
    if (matchType == "prefix") {
        // Exact prefix gets highest score
        if (lowerSymbol.substr(0, lowerQuery.length()) == lowerQuery) {
            return 1.0 - (static_cast<double>(symbol.length() - query.length()) / symbol.length() * 0.1);
        }
    } else if (matchType == "substring") {
        size_t pos = lowerSymbol.find(lowerQuery);
        if (pos != std::string::npos) {
            // Earlier position gets higher score
            double positionBonus = 1.0 - (static_cast<double>(pos) / symbol.length() * 0.3);
            double lengthRatio = static_cast<double>(query.length()) / symbol.length();
            return positionBonus * lengthRatio;
        }
    }
    
    return 0.5; // Default score
}

double AutocompleteEngine::applyContextBoosts(double baseScore, const Symbol& symbol) const {
    double score = baseScore;
    
    // Apply type boost
    score = applyTypeBoost(score, symbol.type);
    
    // Apply frequency boost
    score = applyFrequencyBoost(score, symbol.name);
    
    // Boost shorter names (often more commonly used)
    if (symbol.name.length() <= 5) {
        score *= 1.1;
    } else if (symbol.name.length() >= 20) {
        score *= 0.9;
    }
    
    return std::min(score, 1.0);
}

double AutocompleteEngine::applyTypeBoost(double score, SymbolType type) const {
    auto it = m_typeBoosts.find(type);
    if (it != m_typeBoosts.end()) {
        return score * it->second;
    }
    return score;
}

double AutocompleteEngine::applyFrequencyBoost(double score, const std::string& symbolName) const {
    std::string lowerName = toLowerCase(symbolName);
    auto it = m_symbolMap.find(lowerName);
    
    if (it != m_symbolMap.end() && it->second.size() > 1) {
        // Symbol appears multiple times, slight boost
        return score * 1.05;
    }
    
    return score;
}

void AutocompleteEngine::deduplicateResults(std::vector<AutocompleteResult>& results) const {
    std::set<std::string> seen;
    auto it = results.begin();
    
    while (it != results.end()) {
        std::string key = it->suggestion + "|" + it->file + "|" + std::to_string(it->line);
        if (seen.find(key) != seen.end()) {
            it = results.erase(it);
        } else {
            seen.insert(key);
            ++it;
        }
    }
}

void AutocompleteEngine::sortAndLimitResults(std::vector<AutocompleteResult>& results, 
                                            size_t maxResults) const {
    // Sort by score (descending)
    std::sort(results.begin(), results.end(), std::greater<AutocompleteResult>());
    
    // Limit results
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
}

std::string AutocompleteEngine::toLowerCase(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void AutocompleteEngine::runInteractiveMode(const std::string& prompt) const {
    std::cout << "\n🔍 Interactive Autocomplete Mode\n";
    std::cout << "Type symbols to get suggestions. Use 'exit' or 'quit' to leave.\n\n";
    
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        if (input == "exit" || input == "quit" || input == "q") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        auto results = getCompletions(input, 10);
        
        if (results.empty()) {
            std::cout << "❌ No completions found for '" << input << "'\n\n";
        } else {
            std::cout << "\n✅ Found " << results.size() << " completion(s) for '" << input << "':\n";
            printResultsTable(results);
            std::cout << "\n";
        }
    }
    
    std::cout << "👋 Goodbye!\n";
}

void AutocompleteEngine::printResultsTable(const std::vector<AutocompleteResult>& results) const {
    if (results.empty()) return;
    
    std::cout << "┌─ AUTOCOMPLETE RESULTS ─────────────────────────────────────────────────────┐\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        printResult(results[i], i + 1);
    }
    
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘";
}

void AutocompleteEngine::printResult(const AutocompleteResult& result, size_t index) const {
    std::string filename = AutocompleteUtils::getFileBasename(result.file);
    std::string typeStr = ""; // You'd need to implement symbolTypeToString
    
    std::cout << "│ " << std::setw(2) << index << ". " 
             << std::setw(20) << result.suggestion
             << " | " << std::setw(8) << result.matchType
             << " | " << std::setw(6) << std::fixed << std::setprecision(2) << result.score
             << " | " << std::setw(15) << filename << ":" << result.line << "\n";
}

size_t AutocompleteEngine::getSymbolCount() const {
    return m_symbols.size();
}

size_t AutocompleteEngine::getTrieSize() const {
    return calculateTrieSize(m_trieRoot.get());
}

size_t AutocompleteEngine::calculateTrieSize(TrieNode* node) const {
    if (!node) return 0;
    
    size_t size = 1;
    for (const auto& [ch, child] : node->children) {
        size += calculateTrieSize(child.get());
    }
    return size;
}

void AutocompleteEngine::printStatistics() const {
    std::cout << "\n┌─ AUTOCOMPLETE STATISTICS ──────────────────────────────────────────────────┐\n";
    std::cout << "│ 📊 Total Symbols: " << getSymbolCount() << "\n";
    std::cout << "│ 🌳 Trie Nodes: " << getTrieSize() << "\n";
    std::cout << "│ 🗂️  Unique Names: " << m_symbolMap.size() << "\n";
    std::cout << "│ ⚙️  Fuzzy Threshold: " << m_fuzzyThreshold << "\n";
    std::cout << "│ 🎯 Prefix Weight: " << m_prefixWeight << "\n";
    std::cout << "│ 🔍 Fuzzy Weight: " << m_fuzzyWeight << "\n";
    std::cout << "│ 📝 Substring Weight: " << m_substringWeight << "\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n";
}

void AutocompleteEngine::setPerformanceLogger(PerformanceLogger* logger) {
    m_logger = logger;
}

void AutocompleteEngine::setFuzzyThreshold(double threshold) {
    m_fuzzyThreshold = threshold;
}

void AutocompleteEngine::setPrefixWeight(double weight) {
    m_prefixWeight = weight;
}

void AutocompleteEngine::setFuzzyWeight(double weight) {
    m_fuzzyWeight = weight;
}

void AutocompleteEngine::setSubstringWeight(double weight) {
    m_substringWeight = weight;
}

void AutocompleteEngine::setTypeBoosts(const std::unordered_map<SymbolType, double>& boosts) {
    m_typeBoosts = boosts;
}

// AutocompleteUtils implementation
std::string AutocompleteUtils::getCommonPrefix(const std::vector<std::string>& strings) {
    if (strings.empty()) return "";
    
    std::string prefix = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        std::string current = strings[i];
        size_t j = 0;
        while (j < prefix.length() && j < current.length() && prefix[j] == current[j]) {
            j++;
        }
        prefix = prefix.substr(0, j);
        if (prefix.empty()) break;
    }
    
    return prefix;
}

std::string AutocompleteUtils::getFileBasename(const std::string& filePath) {
    try {
        return std::filesystem::path(filePath).filename().string();
    } catch (...) {
        size_t pos = filePath.find_last_of('/');
        if (pos != std::string::npos) {
            return filePath.substr(pos + 1);
        }
        return filePath;
    }
}

std::vector<std::string> AutocompleteUtils::expandCamelCase(const std::string& str) {
    std::vector<std::string> parts;
    std::string current;
    
    for (char c : str) {
        if (std::isupper(c) && !current.empty()) {
            parts.push_back(current);
            current = c;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
} 