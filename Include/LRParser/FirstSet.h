/*================================================================
Filename: FirstSet.h
Date: 2018.4.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <set>

#include <Common/Utility.h>
#include <LRParser/RuleTable.h>

NS_BEGIN(AGZ)

template<typename _tA>
class FirstSetTable
{
public:
    using FstSet = Set<TokT<_tA>>;

    FirstSetTable(const RuleTable<_tA> &ruleTab);

    const FstSet &GetFirstSet(NTIdx NT) const;

private:
    bool UpdateFstSet(NTIdx NT, const RuleTable<_tA> &ruleTab);

    bool UpdateFstSetByRule(Ptr<Rule<_tA>> rule, FstSet &fstSet);

private:
    Vec<FstSet> fstSets_;
};

NS_END(AGZ)

#include <LRParser/FirstSet.inl>
