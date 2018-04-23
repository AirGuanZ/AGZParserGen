/*================================================================
Filename: LRTable.h
Date: 2018.4.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <LRItem.h>
#include <Utility.h>

NS_BEGIN(AGZ)

template<typename _tA>
class LRTable
{
public:
    void Build(const RuleTable<_tA> &ruleTab,
               const LRItemSetConstructor<_tA> &sets,
               _tA &tA);

private:
    bool IsDotFrontToken(const LRItem<_tA> &item) const;

private:
    struct ActionTransInput
    {
        size_t state;
        TokT<_tA> lookAhead;

        struct Hasher
        {
            size_t operator()(const ActionTransInput &x) const;
        };

        struct Equal
        {
            bool operator()(const ActionTransInput &L,
                            const ActionTransInput &R) const;
        };
    };

    struct GotoTransInput
    {
        size_t state;
        NTIdx lookAhead;

        struct Hasher
        {
            size_t operator()(const GotoTransInput &x) const;
        };

        struct Equal
        {
            bool operator()(const GotoTransInput &L,
                            const GotoTransInput &R) const;
        };
    };

    struct TransOperation
    {
        enum
        {
            Shift,
            Reduce,
            Accept
        } type;

        size_t shiftDst;
        Ptr<Rule<_tA>> reduceRule;
    };

    UrdMap<ActionTransInput, TransOperation,
           typename ActionTransInput::Hasher,
           typename ActionTransInput::Equal> action_;
    UrdMap<GotoTransInput, size_t,
           typename GotoTransInput::Hasher,
           typename GotoTransInput::Equal> goto_;
};

NS_END(AGZ)

#include <LRTable.inl>
