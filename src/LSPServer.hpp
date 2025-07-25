#pragma once

#include "Symbol.hpp"
#include "JsonExporter.hpp"
#include <string>
#include <map>
#include <functional>
#include <memory>

struct LSPRequest {
    std::string jsonrpc;
    std::string method;
    std::string id;
    std::map<std::string, std::string> params;
    
    LSPRequest() : jsonrpc("2.0") {}
};

struct LSPResponse {
    std::string jsonrpc;
    std::string id;
    std::string result;
    std::string error;
    
    LSPResponse() : jsonrpc("2.0") {}
};

class LSPServer {
public:
    LSPServer();
    ~LSPServer() = default;
    
    // Core LSP lifecycle
    void start();
    void stop();
    bool isRunning() const { return m_running; }
    
    // Message handling
    std::string processMessage(const std::string& message);
    LSPResponse processRequest(const LSPRequest& request);
    
    // LSP method handlers
    std::string handleInitialize(const std::map<std::string, std::string>& params);
    std::string handleInitialized(const std::map<std::string, std::string>& params);
    std::string handleShutdown(const std::map<std::string, std::string>& params);
    std::string handleExit(const std::map<std::string, std::string>& params);
    
    // Document methods
    std::string handleTextDocumentDidOpen(const std::map<std::string, std::string>& params);
    std::string handleTextDocumentDidChange(const std::map<std::string, std::string>& params);
    std::string handleTextDocumentDidClose(const std::map<std::string, std::string>& params);
    std::string handleTextDocumentDocumentSymbol(const std::map<std::string, std::string>& params);
    
    // Workspace methods
    std::string handleWorkspaceSymbol(const std::map<std::string, std::string>& params);
    std::string handleWorkspaceDidChangeWatchedFiles(const std::map<std::string, std::string>& params);
    
    // Symbol navigation
    std::string handleTextDocumentDefinition(const std::map<std::string, std::string>& params);
    std::string handleTextDocumentReferences(const std::map<std::string, std::string>& params);
    std::string handleTextDocumentHover(const std::map<std::string, std::string>& params);
    
    // Configuration
    void setWorkspaceRoot(const std::string& root);
    void enableLogging(bool enable) { m_loggingEnabled = enable; }
    
    // Index management
    void rebuildIndex();
    void updateFile(const std::string& filePath);

private:
    // Message parsing
    LSPRequest parseRequest(const std::string& message) const;
    std::string formatResponse(const LSPResponse& response) const;
    std::string formatError(const std::string& id, int code, const std::string& message) const;
    
    // Utility methods
    std::string extractParam(const std::map<std::string, std::string>& params, const std::string& key) const;
    void logMessage(const std::string& message) const;
    std::string uriToPath(const std::string& uri) const;
    std::string pathToUri(const std::string& path) const;
    
    // Member variables
    bool m_running = false;
    bool m_initialized = false;
    bool m_loggingEnabled = false;
    std::string m_workspaceRoot;
    std::unique_ptr<SymbolIndex> m_index;
    std::unique_ptr<JsonExporter> m_exporter;
    
    // Method handlers map
    std::map<std::string, std::function<std::string(const std::map<std::string, std::string>&)>> m_handlers;
    
    // Initialize handlers
    void initializeHandlers();
}; 