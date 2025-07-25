#ifndef FILEWATCHER_HPP
#define FILEWATCHER_HPP

#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <unordered_set>
#include <chrono>
#include <mutex>

enum class FileEvent {
    CREATED,
    MODIFIED,
    DELETED,
    MOVED
};

struct FileChange {
    std::string path;
    FileEvent event;
    std::chrono::system_clock::time_point timestamp;
    
    FileChange(const std::string& p, FileEvent e) 
        : path(p), event(e), timestamp(std::chrono::system_clock::now()) {}
};

class FileWatcher {
public:
    using ChangeCallback = std::function<void(const FileChange&)>;
    
    FileWatcher();
    ~FileWatcher();
    
    // Start watching a directory tree
    bool startWatching(const std::string& rootPath, const std::vector<std::string>& extensions);
    
    // Stop watching
    void stopWatching();
    
    // Set callback for file changes
    void setChangeCallback(ChangeCallback callback);
    
    // Check if currently watching
    bool isWatching() const;
    
    // Get statistics
    size_t getWatchedFileCount() const;
    size_t getChangeEventCount() const;
    
    // Performance settings
    void setDebounceTime(std::chrono::milliseconds ms);
    void setMaxEvents(size_t maxEvents);
    
private:
    std::atomic<bool> m_running;
    std::atomic<bool> m_watching;
    std::thread m_watchThread;
    std::string m_rootPath;
    std::vector<std::string> m_extensions;
    ChangeCallback m_callback;
    
    // Statistics
    std::atomic<size_t> m_watchedFileCount;
    std::atomic<size_t> m_changeEventCount;
    
    // Performance settings
    std::chrono::milliseconds m_debounceTime;
    size_t m_maxEvents;
    
    // Internal state
    std::mutex m_stateMutex;
    std::unordered_set<std::string> m_watchedFiles;
    
    // Platform-specific implementation
    void watchLoop();
    bool isFileRelevant(const std::string& path) const;
    void handleFileChange(const std::string& path, FileEvent event);
    void debounceAndNotify(const FileChange& change);
    
#ifdef __APPLE__
    // macOS kqueue implementation
    int m_kqueue;
    void setupKqueue();
    void cleanupKqueue();
    void addFileToKqueue(const std::string& path);
    void removeFileFromKqueue(const std::string& path);
#elif __linux__
    // Linux inotify implementation
    int m_inotify;
    void setupInotify();
    void cleanupInotify();
    void addFileToInotify(const std::string& path);
#else
    // Fallback polling implementation
    std::chrono::system_clock::time_point m_lastScan;
    void pollForChanges();
#endif

    // Utility functions
    void scanDirectory(const std::string& path);
    std::string getFileExtension(const std::string& path) const;
};

#endif // FILEWATCHER_HPP 