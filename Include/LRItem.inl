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
void LRItemSetConstructor<_tA>::Closure(LRSet<_tA> &itemSet,
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
                    newItems.insert({ r, 0, rule->lookAhead });
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

NS_END(AGZ)
