/*================================================================
Filename: RuleTable.inl
Date: 2018.4.21
Created by AirGuanZ
================================================================*/

NS_BEGIN(AGZ)

template<typename _tA>
inline TokT<_tA> ToToken(_tA &tA, const String &tok)
{
    return tA.ToToken(tok);
}

template<typename _tA>
inline bool operator<(const Sym<_tA> &L, const Sym<_tA> &R)
{
    if(L.type == SymT::Token)
    {
        if(R.type == SymT::Token)
            return L.tok < R.tok;
        return true;
    }
    
    if(R.type == SymT::NT)
        return L.NT < R.NT;
    return false;
}

template<typename _tA>
inline bool operator==(const Sym<_tA> &L, const Sym<_tA> &R)
{
    if(L.type == SymT::Token)
        return R.type == SymT::Token && L.tok == R.tok;
    return R.type == SymT::NT && L.NT == R.NT;
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
    auto it = idx2NT_.find(NT);
    if(it == idx2NT_.end())
    {
        throw RuleTableException(
            "invalid NT index " + std::to_string(NT));
    }
    return it->second;
}

template<typename _tA>
NTIdx RuleTable<_tA>::Trans(const String &NT) const
{
    auto it = NT2Idx_.find(NT);
    if(it == NT2Idx_.end())
    {
        throw RuleTableException(
            "invalid NT name " + NT);
    }
    return it->second;
}

template<typename _tA>
bool RuleTable<_tA>::HasNTDef(const String &NT) const
{
    return NT2Idx_.find(NT) != NT2Idx_.end();
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
inline bool RuleTable<_tA>::ToBinaryFile(std::ofstream &fout, _tA &tA) const
{
#define WRITE_BINARY(X) \
    do { if(!WriteBinary(fout, X)) return false; } while(0)

    // idx2NT和NT2Idx写其中一个就行，我选择idx2NT
    WRITE_BINARY(idx2NT_.size());
    for(const auto &it : idx2NT_)
    {
        WRITE_BINARY(it.first);
        if(!WriteString(fout, it.second))
            return false;
    }

    WRITE_BINARY(nextNTIdx_);
    WRITE_BINARY(autoRuleNameIdx_);

    WRITE_BINARY(rules_.size());
    for(const auto &it : rules_)
    {
        const auto &m = it.second;

        WRITE_BINARY(it.first);

        WRITE_BINARY(m.size());
        for(const auto &jt : m)
        {
            auto &rule = *jt.second;
            if(!WriteString(fout, jt.first))
                return false;
            if(!WriteString(fout, rule.name))
                return false;
            WRITE_BINARY(rule.left);
            WRITE_BINARY(rule.right.size());
            for(const Sym<_tA> &sym : rule.right)
            {
                WRITE_BINARY(sym.type);
                if(sym.type == SymT::NT)
                    WRITE_BINARY(sym.NT);
                else
                {
                    if(!tA.ToBinaryFile(fout, sym.tok))
                        return false;
                }
            }
        }
    }

#undef WRITE_BINARY

    return true;
}

template<typename _tA>
inline bool RuleTable<_tA>::FromBinaryFile(std::ifstream &fin, _tA &tA)
{
    Clear();

#define READ_BINARY(X) \
    do { if(!ReadBinary(fin, X)) \
         { \
            Clear(); \
            return false; \
         } } while(0)

    decltype(idx2NT_.size()) idx2NTSize;
    READ_BINARY(idx2NTSize);

    for(size_t i = 0;i != idx2NTSize; ++i)
    {
        NTIdx idx;
        READ_BINARY(idx);

        String str;
        if(!ReadString(fin, str))
        {
            Clear();
            return false;
        }

        idx2NT_[idx] = str;
        NT2Idx_[str] = idx;
    }

    READ_BINARY(nextNTIdx_);
    READ_BINARY(autoRuleNameIdx_);

    decltype(rules_.size()) ruleLeftCount;
    READ_BINARY(ruleLeftCount);
    for(size_t i = 0;i != ruleLeftCount; ++i)
    {
        NTIdx idx;
        READ_BINARY(idx);

        decltype(rules_[0].size()) mSize;
        READ_BINARY(mSize);

        Map<String, Ptr<Rule<_tA>>> m;
        for(size_t j = 0;j != mSize; ++j)
        {
            String str, name;
            if(!ReadString(fin, str) || !ReadString(fin, name))
            {
                Clear();
                return false;
            }

            NTIdx left;
            typename Vec<Sym<_tA>>::size_type rightSize;
            READ_BINARY(left);
            READ_BINARY(rightSize);
            Vec<Sym<_tA>> right(rightSize);
            for(size_t k = 0;k != rightSize; ++k)
            {
                READ_BINARY(right[k].type);
                if(right[k].type == SymT::NT)
                    READ_BINARY(right[k].NT);
                else if(!tA.FromBinaryFile(fin, right[k].tok))
                {
                    Clear();
                    return false;
                }
            }

            Ptr<Rule<_tA>> r = MakePtr<Rule<_tA>>();
            r->name  = name;
            r->left  = left;
            r->right = std::move(right);
            m[name]  = r;
        }

        rules_[idx] = std::move(m);
    }

#undef READ_BINARY

    return true;
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
