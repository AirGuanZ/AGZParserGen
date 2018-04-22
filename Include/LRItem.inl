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
    LRSet<_tA> CC0;
    NTIdx startNT = ruleTab.Trans(MetaLang::GLOBAL_NAMESPACE_STR
                                + "." + MetaLang::KEYWORD_STR_START);
    TokT<_tA> endmarkTok = ToToken(tA, MetaLang::TOKEN_STR_ENDMARK);
    for(auto &r : ruleTab.GetRulesByLeft(startNT))
        CC0.insert(LRItem<_tA>{ r.second, 0, endmarkTok });
    Closure(CC0, ruleTab, fstSets);

    // 把CC0插入状态表
    GetIndexOf(std::move(CC0));

    while(unmarkedSets_.size())
    {
        size_t idx = unmarkedSets_.front();
        unmarkedSets_.pop_front();
        const LRSet<_tA> &CCi = idx2Set_[idx];

        for(const LRItem<_tA> &item : CCi)
        {
            if(item.dotPos >= item.rule->right.size())
                continue;
            
            const Sym<_tA> &s = item.rule->right[item.dotPos + 1];
            if(s.type != SymT::Token)
                continue;

            LRSet<_tA> dst = Goto(CCi, ruleTab, fstSets, s.tok);

            TransInput transInput = { idx, s.tok };
            if(trans_.find(transInput) != trans_.end())
            {
                throw RuleTableException(
                    "translation repeated");
            }
            trans_[transInput] = GetIndexOf(std::move(dst));
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
inline LRSet<_tA> LRItemSetConstructor<_tA>::Goto(
            const LRSet<_tA> &src,
            const RuleTable<_tA> &ruleTab,
            const FirstSetTable<_tA> &fstSets,
            const TokT<_tA> &tok) const
{
    LRSet<_tA> ret;

    for(const LRItem<_tA> &item : src)
    {
        if(item.dotPos >= item.rule->right.size())
            continue;
        const auto &sym = item.rule->right[item.dotPos];
        if(sym.type == SymT::Token && sym.tok == tok)
            ret.insert({ item.rule, item.dotPos + 1, item.lookAhead });
    }

    Closure(ret, ruleTab, fstSets);
    return ret;
}

template<typename _tA>
inline size_t LRItemSetConstructor<_tA>::GetIndexOf(LRSet<_tA> &&s)
{
    auto it = set2Idx_.find(s);
    if(it != set2Idx_.end())
        return it->second;

    size_t newIdx = idx2Set_.size();
    set2Idx_[s] = newIdx;
    unmarkedSets_.push_back(newIdx);
    idx2Set_.push_back(std::move(s));

    return newIdx;
}

NS_END(AGZ)
