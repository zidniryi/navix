#include "PerformanceLogger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>

PerformanceLogger::PerformanceLogger()
    : m_verbose(false)
    , m_logToFile(false)
    , m_logFilename("navix_performance.log")
    , m_minLogTime(std::chrono::milliseconds(1))
    , m_sessionActive(false)
    , m_filesProcessed(0)
    , m_symbolsFound(0)
    , m_errorsEncountered(0)
{
}

PerformanceLogger::~PerformanceLogger() {
    if (m_sessionActive) {
        endSession();
    }
}

void PerformanceLogger::startSession(const std::string& operation) {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    m_currentSession = SessionMetrics();
    m_currentOperation = operation;
    m_sessionActive = true;
    m_filesProcessed = 0;
    m_symbolsFound = 0;
    m_errorsEncountered = 0;
    
    std::string msg = "📊 Starting " + operation + " session at " + getCurrentTimeString();
    
    if (m_verbose) {
        std::cout << msg << "\n";
    }
    
    if (m_logToFile) {
        writeToLog(msg);
    }
}

void PerformanceLogger::endSession() {
    if (!m_sessionActive) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    auto endTime = std::chrono::system_clock::now();
    m_currentSession.totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - m_currentSession.startTime);
    
    m_sessionActive = false;
    
    std::string msg = "✅ Session completed in " + formatDuration(m_currentSession.totalTime);
    
    if (m_verbose) {
        std::cout << msg << "\n";
        printSessionSummary();
    }
    
    if (m_logToFile) {
        writeToLog(msg);
        writeToLog("Session Summary:");
        writeToLog("- Files processed: " + std::to_string(m_currentSession.totalFiles));
        writeToLog("- Symbols found: " + std::to_string(m_currentSession.totalSymbols));
        writeToLog("- Total size: " + formatSize(m_currentSession.totalBytes));
        writeToLog("- Files/sec: " + std::to_string(getFilesPerSecond()));
        writeToLog("- Symbols/sec: " + std::to_string(getSymbolsPerSecond()));
    }
}

void PerformanceLogger::logFileStart(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    m_fileStartTimes[filePath] = std::chrono::steady_clock::now();
}

void PerformanceLogger::logFileEnd(const std::string& filePath, size_t symbolCount, const std::string& language) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    
    auto it = m_fileStartTimes.find(filePath);
    if (it == m_fileStartTimes.end()) {
        return;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->second);
    
    if (duration >= m_minLogTime) {
        size_t fileSize = getFileSize(filePath);
        FileMetrics metrics(filePath, duration, symbolCount, fileSize, language);
        
        {
            std::lock_guard<std::mutex> metricsLock(m_metricsMutex);
            m_fileMetrics.push_back(metrics);
            updateSessionMetrics(metrics);
        }
        
        m_filesProcessed++;
        m_symbolsFound += symbolCount;
        
        if (m_verbose && duration > std::chrono::milliseconds(10)) {
            std::cout << "📁 " << std::filesystem::path(filePath).filename().string() 
                     << " (" << language << "): " << formatDuration(duration)
                     << ", " << symbolCount << " symbols, " << formatSize(fileSize) << "\n";
        }
        
        if (m_logToFile) {
            std::ostringstream oss;
            oss << getCurrentTimeString() << " - " << filePath 
                << " | " << language << " | " << duration.count() << "ms"
                << " | " << symbolCount << " symbols | " << fileSize << " bytes";
            writeToLog(oss.str());
        }
    }
    
    m_fileStartTimes.erase(it);
}

void PerformanceLogger::logSymbol(const std::string& symbolType) {
    m_symbolsFound++;
}

void PerformanceLogger::logError(const std::string& filePath, const std::string& error) {
    m_errorsEncountered++;
    
    std::string msg = "❌ Error in " + filePath + ": " + error;
    
    if (m_verbose) {
        std::cout << msg << "\n";
    }
    
    if (m_logToFile) {
        writeToLog(msg);
    }
}

SessionMetrics PerformanceLogger::getCurrentSession() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    return m_currentSession;
}

std::vector<FileMetrics> PerformanceLogger::getFileMetrics() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    return m_fileMetrics;
}

void PerformanceLogger::printSessionSummary() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    std::cout << "\n┌─ PERFORMANCE SUMMARY ──────────────────────────────────────────────────────┐\n";
    std::cout << "│ 📊 Session: " << m_currentOperation << "\n";
    std::cout << "│ ⏱️  Total Time: " << formatDuration(m_currentSession.totalTime) << "\n";
    std::cout << "│ 📁 Files Processed: " << m_currentSession.totalFiles << "\n";
    std::cout << "│ 🎯 Symbols Found: " << m_currentSession.totalSymbols << "\n";
    std::cout << "│ 💾 Data Processed: " << formatSize(m_currentSession.totalBytes) << "\n";
    std::cout << "│ ⚡ Files/Second: " << std::fixed << std::setprecision(1) << getFilesPerSecond() << "\n";
    std::cout << "│ 🎪 Symbols/Second: " << std::fixed << std::setprecision(1) << getSymbolsPerSecond() << "\n";
    
    if (m_errorsEncountered > 0) {
        std::cout << "│ ❌ Errors: " << m_errorsEncountered << "\n";
    }
    
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
}

void PerformanceLogger::printTopSlowFiles(size_t count) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    auto sortedMetrics = m_fileMetrics;
    std::sort(sortedMetrics.begin(), sortedMetrics.end(),
              [](const FileMetrics& a, const FileMetrics& b) {
                  return a.parseTime > b.parseTime;
              });
    
    std::cout << "\n┌─ SLOWEST FILES ────────────────────────────────────────────────────────────┐\n";
    
    size_t displayCount = std::min(count, sortedMetrics.size());
    for (size_t i = 0; i < displayCount; ++i) {
        const auto& metrics = sortedMetrics[i];
        std::string filename = std::filesystem::path(metrics.filePath).filename().string();
        
        std::cout << "│ " << std::setw(3) << (i + 1) << ". " 
                 << std::setw(25) << filename.substr(0, 25)
                 << " | " << std::setw(8) << formatDuration(metrics.parseTime)
                 << " | " << std::setw(6) << metrics.symbolCount << " symbols"
                 << " | " << metrics.language << "\n";
    }
    
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
}

void PerformanceLogger::printLanguageBreakdown() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    std::cout << "\n┌─ LANGUAGE BREAKDOWN ───────────────────────────────────────────────────────┐\n";
    
    for (const auto& [language, count] : m_currentSession.languageCounts) {
        auto timeIt = m_currentSession.languageTimes.find(language);
        auto avgTime = timeIt != m_currentSession.languageTimes.end() ? 
                       timeIt->second.count() / count : 0;
        
        std::cout << "│ " << std::setw(12) << language 
                 << " | " << std::setw(8) << count << " files"
                 << " | " << std::setw(8) << avgTime << "ms avg"
                 << " | " << std::setw(10) << formatDuration(timeIt->second) << " total\n";
    }
    
    std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
}

void PerformanceLogger::printLiveStats() const {
    if (!m_sessionActive) {
        return;
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - m_currentSession.startTime);
    
    std::cout << "\r🔄 Live: " << m_filesProcessed << " files, "
             << m_symbolsFound << " symbols, "
             << formatDuration(elapsed) << " elapsed, "
             << std::fixed << std::setprecision(1) << getFilesPerSecond() << " files/sec"
             << std::flush;
}

double PerformanceLogger::getFilesPerSecond() const {
    if (!m_sessionActive || m_currentSession.totalTime.count() == 0) {
        return 0.0;
    }
    
    double seconds = m_currentSession.totalTime.count() / 1000.0;
    return static_cast<double>(m_filesProcessed) / seconds;
}

double PerformanceLogger::getSymbolsPerSecond() const {
    if (!m_sessionActive || m_currentSession.totalTime.count() == 0) {
        return 0.0;
    }
    
    double seconds = m_currentSession.totalTime.count() / 1000.0;
    return static_cast<double>(m_symbolsFound) / seconds;
}

void PerformanceLogger::setVerbose(bool verbose) {
    m_verbose = verbose;
}

void PerformanceLogger::setLogToFile(bool enabled, const std::string& filename) {
    m_logToFile = enabled;
    m_logFilename = filename;
}

void PerformanceLogger::setMinLogTime(std::chrono::milliseconds minTime) {
    m_minLogTime = minTime;
}

void PerformanceLogger::writeToLog(const std::string& message) {
    if (!m_logToFile) {
        return;
    }
    
    std::ofstream logFile(m_logFilename, std::ios::app);
    if (logFile.is_open()) {
        logFile << message << "\n";
    }
}

void PerformanceLogger::updateSessionMetrics(const FileMetrics& metrics) {
    m_currentSession.totalFiles++;
    m_currentSession.totalSymbols += metrics.symbolCount;
    m_currentSession.totalBytes += metrics.fileSize;
    m_currentSession.languageCounts[metrics.language]++;
    m_currentSession.languageTimes[metrics.language] += metrics.parseTime;
}

std::string PerformanceLogger::formatDuration(std::chrono::milliseconds duration) const {
    auto ms = duration.count();
    
    if (ms < 1000) {
        return std::to_string(ms) + "ms";
    } else if (ms < 60000) {
        return std::to_string(ms / 1000.0) + "s";
    } else {
        auto minutes = ms / 60000;
        auto seconds = (ms % 60000) / 1000.0;
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }
}

std::string PerformanceLogger::formatSize(size_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB"};
    double size = static_cast<double>(bytes);
    int unit = 0;
    
    while (size >= 1024 && unit < 3) {
        size /= 1024;
        unit++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << size << units[unit];
    return oss.str();
}

std::string PerformanceLogger::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string PerformanceLogger::getLanguageFromPath(const std::string& path) const {
    std::string ext = std::filesystem::path(path).extension().string();
    
    if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc" || ext == ".cxx") {
        return "C++";
    } else if (ext == ".ts" || ext == ".tsx") {
        return "TypeScript";
    } else if (ext == ".js" || ext == ".jsx" || ext == ".mjs" || ext == ".cjs") {
        return "JavaScript";
    } else if (ext == ".py" || ext == ".pyw" || ext == ".pyi") {
        return "Python";
    } else if (ext == ".go") {
        return "Go";
    } else if (ext == ".swift") {
        return "Swift";
    } else if (ext == ".txt" || ext == ".md" || ext == ".rst" || ext == ".log") {
        return "Text";
    }
    
    return "Unknown";
}

size_t PerformanceLogger::getFileSize(const std::string& path) const {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::exception&) {
        return 0;
    }
} 