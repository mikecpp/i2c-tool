#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

using CommandHandler = std::function<int(int argc, char* argv[])>;  

struct CommandInfo {
    CommandHandler handler;
    std::string helpString;
    
    CommandInfo(CommandHandler h, const std::string& help) 
        : handler(h), helpString(help) {}
};

class CmdManager {
public:  
    CmdManager();
    void registerCommand(const std::string& command, CommandHandler handler, const std::string& helpString);
    int execute(const std::string& commandLine);

private:
    std::unordered_map<std::string, CommandInfo> commands_;    
    int helpCommandHandler(int argc, char* argv[]); 
    char** createArgv(const std::vector<std::string>& tokens); 
    void freeArgv(char** argv, int argc);
};
