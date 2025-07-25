#ifndef PERFORMANCELOGGER_HPP
#define PERFORMANCELOGGER_HPP

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <fstream>
#include <mutex>
#include <atomic>

struct FileMetrics {
    std::string filePath;
    std::chrono::milliseconds parseTime;
    size_t symbolCount;
    size_t fileSize;
    std::string language;
    std::chrono::system_clock::time_point timestamp;
    
    FileMetrics(const std::string& path, std::chrono::milliseconds time, 
                size_t symbols, size_t size, const std::string& lang)
        : filePath(path), parseTime(time), symbolCount(symbols), 
          fileSize(size), language(lang), timestamp(std::chrono::system_clock::now()) {}
};

struct SessionMetrics {
    std::chrono::system_clock::time_point startTime;
    std::chrono::milliseconds totalTime;
    size_t totalFiles;
    size_t totalSymbols;
    size_t totalBytes;
    std::map<std::string, size_t> languageCounts;
    std::map<std::string, std::chrono::milliseconds> languageTimes;
    
    SessionMetrics() 
        : startTime(std::chrono::system_clock::now())
        , totalTime(std::chrono::milliseconds::zero())
        , totalFiles(0), totalSymbols(0), totalBytes(0) {}
};

class PerformanceLogger {
public:
    PerformanceLogger();
    ~PerformanceLogger();
    
    // Session management
    void startSession(const std::string& operation = "indexing");
    void endSession();
    
    // File-level logging
    void logFileStart(const std::string& filePath);
    void logFileEnd(const std::string& filePath, size_t symbolCount, const std::string& language);
    
    // Real-time metrics
    void logSymbol(const std::string& symbolType);
    void logError(const std::string& filePath, const std::string& error);
    
    // Statistics
    SessionMetrics getCurrentSession() const;
    std::vector<FileMetrics> getFileMetrics() const;
    void printSessionSummary() const;
    void printTopSlowFiles(size_t count = 10) const;
    void printLanguageBreakdown() const;
    
    // Configuration
    void setVerbose(bool verbose);
    void setLogToFile(bool enabled, const std::string& filename = "navix_performance.log");
    void setMinLogTime(std::chrono::milliseconds minTime);
    
    // Live monitoring
    void printLiveStats() const;
    double getFilesPerSecond() const;
    double getSymbolsPerSecond() const;
    
private:
    bool m_verbose;
    bool m_logToFile;
    std::string m_logFilename;
    std::chrono::milliseconds m_minLogTime;
    
    // Session state
    std::atomic<bool> m_sessionActive;
    SessionMetrics m_currentSession;
    std::string m_currentOperation;
    
    // File tracking
    std::map<std::string, std::chrono::steady_clock::time_point> m_fileStartTimes;
    std::vector<FileMetrics> m_fileMetrics;
    
    // Thread safety
    mutable std::mutex m_metricsMutex;
    std::mutex m_fileMutex;
    
    // Counters
    std::atomic<size_t> m_filesProcessed;
    std::atomic<size_t> m_symbolsFound;
    std::atomic<size_t> m_errorsEncountered;
    
    // Internal methods
    void writeToLog(const std::string& message);
    void updateSessionMetrics(const FileMetrics& metrics);
    std::string formatDuration(std::chrono::milliseconds duration) const;
    std::string formatSize(size_t bytes) const;
    std::string getCurrentTimeString() const;
    std::string getLanguageFromPath(const std::string& path) const;
    size_t getFileSize(const std::string& path) const;
};

// RAII helper for automatic file timing
class FileTimer {
public:
    FileTimer(PerformanceLogger& logger, const std::string& filePath)
        : m_logger(logger), m_filePath(filePath) {
        m_logger.logFileStart(filePath);
    }
    
    ~FileTimer() {
        m_logger.logFileEnd(m_filePath, m_symbolCount, m_language);
    }
    
    void setSymbolCount(size_t count) { m_symbolCount = count; }
    void setLanguage(const std::string& language) { m_language = language; }
    
private:
    PerformanceLogger& m_logger;
    std::string m_filePath;
    size_t m_symbolCount = 0;
    std::string m_language = "unknown";
};

#endif // PERFORMANCELOGGER_HPP 