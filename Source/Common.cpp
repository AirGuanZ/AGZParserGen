/*================================================================
Filename: Common.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>
#include <sstream>

#include "Common.h"

AGZ_NAMESPACE_BEGIN(AGZ)

bool ReadFile(const std::string &filename, std::string &output)
{
    output = "";
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = std::string(std::istreambuf_iterator<char>(fin),
                         std::istreambuf_iterator<char>());
    return true;
}

std::string StrJoin(const std::vector<std::string> &strs, const std::string &join)
{
    if(strs.empty())
        return "";
    std::stringstream sst(strs[0]);
    for(size_t i = 1; i < strs.size(); ++i)
        sst << join << strs[i];
    return sst.str();
}

AGZ_NAMESPACE_END(AGZ)
