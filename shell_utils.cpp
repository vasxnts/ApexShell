#include "shell_utils.h"
#include <sstream>
#include <unistd.h>

// split by whitespace, nothing fancy
std::vector<std::string> splitLine(const std::string& line)
{
    std::stringstream ss(line);
    std::vector<std::string> tokens;
    std::string w;

    while (ss >> w)
        tokens.push_back(w);

    return tokens;
}

// replace $VAR with env value if it exists
void substituteEnv(std::vector<std::string>& tokens)
{
    for (auto& t : tokens)
    {
        if (t.size() > 1 && t[0] == '$')
        {
            char* v = getenv(t.c_str() + 1);
            if (v)
                t = v;
            // if not found — leave as is
        }
    }
}

// build argv[] for execvp
std::vector<char*> makeArgv(std::vector<std::string>& cmd)
{
    std::vector<char*> argv;

    for (auto& s : cmd)
        argv.push_back(const_cast<char*>(s.c_str()));

    argv.push_back(nullptr); // exec expects null-terminated argv
    return argv;
}