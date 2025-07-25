#include "FileWatcher.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <thread>

#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#elif __linux__
#include <sys/inotify.h>
#include <unistd.h>
#endif

FileWatcher::FileWatcher() 
    : m_running(false)
    , m_watching(false)
    , m_watchedFileCount(0)
    , m_changeEventCount(0)
    , m_debounceTime(std::chrono::milliseconds(100))
    , m_maxEvents(1000)
#ifdef __APPLE__
    , m_kqueue(-1)
#elif __linux__
    , m_inotify(-1)
#else
    , m_lastScan(std::chrono::system_clock::now())
#endif
{
}

FileWatcher::~FileWatcher() {
    stopWatching();
}

bool FileWatcher::startWatching(const std::string& rootPath, const std::vector<std::string>& extensions) {
    if (m_watching) {
        return false;
    }
    
    m_rootPath = rootPath;
    m_extensions = extensions;
    m_running = true;
    m_watching = true;
    m_changeEventCount = 0;
    
    // Initial scan to populate watched files
    scanDirectory(rootPath);
    
    std::cout << "🔍 Starting file watcher for " << m_watchedFileCount << " files...\n";
    
    // Start the watch thread
    m_watchThread = std::thread([this]() {
        try {
            watchLoop();
        } catch (const std::exception& e) {
            std::cerr << "❌ File watcher error: " << e.what() << "\n";
            m_watching = false;
        }
    });
    
    return true;
}

void FileWatcher::stopWatching() {
    if (!m_watching) {
        return;
    }
    
    m_running = false;
    m_watching = false;
    
    if (m_watchThread.joinable()) {
        m_watchThread.join();
    }
    
#ifdef __APPLE__
    cleanupKqueue();
#elif __linux__
    cleanupInotify();
#endif
    
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_watchedFiles.clear();
    m_watchedFileCount = 0;
    
    std::cout << "⏹️  File watcher stopped\n";
}

void FileWatcher::setChangeCallback(ChangeCallback callback) {
    m_callback = callback;
}

bool FileWatcher::isWatching() const {
    return m_watching;
}

size_t FileWatcher::getWatchedFileCount() const {
    return m_watchedFileCount;
}

size_t FileWatcher::getChangeEventCount() const {
    return m_changeEventCount;
}

void FileWatcher::setDebounceTime(std::chrono::milliseconds ms) {
    m_debounceTime = ms;
}

void FileWatcher::setMaxEvents(size_t maxEvents) {
    m_maxEvents = maxEvents;
}

void FileWatcher::watchLoop() {
#ifdef __APPLE__
    setupKqueue();
    
    while (m_running) {
        struct kevent events[64];
        struct timespec timeout = {1, 0}; // 1 second timeout
        
        int eventCount = kevent(m_kqueue, nullptr, 0, events, 64, &timeout);
        
        if (eventCount > 0) {
            for (int i = 0; i < eventCount; ++i) {
                // Handle kqueue events
                if (events[i].filter == EVFILT_VNODE) {
                    std::string path = reinterpret_cast<char*>(events[i].udata);
                    
                    if (events[i].fflags & NOTE_WRITE) {
                        handleFileChange(path, FileEvent::MODIFIED);
                    } else if (events[i].fflags & NOTE_DELETE) {
                        handleFileChange(path, FileEvent::DELETED);
                    }
                }
            }
        }
    }
    
#elif __linux__
    setupInotify();
    
    char buffer[4096];
    while (m_running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_inotify, &readfds);
        
        struct timeval timeout = {1, 0}; // 1 second timeout
        int result = select(m_inotify + 1, &readfds, nullptr, nullptr, &timeout);
        
        if (result > 0 && FD_ISSET(m_inotify, &readfds)) {
            ssize_t length = read(m_inotify, buffer, sizeof(buffer));
            
            if (length > 0) {
                size_t offset = 0;
                while (offset < static_cast<size_t>(length)) {
                    struct inotify_event* event = reinterpret_cast<struct inotify_event*>(buffer + offset);
                    
                    if (event->len > 0) {
                        std::string path = std::string(event->name);
                        
                        if (event->mask & IN_MODIFY) {
                            handleFileChange(path, FileEvent::MODIFIED);
                        } else if (event->mask & IN_CREATE) {
                            handleFileChange(path, FileEvent::CREATED);
                        } else if (event->mask & IN_DELETE) {
                            handleFileChange(path, FileEvent::DELETED);
                        }
                    }
                    
                    offset += sizeof(struct inotify_event) + event->len;
                }
            }
        }
    }
    
#else
    // Fallback polling implementation
    while (m_running) {
        pollForChanges();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#endif
}

bool FileWatcher::isFileRelevant(const std::string& path) const {
    std::string ext = getFileExtension(path);
    return std::find(m_extensions.begin(), m_extensions.end(), ext) != m_extensions.end();
}

void FileWatcher::handleFileChange(const std::string& path, FileEvent event) {
    if (!isFileRelevant(path)) {
        return;
    }
    
    m_changeEventCount++;
    
    if (m_callback) {
        FileChange change(path, event);
        debounceAndNotify(change);
    }
}

void FileWatcher::debounceAndNotify(const FileChange& change) {
    // Simple debouncing - could be improved with a proper debounce mechanism
    std::this_thread::sleep_for(m_debounceTime);
    
    if (m_callback) {
        m_callback(change);
    }
}

void FileWatcher::scanDirectory(const std::string& path) {
    try {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (isFileRelevant(filePath)) {
                    m_watchedFiles.insert(filePath);
                }
            }
        }
        
        m_watchedFileCount = m_watchedFiles.size();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error scanning directory " << path << ": " << e.what() << "\n";
    }
}

std::string FileWatcher::getFileExtension(const std::string& path) const {
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        return path.substr(pos);
    }
    return "";
}

#ifdef __APPLE__
void FileWatcher::setupKqueue() {
    m_kqueue = kqueue();
    if (m_kqueue == -1) {
        throw std::runtime_error("Failed to create kqueue");
    }
}

void FileWatcher::cleanupKqueue() {
    if (m_kqueue != -1) {
        close(m_kqueue);
        m_kqueue = -1;
    }
}

void FileWatcher::addFileToKqueue(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        return;
    }
    
    struct kevent event;
    EV_SET(&event, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT,
           NOTE_WRITE | NOTE_DELETE, 0, const_cast<char*>(path.c_str()));
    
    kevent(m_kqueue, &event, 1, nullptr, 0, nullptr);
}

void FileWatcher::removeFileFromKqueue(const std::string& path) {
    // Implementation for removing files from kqueue
}

#elif __linux__
void FileWatcher::setupInotify() {
    m_inotify = inotify_init();
    if (m_inotify == -1) {
        throw std::runtime_error("Failed to initialize inotify");
    }
    
    // Add root directory to inotify
    inotify_add_watch(m_inotify, m_rootPath.c_str(), 
                      IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
}

void FileWatcher::cleanupInotify() {
    if (m_inotify != -1) {
        close(m_inotify);
        m_inotify = -1;
    }
}

void FileWatcher::addFileToInotify(const std::string& path) {
    inotify_add_watch(m_inotify, path.c_str(), IN_MODIFY | IN_DELETE);
}

#else
void FileWatcher::pollForChanges() {
    auto now = std::chrono::system_clock::now();
    
    try {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        
        for (const auto& filePath : m_watchedFiles) {
            if (std::filesystem::exists(filePath)) {
                auto lastModified = std::filesystem::last_write_time(filePath);
                // Convert to system_clock time point for comparison
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    lastModified - std::filesystem::file_time_type::clock::now() + now
                );
                
                if (sctp > m_lastScan) {
                    handleFileChange(filePath, FileEvent::MODIFIED);
                }
            } else {
                handleFileChange(filePath, FileEvent::DELETED);
            }
        }
        
        m_lastScan = now;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error polling for changes: " << e.what() << "\n";
    }
}
#endif 