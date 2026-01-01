#pragma once
#include <vector>
#include <string>

std::vector<std::string> splitLine(const std::string& line);
void substituteEnv(std::vector<std::string>& tokens);
std::vector<char*> makeArgv(std::vector<std::string>& cmd);