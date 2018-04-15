/*================================================================
Filename: Utility.cpp
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>

#include <Utility.h>

NS_BEGIN(AGZ)

bool ReadTxt(const String &filename, String &output)
{
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = String(std::istreambuf_iterator<char>(fin),
                    std::istreambuf_iterator<char>());
    return true;
}

NS_END(AGZ)
