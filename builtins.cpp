#include "builtins.h"
#include <iostream>
#include <unistd.h>

bool isBuiltin(const std::string& cmd)
{
    return cmd == "cd" || cmd == "pwd" || cmd == "exit" ||
           cmd == "export" || cmd == "echo";
}

void runBuiltin(std::vector<std::string>& tokens, const std::string& line) {
    const std::string& cmd = tokens[0];

    if (cmd == "exit")
        exit(0);

    if (cmd == "pwd") {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
            std::cout << cwd << std::endl;
        return;
    }

    if (cmd == "cd") {
        static std::string lastDir;
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));

        if (tokens.size() < 2) {
            std::cerr << "apex_shell: expected argument to \"cd\"\n";
            return;
        }

        std::string path = tokens[1];
        if (path == "-") {
            if (lastDir.empty()) {
                std::cerr << "apex_shell: OLDPWD not set\n";
                return;
            }
            path = lastDir;
            std::cout << path << std::endl;
        }

        if (chdir(path.c_str()) != 0)
            perror("apex_shell");
        else
            lastDir = cwd;
        return;
    }

    if (cmd == "export") {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string name = line.substr(7, pos - 7);
            std::string value = line.substr(pos + 1);

            name.erase(0, name.find_first_not_of(" "));
            name.erase(name.find_last_not_of(" ") + 1);

            setenv(name.c_str(), value.c_str(), 1);
        }
        return;
    }

    if (cmd == "echo") {
        for (size_t i = 1; i < tokens.size(); ++i) {
            std::cout << tokens[i];
            if (i + 1 < tokens.size())
                std::cout << " ";
        }
        std::cout << std::endl;
        return;
    }
}