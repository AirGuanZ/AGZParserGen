/*================================================================
Filename: Rule.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include "Common.h"

AGZ_NAMESPACE_BEGIN(AGZ)

struct Rule
{
    struct Symbol
    {
        enum class Type
        {
            Token,
            NonTerminate
        };

        Type type;
        std::string name;
    };

    std::string left;
    std::vector<Symbol> syms;
};

AGZ_NAMESPACE_END(AGZ)
