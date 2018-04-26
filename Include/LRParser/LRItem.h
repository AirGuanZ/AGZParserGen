/*================================================================
Filename: LRItem.h
Date: 2018.4.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <Common/Utility.h>
#include <LRParser/FirstSet.h>
#include <LRParser/RuleTable.h>

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
    void Build(const RuleTable<_tA> &ruleTab,
               const FirstSetTable<_tA> &fstSets,
               _tA &tA);

    void Clear(void);

private:
    void Closure(LRSet<_tA> &itemSet,
                 const RuleTable<_tA> &ruleTab,
                 const FirstSetTable<_tA> &fstSets) const;
    
    Ptr<LRSet<_tA>> Goto(const LRSet<_tA> &src,
                         const RuleTable<_tA> &ruleTab,
                         const FirstSetTable<_tA> &fstSets,
                         const Sym<_tA> &x) const;

    size_t GetIndexOf(Ptr<LRSet<_tA>> s);

public:
    Vec<Ptr<LRSet<_tA>>> idx2Set_;
    Map<LRSet<_tA>, size_t> set2Idx_;

    struct TransInput
    {
        size_t setIdx;
        Sym<_tA> sym;
    };

    class CompareTransInput
    {
    public:
        bool operator()(const TransInput &L, const TransInput &R) const
        {
            if(L.setIdx < R.setIdx)
                return true;
            if(L.setIdx > R.setIdx)
                return false;
            return L.sym < R.sym;
        }
    };

    Map<TransInput, size_t, CompareTransInput> trans_;

private:
    Deq<size_t> unmarkedSets_;
};

NS_END(AGZ)

#include <LRParser/LRItem.inl>
