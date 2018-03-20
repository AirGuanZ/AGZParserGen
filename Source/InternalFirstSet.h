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

    //�Է��ս��A�����������ʽA -> a1 a2 ... ak
    //������1 <= i <= k�������㣺
    //    ������1 <= j < i������aj.HasEmpty()Ϊ��
    //��ôFst(ai) \subseteq Fst(A)
    //��������1 <= i <= k������ai.HasEmpty()Ϊ��
    //��ôA.HasEmpty()Ϊ��
    //��������ϸ�����룬�ڽ�ֹ�û�дempty���ŵ�����£�����û��˭��HasEmpty��Ϊ�桭��
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
