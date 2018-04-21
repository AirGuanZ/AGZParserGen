/*================================================================
Filename: LRItem.h
Date: 2018.4.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <FirstSet.h>
#include <RuleTable.h>
#include <Utility.h>

NS_BEGIN(AGZ)

template<typename _tA>
struct LRItem
{
    Ptr<Rule<_tA>> rule;
    size_t dotPos;
    TokT<_tA> lookAhead;
};

template<typename _tA>
inline bool operator<(const LRItem<_tA> &L, const LRItem<_tA> &R);

template<typename _tA>
using LRSet = Set<LRItem<_tA>>;

template<typename _tA>
class LRItemSetConstructor
{
public:
    void Closure(LRSet<_tA> &itemSet,
                 const RuleTable<_tA> &ruleTab,
                 const FirstSetTable<_tA> &fstSets) const;
};

NS_END(AGZ)

#include <LRItem.inl>
