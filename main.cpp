#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include "shell_utils.h"
#include "builtins.h"
#include "executor.h"

int main()
{
    std::string line;

    while (true)
    {
        std::cout << "apex_shell> ";
        
        if (!std::getline(std::cin, line))
            break;

        if (line.empty())
            continue;

        auto tokens = splitLine(line);
        substituteEnv(tokens);

        if (tokens.empty())
            continue;

        std::string inFile, outFile;
        bool append = false;

        for (size_t i = 0; i < tokens.size(); )
        {
            if (tokens[i] == "<" && i + 1 < tokens.size())
            {
                inFile = tokens[i + 1];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            else if (tokens[i] == ">" && i + 1 < tokens.size())
            {
                outFile = tokens[i + 1];
                append = false;
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            else if (tokens[i] == ">>" && i + 1 < tokens.size())
            {
                outFile = tokens[i + 1];
                append = true;
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            else
                ++i;
        }

        if (tokens.empty())
            continue;

        if (isBuiltin(tokens[0]))
        {
            int savedIn = dup(STDIN_FILENO);
            int savedOut = dup(STDOUT_FILENO);

            if (!inFile.empty())
            {
                int fd = open(inFile.c_str(), O_RDONLY);
                if (fd >= 0)
                {
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
            }
            if (!outFile.empty())
            {
                int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                int fd = open(outFile.c_str(), flags, 0644);
                if (fd >= 0)
                {
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
            }

            runBuiltin(tokens, line);

            dup2(savedIn, STDIN_FILENO);
            dup2(savedOut, STDOUT_FILENO);
            close(savedIn);
            close(savedOut);
            
            continue;
        }

        std::vector<std::vector<std::string>> cmds;
        std::vector<std::string> current;

        for (auto& t : tokens)
        {
            if (t == "|")
            {
                if (!current.empty())
                {
                    cmds.push_back(current);
                    current.clear();
                }
            }
            else
                current.push_back(t);
        }
        if (!current.empty())
            cmds.push_back(current);

        if (!cmds.empty())
            executePipeline(cmds, inFile, outFile, append);
    }

    return 0;
}