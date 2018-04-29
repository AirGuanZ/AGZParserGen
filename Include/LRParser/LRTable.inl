#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)

template<typename _tA>
inline void LRTable<_tA>::Build(const RuleTable<_tA> &ruleTab,
                                const LRItemSetConstructor<_tA> &sets,
                                _tA &tA)
{
    size_t startIdx = ruleTab.Trans(MetaLang::GLOBAL_NAMESPACE_STR() + "." +
                                    MetaLang::KEYWORD_STR_START());
    TokT<_tA> endmarkType = ToToken(tA, MetaLang::TOKEN_STR_ENDMARK());

    for(size_t i = 0;i != sets.idx2Set_.size(); ++i)
    {
        const Ptr<LRSet<_tA>> CCi = sets.idx2Set_[i];
        for(const LRItem<_tA> &item : *CCi)
        {
            if(IsDotFrontToken(item))
            {
                auto transInput = typename LRItemSetConstructor<_tA>::TransInput
                    { i, item.rule->right[item.dotPos] };
                auto it = sets.trans_.find(transInput);
                assert(it != sets.trans_.end());

                action_[{ i, item.rule->right[item.dotPos].tok }] =
                    TransOperation{ TransOperation::Shift, it->second };
            }
            else if(item.dotPos >= item.rule->right.size())
            {
                action_[{ i, item.lookAhead }] =
                    TransOperation{ TransOperation::Reduce, 0, item.rule };
            }
            else if(item.rule->left == startIdx && item.lookAhead == endmarkType)
            {
                action_[{ i, endmarkType }] =
                    TransOperation{ TransOperation::Accept };
            }
        }

        for(NTIdx NT = 0; NT != ruleTab.GetNTCount(); ++NT)
        {
            auto it = sets.trans_.find({ i, Sym<_tA>{ SymT::NT, NT } });
            if(it != sets.trans_.end())
                goto_[{ i, NT }] = it->second;
        }
    }
}

template<typename _tA>
inline void LRTable<_tA>::Clear(void)
{
    action_.clear();
    goto_.clear();
}

template<typename _tA>
inline bool LRTable<_tA>::ToBinaryFile(std::ofstream &fout, _tA &tA) const
{
#define WRITE_BINARY(X) \
    do { if(!WriteBinary(fout, X)) return false; } while(0)

    WRITE_BINARY(action_.size());
    for(auto &it : action_)
    {
        WRITE_BINARY(it.first.state);
        if(!tA.ToBinaryFile(fout, it.first.lookAhead))
            return false;
        
        if(!ToBinaryFile(fout, tA, it.second))
            return false;
    }

    WRITE_BINARY(goto_.size());
    for(auto &it : goto_)
    {
        WRITE_BINARY(it.first.state);
        WRITE_BINARY(it.first.lookAhead);
        WRITE_BINARY(it.second);
    }

#undef WRITE_BINARY

    return true;
}

template<typename _tA>
inline bool LRTable<_tA>::FromBinaryFile(std::ifstream &fin, _tA &tA,
                                         const RuleTable<_tA> &ruleTab)
{
    Clear();

#define READ_BINARY(X) \
    do { if(!ReadBinary(fin, X)) \
         { \
            Clear(); \
            return false; \
         } } while(0)

    typename decltype(action_)::size_type actionCount;
    READ_BINARY(actionCount);
    for(size_t i = 0;i != actionCount; ++i)
    {
        ActionTransInput input;
        TransOperation opr;

        READ_BINARY(input.state);
        if(!tA.FromBinaryFile(fin, input.lookAhead) ||
           !FromBinaryFile(fin, tA, opr, ruleTab))
        {
            Clear();
            return false;
        }

        action_[input] = opr;
    }

    typename decltype(goto_)::size_type gotoCount;
    READ_BINARY(gotoCount);
    for(size_t i = 0;i != gotoCount; ++i)
    {
        GotoTransInput input;
        size_t dst;

        READ_BINARY(input.state);
        READ_BINARY(input.lookAhead);
        READ_BINARY(dst);

        goto_[input] = dst;
    }

#undef READ_BINARY

    return true;
}

template<typename _tA>
inline bool LRTable<_tA>::IsDotFrontToken(const LRItem<_tA> &item) const
{
    if(item.dotPos >= item.rule->right.size())
        return false;
    return item.rule->right[item.dotPos].type == SymT::Token;
}

template<typename _tA>
inline bool LRTable<_tA>::ToBinaryFile(std::ofstream &fout, _tA &tA,
                                       const TransOperation &opr) const
{
    if(!WriteBinary(fout, opr.type))
        return false;
    if(opr.type == TransOperation::Shift)
        return WriteBinary(fout, opr.shiftDst);
    else if(opr.type == TransOperation::Reduce)
    {
        if(!WriteBinary(fout, opr.reduceRule->left))
            return false;
        if(!WriteString(fout, opr.reduceRule->name))
            return false;
    }
    return true;
}

template<typename _tA>
inline bool LRTable<_tA>::FromBinaryFile(std::ifstream &fin, _tA &tA,
                                         TransOperation &opr,
                                         const RuleTable<_tA> &ruleTab)
{
    if(!ReadBinary(fin, opr.type))
        return false;
    if(opr.type == TransOperation::Shift)
        return ReadBinary(fin, opr.shiftDst);
    else if(opr.type == TransOperation::Reduce)
    {
        NTIdx left; String name;
        if(!ReadBinary(fin, left) || !ReadString(fin, name))
            return false;
        opr.reduceRule = ruleTab.GetRuleByLeftAndName(left, name);
        if(!opr.reduceRule)
            return false;
    }
    return true;
}

template<typename _tA>
inline size_t LRTable<_tA>::ActionTransInput::Hasher::operator()(
        const typename LRTable<_tA>::ActionTransInput &x) const
{
    return std::hash<size_t>()(
            (std::hash<size_t>()(x.state) ^
            std::hash<TokT<_tA>>()(x.lookAhead)) >> 2);
}

template<typename _tA>
inline bool LRTable<_tA>::ActionTransInput::Equal::operator()(
                            const ActionTransInput &L,
                            const ActionTransInput &R) const
{
    return L.state == R.state && L.lookAhead == R.lookAhead;
}

template<typename _tA>
inline size_t LRTable<_tA>::GotoTransInput::Hasher::operator()(
        const typename LRTable<_tA>::GotoTransInput &x) const
{
    return std::hash<size_t>()(
            (std::hash<size_t>()(x.state) ^
            std::hash<NTIdx>()(x.lookAhead)) >> 2);
}

template<typename _tA>
inline bool LRTable<_tA>::GotoTransInput::Equal::operator()(
                            const GotoTransInput &L,
                            const GotoTransInput &R) const
{
    return L.state == R.state && L.lookAhead == R.lookAhead;
}

NS_END(AGZ)
