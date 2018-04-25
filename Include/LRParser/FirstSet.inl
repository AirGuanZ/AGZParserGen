#include <cassert>

NS_BEGIN(AGZ)

template<typename _tA>
inline FirstSetTable<_tA>::FirstSetTable(
        const RuleTable<_tA> &ruleTab)
    : fstSets_(ruleTab.GetNTCount())
{
    bool changing = true;
    while(changing)
    {
        changing = false;
        for(NTIdx i = 0;i < ruleTab.GetNTCount(); ++i)
            changing |= UpdateFstSet(i, ruleTab);
    }
}

template<typename _tA>
inline const typename FirstSetTable<_tA>::FstSet &
FirstSetTable<_tA>::GetFirstSet(NTIdx NT) const
{
    assert(0 <= NT && NT < fstSets_.size());
    return fstSets_[static_cast<size_t>(NT)];
}

template<typename _tA>
inline bool FirstSetTable<_tA>::UpdateFstSet(
        NTIdx NT, const RuleTable<_tA> &ruleTab)
{
    assert(0 <= NT && NT < ruleTab.GetNTCount());
    bool ret = false;
    auto &nMap = ruleTab.GetRulesByLeft(NT);
    FstSet &fstSet = fstSets_[NT];
    for(auto &it : nMap)
        ret |= UpdateFstSetByRule(it.second, fstSet);
    return ret;
}

template<typename _tA>
inline bool FirstSetTable<_tA>::UpdateFstSetByRule(
        Ptr<Rule<_tA>> rule, FstSet &fstSet)
{
    Sym<_tA> &sym = rule->right[0];
    size_t oldSize = fstSet.size();

    if(sym.type == SymT::Token)
        fstSet.insert(sym.tok);
    else
    {
        for(auto &tok : GetFirstSet(sym.NT))
            fstSet.insert(tok);
    }

    return oldSize != fstSet.size();
}

NS_END(AGZ)
