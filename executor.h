#pragma once
#include <vector>
#include <string>

void executePipeline(std::vector<std::vector<std::string>>& cmds,
const std::string& inFile, const std::string& outFile, bool append);