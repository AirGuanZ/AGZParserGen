/*================================================================
Filename: InternalLRItem.h
Date: 2018.3.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <cassert>
#include <set>

#include "InternalFirstSet.h"
#include "InternalSymbolTable.h"

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
    void Closure(LRItemSet<TokenMapping> &itemSet,
                 const SymbolTable<TokenMapping> &symTab,
                 const FirstSetTable<TokenMapping> &fstSets)
    {
        bool changing = true;

        //IMPROVE：分批次迭代优化
        while(changing)
        {
            size_t oldSize = itemSet.size();
            LRItemSet<TokenMapping> newItems;

            for(auto &item : itemSet)
            {
                auto itemRule = symTab.GetRuleByID(item.ruleID);
                assert(itemRule.get() != nullptr);

                if(item.dotPosition >= itemRule->syms.size())
                    continue;

                if(itemRule->syms[item.dotPosition].type != RuleSymbolType::NonTerminate)
                    continue;

                auto ruleRange = symTab.GetRulesByLeft(itemRule->syms[item.dotPosition].NTName);
                for(auto it = ruleRange.first; it != ruleRange.second; ++it)
                {
                    auto rule = it->second;
                    if(item.dotPosition + 1 >= itemRule->syms.size())
                    {
                        LRItem<TokenMapping> newItem;
                        newItem.ruleID = rule->id;
                        newItem.dotPosition = 0;
                        newItem.lookAhead = itemRule->lookAhead;
                        newItems.insert(newItem);
                    }
                    else if(itemRule->syms[item.dotPosition + 1].type == RuleSymbolType::Token)
                    {
                        LRItem<TokenMapping> newItem;
                        newItem.ruleID = rule->id;
                        newItem.dotPosition = 0;
                        newItem.lookAhead = itemRule->syms[item.dotPosition + 1].tokenName;
                        newItems.insert(newItem);
                    }
                    else
                    {
                        auto &fstSet = fstSets.GetFirstSet(itemRule->syms[item.dotPosition + 1].NTName);
                        for(TokenName tokenName : fstSet)
                        {
                            LRItem<TokenMapping> newItem;
                            newItem.ruleID = rule->id;
                            newItem.dotPosition = 0;
                            newItem.lookAhead = tokenName;
                            newItems.insert(newItem);
                        }
                    }
                }
            }

            for(auto &item : newItems)
                itemSet.insert(item);

            changing = itemSet.size() > oldSize;
        }
    }
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
