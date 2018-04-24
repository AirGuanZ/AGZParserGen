#include <cassert>

NS_BEGIN(AGZ)

template<typename _tA>
inline ASTNode<_tA>::ASTNode(const Ptr<Rule<_tA>> rule,
                             Vec<Ptr<ASTSym<_tA>>> &&children)
    : rule_(rule), children_(std::move(children))
{
    assert(rule && rule->right.size() == children_.size());
}

template<typename _tA>
inline const String &ASTNode<_tA>::GetRuleName(void) const
{
    return rule_->name;
}

template<typename _tA>
inline NTIdx ASTNode<_tA>::GetNTIdx(void) const
{
    return rule_->left;
}

template<typename _tA>
inline const size_t ASTNode<_tA>::GetChildrenCount(void) const
{
    return children_.size();
}

template<typename _tA>
inline const Ptr<ASTSym<_tA>> ASTNode<_tA>::GetChild(size_t index) const
{
    return children_.at(index);
}

NS_END(AGZ)
