/*================================================================
Filename: Rule.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include <Common.h>

AGZ_NAMESPACE_BEGIN(AGZ)

struct Rule
{
    struct Symbol
    {
        enum Type
        {
            Token,
            NonTerminate
        };

        Type type;
        String name;
    };

    String left;
    std::vector<Symbol> syms;
};

AGZ_NAMESPACE_END(AGZ)
