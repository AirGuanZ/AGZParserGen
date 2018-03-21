/*================================================================
Filename: Rule.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <cstdlib>
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

template<typename _tokenName>
struct Rule
{
    using NonTerminatingName = size_t;
    using TokenName = _tokenName;
    using ID = size_t;

    struct Symbol
    {
        Symbol(RuleSymbolType type, NonTerminatingName nt)
            : type(type), NTName(nt), tokenName(TokenName())
        {

        }

        Symbol(RuleSymbolType type, const TokenName &tN)
            : type(type), NTName(0), tokenName(tN)
        {

        }

        RuleSymbolType type;
        NonTerminatingName NTName;
        TokenName tokenName;
    };

    ID id;
    NonTerminatingName left;
    std::vector<Symbol> syms;
};

template<typename TokenName>
bool RuleSymbolEqual(const typename Rule<TokenName>::Symbol &lhs, const typename Rule<TokenName>::Symbol &rhs) noexcept
{
    if(lhs.type != rhs.type)
        return false;
    if(lhs.type == RuleSymbolType::NonTerminate)
        return lhs.NTName == rhs.NTName;
    return lhs.tokenName == rhs.tokenName;
}

template<typename TokenName>
bool RuleEqual(const Rule<TokenName> &lhs, const Rule<TokenName> &rhs) noexcept
{
    if(lhs.left != rhs.left)
        return false;
    if(lhs.syms.size() != rhs.syms.size())
        return false;
    for(size_t i = 0; i < lhs.syms.size(); ++i)
    {
        if(!RuleSymbolEqual<TokenName>(lhs.syms[i], rhs.syms[i]))
            return false;
    }
    return true;
}

AGZ_NAMESPACE_END(AGZ)
