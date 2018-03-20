/*================================================================
Filename: InternalFirstSet.h
Date: 2018.3.20
Created by AirGuanZ
================================================================*/
#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <set>
#include <unordered_map>

#include "Rule.h"
#include "InternalSymbolTable.h"

AGZ_NAMESPACE_BEGIN(AGZ)
AGZ_NAMESPACE_BEGIN(Internal)

template<typename TokenName>
using FirstSet = std::set<TokenName>;

template<typename _tokenMapping>
class FirstSetTable
{
public:
    using TokenMapping = _tokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;
    using NTName = typename SpecRule::NonTerminatingName;

    //对非终结符A，考虑其产生式A -> a1 a2 ... ak
    //对任意1 <= i <= k，若满足：
    //    对任意1 <= j < i，均有aj.HasEmpty()为真
    //那么Fst(ai) \subseteq Fst(A)
    //若对任意1 <= i <= k，均有ai.HasEmpty()为真
    //那么A.HasEmpty()为真
    //不过我仔细想了想，在禁止用户写empty符号的情况下，好像没有谁的HasEmpty会为真……
    FirstSetTable(const SymbolTable<TokenMapping> &symTab)
    {
        for(typename SpecRule::NonTerminatingName i = 0;
            i < symTab.GetNonTerminatingSymbolCount();
            ++i)
        {
            fstSets_[i] = FirstSet<TokenName>();
        }

        bool setsChanging = true;
        while(setsChanging)
        {
            setsChanging = false;
            for(typename SpecRule::NonTerminatingName i = 0;
                i < symTab.GetNonTerminatingSymbolCount();
                ++i)
            {
                setsChanging |= UpdateFirstSet(i, symTab);
            }
        }
    }

    const FirstSet<TokenName> &GetFirstSet(NTName nt) const
    {
        return fstSets_[nt];
    }

private:
    bool UpdateFirstSetByName(typename SpecRule::NonTerminatingName nt, const SymbolTable<TokenMapping> &symTab)
    {
        assert(nt < symTab.GetNonTerminatingSymbolCount());
        bool rt = false;
        auto ruleRange = symTab.GetRulesByLeft(nt);
        FirstSet<TokenName> &fstSet = fstSets_[nt];
        for(auto it = ruleRange.first; it != ruleRange.second; ++it)
        {
            rt |= UpdateFirstSetByRule(*it->second, fstSet);
        }
        return rt;
    }

    bool UpdateFirstSetByRule(const SpecRule &rule, FirstSet<TokenName> &output)
    {
        assert(rule.syms.size());
        bool rt = false;
        auto &sym = rule.syms[0];

        size_t oldSize = output.size();

        if(sym.type == RuleSymbolType::Token)
            output.insert(sym.tokenName);
        else
        {
            assert(sym.type == RuleSymbolType::NonTerminate);
            for(auto &tokenName : fstSets_[sym.NTName])
                output.insert(tokenName);
        }
        
        return oldSize < output.size();
    }

private:
    std::unordered_map<typename SpecRule::NonTerminatingName, FirstSet<TokenName>> fstSets_;
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
