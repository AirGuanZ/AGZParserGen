/*================================================================
Filename: RuleTable.inl
Date: 2018.4.21
Created by AirGuanZ
================================================================*/

NS_BEGIN(AGZ)

template<typename _tA>
inline Tok<_tA> ToToken(_tA &tA, const String &tok)
{
    return tA.ToToken(tok);
}

inline RuleTableException::RuleTableException(const String &msg)
    : msg(msg)
{

}

template<typename _tA>
inline RuleTable<_tA>::RuleTable(void)
    : nextNTIdx_(0), autoRuleNameIdx_(0)
{

}

template<typename _tA>
inline void RuleTable<_tA>::Build(const MetaLang::RawRuleTableBuilder::RawRuleTable &rawRules,
           _tA &tA)
{
    Clear();
    Append(rawRules, tA);
}

template<typename _tA>
inline void RuleTable<_tA>::Append(const MetaLang::RawRuleTableBuilder::RawRuleTable &rawRules,
            _tA &tA)
{
    for(auto &it : rawRules)
    {
        const String &left = it.first;
        const auto &rule   = it.second;
        NTIdx newLeft = GetNTIdx(left);
        Vec<Sym<_tA>> newRight(rule.right.size());
        for(size_t i = 0;i != newRight.size(); ++i)
            newRight[i] = GetSym(rule.right[i], tA);
        
        auto newRule = MakePtr<Rule<_tA>>();
        newRule->name  = rule.name.empty() ? GenRuleName() :
                                             rule.name;
        newRule->left  = newLeft;
        newRule->right = std::move(newRight);
        auto &nMap = rules_[newLeft];
        auto oldIt = nMap.find(newRule->name);
        if(oldIt != nMap.end())
        {
            throw RuleTableException(
                "rule name conflict between " +
                rule.ToString() + " and a previous rule");
        }
        else
            nMap[newRule->name] = newRule;
    }
}

template<typename _tA>
inline void RuleTable<_tA>::Clear(void)
{
    idx2NT_.clear();
    NT2Idx_.clear();
    nextNTIdx_ = 0;
    autoRuleNameIdx_ = 0;
    rules_.clear();
}

template<typename _tA>
inline const String &RuleTable<_tA>::Detrans(NTIdx NT) const
{
    static const String EMPTY_STR;
    auto it = idx2NT_.find(NT);
    return it != idx2NT_.end() ? it->second : EMPTY_STR;
}

template<typename _tA>
inline NTIdx RuleTable<_tA>::GetNTCount(void) const
{
    assert(nextNTIdx_ > 0);
    return nextNTIdx_;
}

template<typename _tA>
inline const Map<String, Ptr<Rule<_tA>>> &RuleTable<_tA>::GetRulesByLeft(NTIdx left) const
{
    auto it = rules_.find(left);
    if(it == rules_.end())
    {
        throw RuleTableException(
            "invalid non-terminating index " + std::to_string(left));
    }
    return it->second;
}

template<typename _tA>
inline NTIdx RuleTable<_tA>::GetNTIdx(const String &NT)
{
    auto it = NT2Idx_.find(NT);
    if(it == NT2Idx_.end())
    {
        NT2Idx_[NT] = nextNTIdx_;
        idx2NT_[nextNTIdx_] = NT;
        return nextNTIdx_++;
    }
    return it->second;
}

template<typename _tA>
inline Sym<_tA> RuleTable<_tA>::GetSym(const MetaLang::RawSym &sym, _tA &tA)
{
    if(sym.type == SymT::NT)
        return Sym<_tA>{ SymT::NT, GetNTIdx(sym.sym) };
    return Sym<_tA>{ SymT::Token, 0, ToToken(tA, sym.sym) };
}

template<typename _tA>
inline String RuleTable<_tA>::GenRuleName(void)
{
    return "@rulename" + std::to_string(autoRuleNameIdx_++);
}

NS_END(AGZ)