﻿#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)

template<typename _tA>
inline bool operator<(const LRItem<_tA> &L, const LRItem<_tA> &R)
{
    if(L.rule < R.rule)
        return true;
    if(L.rule > R.rule)
        return false;
    if(L.dotPos < R.dotPos)
        return true;
    if(L.dotPos > R.dotPos)
        return false;
    return L.lookAhead < R.lookAhead;
}

template<typename _tA>
inline void LRItemSetConstructor<_tA>::Build(const RuleTable<_tA> &ruleTab,
                                             const FirstSetTable<_tA> &fstSets,
                                             _tA &tA)
{
    Clear();

    // 初始状态
    Ptr<LRSet<_tA>> CC0 = MakePtr<LRSet<_tA>>();
    NTIdx startNT = ruleTab.Trans(MetaLang::GLOBAL_NAMESPACE_STR()
                                + "." + MetaLang::KEYWORD_STR_START());
    TokT<_tA> endmarkTok = ToToken(tA, MetaLang::TOKEN_STR_ENDMARK());
    for(auto &r : ruleTab.GetRulesByLeft(startNT))
        CC0->insert(LRItem<_tA>{ r.second, 0, endmarkTok });
    Closure(*CC0, ruleTab, fstSets);

    // 把CC0插入状态表
    GetIndexOf(CC0);

    while(unmarkedSets_.size())
    {
        size_t idx = unmarkedSets_.front();
        unmarkedSets_.pop_front();
        const Ptr<LRSet<_tA>> CCi = idx2Set_[idx];

        for(const LRItem<_tA> &item : *CCi)
        {
            if(item.dotPos >= item.rule->right.size())
                continue;

            Ptr<LRSet<_tA>> dst;

            const Sym<_tA> &s = item.rule->right[item.dotPos];
            dst = Goto(*CCi, ruleTab, fstSets, s);

            TransInput transInput = { idx, s };

            auto it = trans_.find(transInput);
            size_t newIdx = GetIndexOf(dst);
            if(it != trans_.end() && it->second != newIdx)
            {
                throw RuleTableException(
                    "translation repeated");
            }
            trans_[transInput] = newIdx;
        }
    }
}

template<typename _tA>
inline void LRItemSetConstructor<_tA>::Clear(void)
{
    idx2Set_.clear();
    set2Idx_.clear();
    unmarkedSets_.clear();
    trans_.clear();
}

template<typename _tA>
inline void LRItemSetConstructor<_tA>::Closure(LRSet<_tA> &itemSet,
                                               const RuleTable<_tA> &ruleTab,
                                               const FirstSetTable<_tA> &fstSets) const
{
    bool changing = true;
    while(changing)
    {
        size_t oldSize = itemSet.size();
        LRSet<_tA> newItems;

        for(auto &item : itemSet)
        {
            auto rule = item.rule;

            if(item.dotPos >= rule->right.size())
                continue;
            if(rule->right[item.dotPos].type != SymT::NT)
                continue;

            auto &nMap = ruleTab.GetRulesByLeft(rule->right[item.dotPos].NT);
            for(auto &it : nMap)
            {
                auto r = it.second;
                if(item.dotPos + 1 >= rule->right.size())
                    newItems.insert({ r, 0, item.lookAhead });
                else if(rule->right[item.dotPos + 1].type == SymT::Token)
                    newItems.insert({ r, 0, rule->right[item.dotPos + 1].tok });
                else
                {
                    for(auto &tok : fstSets.GetFirstSet(
                                        rule->right[item.dotPos + 1].NT))
                        newItems.insert({ r, 0, tok });
                }
            }
        }

        for(auto &item : newItems)
            itemSet.insert(item);
        changing = itemSet.size() != oldSize;
    }
}

template<typename _tA>
inline Ptr<LRSet<_tA>> LRItemSetConstructor<_tA>::Goto(
            const LRSet<_tA> &src,
            const RuleTable<_tA> &ruleTab,
            const FirstSetTable<_tA> &fstSets,
            const Sym<_tA> &x) const
{
    Ptr<LRSet<_tA>> ret = MakePtr<LRSet<_tA>>();

    for(const LRItem<_tA> &item : src)
    {
        if(item.dotPos >= item.rule->right.size())
            continue;
        const auto &sym = item.rule->right[item.dotPos];
        if(sym == x)
            ret->insert({ item.rule, item.dotPos + 1, item.lookAhead });
    }

    Closure(*ret, ruleTab, fstSets);
    return ret;
}

template<typename _tA>
inline size_t LRItemSetConstructor<_tA>::GetIndexOf(Ptr<LRSet<_tA>> s)
{
    auto it = set2Idx_.find(*s);
    if(it != set2Idx_.end())
        return it->second;

    size_t newIdx = idx2Set_.size();
    set2Idx_[*s] = newIdx;
    unmarkedSets_.push_back(newIdx);
    idx2Set_.push_back(s);

    return newIdx;
}

NS_END(AGZ)
