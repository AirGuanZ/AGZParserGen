/*================================================================
Filename: InternalLRItem.h
Date: 2018.3.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <set>

#include "Rule.h"

AGZ_NAMESPACE_BEGIN(AGZ)
AGZ_NAMESPACE_BEGIN(Internal)

template<typename _tokenMapping>
struct LRItem
{
    using TokenMapping = _tokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

    typename SpecRule::ID ruleID;
    size_t dotPosition;
    TokenName lookAhead;
};

template<typename _tokenMapping>
inline bool operator<(const LRItem<_tokenMapping> &lhs, const LRItem<_tokenMapping> &rhs)
{
    if(lhs.ruleID < rhs.ruleID)
        return true;
    if(lhs.ruleID > rhs.ruleID)
        return false;
    if(lhs.dotPosition < rhs.dotPosition)
        return true;
    if(lhs.dotPosition > rhs.dotPosition)
        return false;
    return lhs.lookAhead < rhs.lookAhead;
}

template<typename _tokenMapping>
using LRItemSet = std::set<LRItem<_tokenMapping>>;

template<typename _tokenMapping>
class LRItemAutomaton
{
public:
    using TokenMapping = _tokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

private:
    void Closure(LRItemSet<TokenMapping> &itemSet);
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
