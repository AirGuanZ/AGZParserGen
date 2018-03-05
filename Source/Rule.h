/*================================================================
Filename: Rule.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include "Common.h"

AGZ_NAMESPACE_BEGIN(AGZ)

enum class RuleSymbolType
{
    Token,
    NonTerminate
};

struct RawRule
{
    struct Symbol
    {
        RuleSymbolType type;
        std::string name;
    };

    std::string left;
    std::vector<Symbol> syms;
};

AGZ_NAMESPACE_END(AGZ)
