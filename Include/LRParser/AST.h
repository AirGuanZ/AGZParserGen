/*================================================================
Filename: AST.h
Date: 2018.4.24
Created by AirGuanZ
================================================================*/
#pragma once

#include <LRParser/RuleTable.h>
#include <Utility.h>

NS_BEGIN(AGZ)

template<typename _tA>
using TokD = typename _tA::TokenData;

template<typename _tA>
class ASTNode;

template<typename _tA>
class Parser;

template<typename _tA>
struct ASTSym
{
    SymT type;
    Ptr<ASTNode<_tA>> NT;
    TokD<_tA> tok;
};

template<typename _tA>
class ASTNode
{
public:
    ASTNode(void) = default;
    ASTNode(const Ptr<Rule<_tA>> rule, Vec<Ptr<ASTSym<_tA>>> &&children);

    const String &GetRuleName(void) const;
    NTIdx GetNTIdx(void) const;

    const size_t GetChildrenCount(void) const;
    const Ptr<ASTSym<_tA>> GetChild(size_t index) const;

private:
    const Ptr<Rule<_tA>> rule_;
    const Vec<Ptr<ASTSym<_tA>>> children_;
};

NS_END(AGZ)

#include <LRParser/AST.inl>
