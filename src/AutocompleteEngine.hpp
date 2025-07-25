#ifndef AUTOCOMPLETEENGINE_HPP
#define AUTOCOMPLETEENGINE_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <set>
#include "Symbol.hpp"

// Forward declarations
class PerformanceLogger;

struct AutocompleteResult {
    std::string suggestion;
    SymbolType type;
    std::string file;
    int line;
    double score;
    std::string context;
    std::string matchType; // "prefix", "fuzzy", "substring"
    
    // Default constructor
    AutocompleteResult() 
        : type(SymbolType::UNKNOWN), line(0), score(0.0) {}
    
    AutocompleteResult(const std::string& sugg, SymbolType t, const std::string& f, 
                      int l, double sc, const std::string& ctx, const std::string& mt)
        : suggestion(sugg), type(t), file(f), line(l), score(sc), context(ctx), matchType(mt) {}
    
    // For sorting by score (higher is better)
    bool operator>(const AutocompleteResult& other) const {
        return score > other.score;
    }
};

// Trie node for efficient prefix matching
struct TrieNode {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    std::vector<const Symbol*> symbols; // Symbols ending at this node
    bool isEndOfWord;
    
    TrieNode() : isEndOfWord(false) {}
};

class AutocompleteEngine {
public:
    AutocompleteEngine();
    ~AutocompleteEngine() = default;
    
    // Index management
    void buildIndex(const std::vector<Symbol>& symbols);
    void addSymbol(const Symbol& symbol);
    void clear();
    
    // Core autocomplete functionality
    std::vector<AutocompleteResult> getCompletions(const std::string& query, 
                                                   size_t maxResults = 20) const;
    
    // Specialized search methods
    std::vector<AutocompleteResult> getPrefixMatches(const std::string& prefix, 
                                                     size_t maxResults = 10) const;
    std::vector<AutocompleteResult> getFuzzyMatches(const std::string& query, 
                                                    size_t maxResults = 10, 
                                                    double minScore = 0.3) const;
    std::vector<AutocompleteResult> getSubstringMatches(const std::string& substring, 
                                                        size_t maxResults = 10) const;
    
    // Configuration
    void setFuzzyThreshold(double threshold);
    void setPrefixWeight(double weight);
    void setFuzzyWeight(double weight);
    void setSubstringWeight(double weight);
    void setTypeBoosts(const std::unordered_map<SymbolType, double>& boosts);
    void setPerformanceLogger(PerformanceLogger* logger);
    
    // Statistics
    size_t getSymbolCount() const;
    size_t getTrieSize() const;
    void printStatistics() const;
    
    // Interactive mode
    void runInteractiveMode(const std::string& prompt = "autocomplete> ") const;
    
private:
    // Trie for prefix matching
    std::unique_ptr<TrieNode> m_trieRoot;
    
    // Symbol storage
    std::vector<Symbol> m_symbols;
    std::unordered_map<std::string, std::vector<size_t>> m_symbolMap; // name -> indices
    
    // Configuration
    double m_fuzzyThreshold;
    double m_prefixWeight;
    double m_fuzzyWeight;
    double m_substringWeight;
    std::unordered_map<SymbolType, double> m_typeBoosts;
    
    // Performance
    PerformanceLogger* m_logger;
    
    // Trie operations
    void insertIntoTrie(const std::string& word, const Symbol* symbol);
    void collectTrieMatches(TrieNode* node, const std::string& prefix, 
                           std::vector<AutocompleteResult>& results, 
                           size_t maxResults) const;
    size_t calculateTrieSize(TrieNode* node) const;
    
    // Fuzzy matching algorithms
    double calculateFuzzyScore(const std::string& target, const std::string& query) const;
    double calculateLevenshteinScore(const std::string& s1, const std::string& s2) const;
    double calculateJaroWinklerScore(const std::string& s1, const std::string& s2) const;
    double calculateLCSScore(const std::string& s1, const std::string& s2) const;
    
    // Scoring and ranking
    double calculateBaseScore(const std::string& symbol, const std::string& query, 
                             const std::string& matchType) const;
    double applyContextBoosts(double baseScore, const Symbol& symbol) const;
    double applyTypeBoost(double score, SymbolType type) const;
    double applyFrequencyBoost(double score, const std::string& symbolName) const;
    
    // Utility functions
    std::string toLowerCase(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& str) const;
    bool isValidSymbolChar(char c) const;
    std::string highlightMatches(const std::string& text, const std::string& query) const;
    
    // Result processing
    void deduplicateResults(std::vector<AutocompleteResult>& results) const;
    void sortAndLimitResults(std::vector<AutocompleteResult>& results, size_t maxResults) const;
    
    // Interactive helpers
    void printResult(const AutocompleteResult& result, size_t index) const;
    void printResultsTable(const std::vector<AutocompleteResult>& results) const;
};

// Utility functions for autocomplete
class AutocompleteUtils {
public:
    static std::string getCommonPrefix(const std::vector<std::string>& strings);
    static std::vector<std::string> expandCamelCase(const std::string& str);
    static double calculateRelevanceScore(const AutocompleteResult& result, 
                                         const std::string& query);
    static std::string formatSymbolSignature(const Symbol& symbol);
    static std::string getFileBasename(const std::string& filePath);
};

#endif // AUTOCOMPLETEENGINE_HPP 