#include "LSPServer.hpp"
#include "FileScanner.hpp"
#include <iostream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

LSPServer::LSPServer() : m_index(std::make_unique<SymbolIndex>()), m_exporter(std::make_unique<JsonExporter>()) {
    initializeHandlers();
}

void LSPServer::start() {
    m_running = true;
    logMessage("LSP Server started");
    
    std::string line;
    while (m_running && std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        std::string response = processMessage(line);
        if (!response.empty()) {
            std::cout << response << std::endl;
        }
    }
}

void LSPServer::stop() {
    m_running = false;
    logMessage("LSP Server stopped");
}

std::string LSPServer::processMessage(const std::string& message) {
    try {
        LSPRequest request = parseRequest(message);
        LSPResponse response = processRequest(request);
        return formatResponse(response);
    } catch (const std::exception& e) {
        logMessage("Error processing message: " + std::string(e.what()));
        return formatError("", -32700, "Parse error");
    }
}

LSPResponse LSPServer::processRequest(const LSPRequest& request) {
    LSPResponse response;
    response.id = request.id;
    
    auto handler = m_handlers.find(request.method);
    if (handler != m_handlers.end()) {
        try {
            response.result = handler->second(request.params);
        } catch (const std::exception& e) {
            response.error = "Internal error: " + std::string(e.what());
        }
    } else {
        response.error = "Method not found: " + request.method;
    }
    
    return response;
}

std::string LSPServer::handleInitialize(const std::map<std::string, std::string>& params) {
    std::string rootUri = extractParam(params, "rootUri");
    if (!rootUri.empty()) {
        m_workspaceRoot = uriToPath(rootUri);
        logMessage("Workspace root: " + m_workspaceRoot);
    }
    
    // Return server capabilities
    return R"({
        "capabilities": {
            "textDocumentSync": 1,
            "documentSymbolProvider": true,
            "workspaceSymbolProvider": true,
            "definitionProvider": true,
            "referencesProvider": true,
            "hoverProvider": true
        },
        "serverInfo": {
            "name": "Navix LSP Server",
            "version": "0.1.0"
        }
    })";
}

std::string LSPServer::handleInitialized(const std::map<std::string, std::string>& params) {
    m_initialized = true;
    
    // Build initial index
    if (!m_workspaceRoot.empty()) {
        rebuildIndex();
    }
    
    logMessage("LSP Server initialized");
    return "null";
}

std::string LSPServer::handleShutdown(const std::map<std::string, std::string>& params) {
    logMessage("Shutdown requested");
    return "null";
}

std::string LSPServer::handleExit(const std::map<std::string, std::string>& params) {
    stop();
    return "";
}

std::string LSPServer::handleTextDocumentDidOpen(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    std::string filePath = uriToPath(uri);
    
    if (!filePath.empty()) {
        updateFile(filePath);
        logMessage("Document opened: " + filePath);
    }
    
    return "";
}

std::string LSPServer::handleTextDocumentDidChange(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    std::string filePath = uriToPath(uri);
    
    if (!filePath.empty()) {
        updateFile(filePath);
        logMessage("Document changed: " + filePath);
    }
    
    return "";
}

std::string LSPServer::handleTextDocumentDidClose(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    logMessage("Document closed: " + uriToPath(uri));
    return "";
}

std::string LSPServer::handleTextDocumentDocumentSymbol(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    std::string filePath = uriToPath(uri);
    
    if (!filePath.empty() && m_index) {
        return m_exporter->exportDocumentSymbols(*m_index, filePath);
    }
    
    return "[]";
}

std::string LSPServer::handleWorkspaceSymbol(const std::map<std::string, std::string>& params) {
    std::string query = extractParam(params, "query");
    
    if (m_index) {
        return m_exporter->exportWorkspaceSymbols(*m_index, query);
    }
    
    return "[]";
}

std::string LSPServer::handleWorkspaceDidChangeWatchedFiles(const std::map<std::string, std::string>& params) {
    // Rebuild index when files change
    rebuildIndex();
    logMessage("Workspace files changed, index rebuilt");
    return "";
}

std::string LSPServer::handleTextDocumentDefinition(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    std::string position = extractParam(params, "position");
    
    // For now, return empty array - would need more sophisticated implementation
    logMessage("Definition requested for: " + uri + " at " + position);
    return "[]";
}

std::string LSPServer::handleTextDocumentReferences(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    std::string position = extractParam(params, "position");
    
    // For now, return empty array - would need more sophisticated implementation
    logMessage("References requested for: " + uri + " at " + position);
    return "[]";
}

std::string LSPServer::handleTextDocumentHover(const std::map<std::string, std::string>& params) {
    std::string uri = extractParam(params, "uri");
    std::string position = extractParam(params, "position");
    
    // For now, return null - would need more sophisticated implementation
    logMessage("Hover requested for: " + uri + " at " + position);
    return "null";
}

void LSPServer::setWorkspaceRoot(const std::string& root) {
    m_workspaceRoot = root;
    rebuildIndex();
}

void LSPServer::rebuildIndex() {
    if (m_workspaceRoot.empty() || !m_index) return;
    
    logMessage("Rebuilding symbol index for: " + m_workspaceRoot);
    
    auto files = FileScanner::scanForAllSupportedFiles(m_workspaceRoot);
    m_index->clear();
    m_index->buildIndex(files);
    
    logMessage("Index rebuilt with " + std::to_string(m_index->size()) + " symbols");
}

void LSPServer::updateFile(const std::string& filePath) {
    if (!m_index || !fs::exists(filePath)) return;
    
    // For now, just rebuild the entire index
    // A more sophisticated implementation would update only the specific file
    rebuildIndex();
}

LSPRequest LSPServer::parseRequest(const std::string& message) const {
    LSPRequest request;
    
    // Simple JSON parsing - in production, use a proper JSON library
    std::regex methodRegex("\"method\"\\s*:\\s*\"([^\"]+)\"");
    std::regex idRegex("\"id\"\\s*:\\s*\"?([^\",}]+)\"?");
    std::regex paramsRegex("\"params\"\\s*:\\s*\\{([^}]*)\\}");
    
    std::smatch match;
    
    if (std::regex_search(message, match, methodRegex)) {
        request.method = match[1].str();
    }
    
    if (std::regex_search(message, match, idRegex)) {
        request.id = match[1].str();
    }
    
    if (std::regex_search(message, match, paramsRegex)) {
        std::string paramsStr = match[1].str();
        
        // Parse common parameters
        std::regex uriRegex("\"uri\"\\s*:\\s*\"([^\"]+)\"");
        std::regex queryRegex("\"query\"\\s*:\\s*\"([^\"]+)\"");
        std::regex rootUriRegex("\"rootUri\"\\s*:\\s*\"([^\"]+)\"");
        std::regex positionRegex("\"position\"\\s*:\\s*\\{[^}]*\"line\"\\s*:\\s*(\\d+)[^}]*\"character\"\\s*:\\s*(\\d+)[^}]*\\}");
        
        if (std::regex_search(paramsStr, match, uriRegex)) {
            request.params["uri"] = match[1].str();
        }
        if (std::regex_search(paramsStr, match, queryRegex)) {
            request.params["query"] = match[1].str();
        }
        if (std::regex_search(paramsStr, match, rootUriRegex)) {
            request.params["rootUri"] = match[1].str();
        }
        if (std::regex_search(paramsStr, match, positionRegex)) {
            request.params["position"] = match[1].str() + ":" + match[2].str();
        }
    }
    
    return request;
}

std::string LSPServer::formatResponse(const LSPResponse& response) const {
    std::ostringstream json;
    
    json << "{";
    json << "\"jsonrpc\":\"" << response.jsonrpc << "\"";
    
    if (!response.id.empty()) {
        json << ",\"id\":\"" << response.id << "\"";
    }
    
    if (!response.error.empty()) {
        json << ",\"error\":{\"code\":-32603,\"message\":\"" << response.error << "\"}";
    } else {
        json << ",\"result\":" << response.result;
    }
    
    json << "}";
    
    return json.str();
}

std::string LSPServer::formatError(const std::string& id, int code, const std::string& message) const {
    std::ostringstream json;
    
    json << "{";
    json << "\"jsonrpc\":\"2.0\"";
    
    if (!id.empty()) {
        json << ",\"id\":\"" << id << "\"";
    } else {
        json << ",\"id\":null";
    }
    
    json << ",\"error\":{\"code\":" << code << ",\"message\":\"" << message << "\"}";
    json << "}";
    
    return json.str();
}

std::string LSPServer::extractParam(const std::map<std::string, std::string>& params, const std::string& key) const {
    auto it = params.find(key);
    return (it != params.end()) ? it->second : "";
}

void LSPServer::logMessage(const std::string& message) const {
    if (m_loggingEnabled) {
        std::cerr << "[LSP] " << message << std::endl;
    }
}

std::string LSPServer::uriToPath(const std::string& uri) const {
    if (uri.substr(0, 7) == "file://") {
        return uri.substr(7);
    }
    return uri;
}

std::string LSPServer::pathToUri(const std::string& path) const {
    return "file://" + path;
}

void LSPServer::initializeHandlers() {
    m_handlers["initialize"] = [this](const auto& params) { return handleInitialize(params); };
    m_handlers["initialized"] = [this](const auto& params) { return handleInitialized(params); };
    m_handlers["shutdown"] = [this](const auto& params) { return handleShutdown(params); };
    m_handlers["exit"] = [this](const auto& params) { return handleExit(params); };
    
    m_handlers["textDocument/didOpen"] = [this](const auto& params) { return handleTextDocumentDidOpen(params); };
    m_handlers["textDocument/didChange"] = [this](const auto& params) { return handleTextDocumentDidChange(params); };
    m_handlers["textDocument/didClose"] = [this](const auto& params) { return handleTextDocumentDidClose(params); };
    m_handlers["textDocument/documentSymbol"] = [this](const auto& params) { return handleTextDocumentDocumentSymbol(params); };
    m_handlers["textDocument/definition"] = [this](const auto& params) { return handleTextDocumentDefinition(params); };
    m_handlers["textDocument/references"] = [this](const auto& params) { return handleTextDocumentReferences(params); };
    m_handlers["textDocument/hover"] = [this](const auto& params) { return handleTextDocumentHover(params); };
    
    m_handlers["workspace/symbol"] = [this](const auto& params) { return handleWorkspaceSymbol(params); };
    m_handlers["workspace/didChangeWatchedFiles"] = [this](const auto& params) { return handleWorkspaceDidChangeWatchedFiles(params); };
} 