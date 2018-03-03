/*================================================================
Filename: Common.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>

#include "Common.h"

AGZ_NAMESPACE_BEGIN(AGZ)

bool ReadFile(const String &filename, String &output)
{
    output = "";
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = std::string(std::istreambuf_iterator<char>(fin),
                         std::istreambuf_iterator<char>());
    return true;
}

AGZ_NAMESPACE_END(AGZ)
