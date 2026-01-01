#include "executor.h"
#include "shell_utils.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>

// run commands connected with pipes
void executePipeline(std::vector<std::vector<std::string>>& cmds,
                     const std::string& inFile,
                     const std::string& outFile,
                     bool append)
{
    int prevFd = -1; // read end from previous pipe

    for (size_t i = 0; i < cmds.size(); ++i)
    {
        int pipefd[2];
        bool hasNext = (i + 1 < cmds.size());

        // pipe only if there is a next command
        if (hasNext && pipe(pipefd) < 0)
        {
            perror("apex_shell: pipe");
            return;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // first command may read from file
            if (i == 0 && !inFile.empty())
            {
                int fd = open(inFile.c_str(), O_RDONLY);
                if (fd < 0) {
                    perror("apex_shell: input");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // stdin from previous pipe
            if (prevFd != -1)
            {
                dup2(prevFd, STDIN_FILENO);
                close(prevFd);
            }

            // stdout to next pipe
            if (hasNext)
            {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            // last command can write to file
            else if (!outFile.empty())
            {
                int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                int fd = open(outFile.c_str(), flags, 0644);
                if (fd < 0) {
                    perror("apex_shell: output");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // exec replaces process image
            auto argv = makeArgv(cmds[i]);
            execvp(argv[0], argv.data());

            // exec returns only on error
            perror("apex_shell: execvp");
            exit(1);
        }
        else if (pid < 0)
        {
            perror("apex_shell: fork");
            return;
        }

        // parent cleanup
        if (prevFd != -1)
            close(prevFd);

        if (hasNext)
        {
            close(pipefd[1]);      // keep read end
            prevFd = pipefd[0];
        }
    }

    // wait for all children
    while (wait(nullptr) > 0);
}