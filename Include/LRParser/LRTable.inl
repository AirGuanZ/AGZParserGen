#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)

template<typename _tA>
inline void LRTable<_tA>::Build(const RuleTable<_tA> &ruleTab,
                                const LRItemSetConstructor<_tA> &sets,
                                _tA &tA)
{
    size_t startIdx = ruleTab.Trans(MetaLang::GLOBAL_NAMESPACE_STR + "." +
                                    MetaLang::KEYWORD_STR_START);
    TokT<_tA> endmarkType = ToToken(tA, MetaLang::TOKEN_STR_ENDMARK);

    for(size_t i = 0;i != sets.idx2Set_.size(); ++i)
    {
        const LRSet<_tA> &CCi = sets.idx2Set_[i];
        for(const LRItem<_tA> &item : CCi)
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
inline bool LRTable<_tA>::IsDotFrontToken(const LRItem<_tA> &item) const
{
    if(item.dotPos >= item.rule->right.size())
        return false;
    return item.rule->right[item.dotPos].type == SymT::Token;
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
