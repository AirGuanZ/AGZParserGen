/*================================================================
Filename: Utility.cpp
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#include <fstream>
#include <iterator>
#include <sstream>

#include <Common/Utility.h>

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

String StrJoin(const Vec<String> &strs, const String &join)
{
    if(strs.empty())
        return "";
    std::stringstream sst;
    sst << strs[0];
    for(size_t i = 1; i != strs.size(); ++i)
        sst << join << strs[i];
    return sst.str();
}

NS_END(AGZ)
