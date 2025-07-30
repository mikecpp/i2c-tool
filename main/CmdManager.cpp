#include "CmdManager.h"
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

CmdManager::CmdManager() 
{
    registerCommand("help", 
        [this](int argc, char* argv[]) { return helpCommandHandler(argc, argv); },
        "Show help for all commands or specific command");
}

void CmdManager::registerCommand(const std::string& command, CommandHandler handler, const std::string& helpString) 
{
    commands_.emplace(command, CommandInfo(handler, helpString));
}

int CmdManager::execute(const std::string& commandLine) 
{
    if (commandLine.empty()) {
        printf("Error: Empty command\n");
        return -1;
    }

    std::vector<std::string> tokens;
    std::istringstream iss(commandLine);
    std::string token;
    char c;
    while (iss >> std::ws) {
        if (iss.peek() == '"') {
            iss.get(c); 
            std::getline(iss, token, '"');
            tokens.push_back(token);
        } else {
            iss >> token;
            tokens.push_back(token);
        }
    }
  
    if (tokens.empty()) {
        printf("Error: No command specified\n");
        return -1;
    }
    
    std::string command = tokens[0];
    auto it = commands_.find(command);
    if (it == commands_.end()) {
        printf("Error: Unknown command '%s'. Type 'help' for available commands.\n", command.c_str());
        return -1;
    }
    
    int argc = static_cast<int>(tokens.size());
    char** argv = createArgv(tokens);
    
    int result = it->second.handler(argc, argv);
    freeArgv(argv, argc);
    return result;
}

int CmdManager::helpCommandHandler(int argc, char* argv[]) 
{
    if (argc == 1) {
        printf("Available commands:\n");
        std::vector<std::string> commandList;
        for (const auto& pair : commands_) {
            commandList.push_back(pair.first);
        }
        std::sort(commandList.begin(), commandList.end());
        for (const auto& cmd : commandList) {
            const auto& info = commands_.at(cmd);
            printf("%s - %s\n", cmd.c_str(), info.helpString.c_str());
        }
    } 
    else if (argc == 2) {
        std::string targetCommand = argv[1];
        auto it = commands_.find(targetCommand);
        if (it != commands_.end()) {
            printf("%s\n", it->second.helpString.c_str());
        } else {
            printf("Unknown command: %s\n", targetCommand.c_str());
            return -1;
        }
    } 
    else {
        printf("Usage: help [command]\n");
        return -1;
    }
    
    return 0;
}

char** CmdManager::createArgv(const std::vector<std::string>& tokens) 
{
    char** argv = static_cast<char**>(malloc(tokens.size() * sizeof(char*)));
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        argv[i] = static_cast<char*> (malloc((tokens[i].length() + 1) * sizeof(char)));
        strcpy(argv[i], tokens[i].c_str());
    }
    
    return argv;
}

void CmdManager::freeArgv(char** argv, int argc) 
{
    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }

    free(argv);
}
