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

template<typename _TokenName>
struct Rule
{
    using Name = size_t;
    using TokenName = _TokenName;

    struct Symbol
    {
        RuleSymbolType type;
        union
        {
            Name NTName;
            TokenName tokenName;
        };
    };

    Name left;
    std::vector<Symbol> syms;
};

template<typename TokenName>
inline bool operator==(const typename Rule<TokenName>::Symbol &lhs, const typename Rule<TokenName>::Symbol &rhs) noexcept
{
    return lhs.type == rhs.type && lhs.name == rhs.name;
}

template<typename TokenName>
inline bool operator==(const Rule<TokenName> &lhs, const Rule<TokenName> &rhs) noexcept
{
    return lhs.left == rhs.left && lhs.syms == rhs.syms;
}

AGZ_NAMESPACE_END(AGZ)
