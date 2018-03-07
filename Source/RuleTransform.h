/*================================================================
Filename: RuleTransform.h
Date: 2018.3.6
Created by AirGuanZ
================================================================*/
#pragma once

/*
    左递归替换的变换及逆变换：
        正变换：A    := A b | c
             => A    := c rest | c
                rest := b | b rest
        逆变换：T[ ]{ A(c) }            = A(c)
                T[ ]{ A(c, <rest>) }    = T[c]{ <rest> }
                T[L]{ rest(b, <rest>) } = T[A(L, b)]{ <rest> }
                T[L]{ rest(b) }         = A(L, b)
    左公因子提取的变换及逆变换：
        正变换：A    := a B | a C
             => A    := a rest
                rest := B | C
        逆变换：T[ ]{ A(a, <rest>) } = T[a]{ <rest> }
                T[L]{ rest(B) }      = A(L, B)
                T[L]{ rest(C) }      = A(L, C)
*/

#include <cassert>
#include <unordered_map>

#include "ASTNode.h"
#include "Common.h"
#include "InternalSymbolTable.h"

AGZ_NAMESPACE_BEGIN(AGZ)

template<typename _TokenMapping>
class ASTTransform
{
public:
    using TokenMapping = _TokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

    virtual Ptr<ASTNode<TokenMapping>> Transform(Ptr<ASTNode<TokenMapping>> node) const = 0;

    virtual Ptr<ASTNode<TokenMapping>> Transform(Ptr<ASTNode<TokenMapping>> param,
                                                 Ptr<ASTNode<TokenMapping>> node) const = 0;
};

/*
    左递归替换处理形如这样的产生式结构：
        Ai := Aj alpha
        Aj := w
    上面的第一条产生式被变换为：
        Ai := w alpha
    该条编号规则被记录为ruleID_，Aj := w则被记录为subruleID_。
    进行AST变换时进行如下操作：
        T[]{ Ai(w, alpha) } = Ai(T[]{ Aj(w) }, alpha)
*/
template<typename _TokenMapping>
class ASTTransform_LeftRecursiveSubstitution : public ASTTransform<_TokenMapping>
{
public:
    using TokenMapping = _TokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

    Ptr<ASTNode<TokenMapping>> Transform(Ptr<ASTNode<TokenMapping>> node) const;

    Ptr<ASTNode<TokenMapping>> Transform(Ptr<ASTNode<TokenMapping>> param,
                                         Ptr<ASTNode<TokenMapping>> node) const;

private:
    typename SpecRule::ID ruleID_;
    typename SpecRule::ID subruleID_;
};

template<typename _TokenMapping>
class RuleTransformManager
{
public:
    using TokenMapping = _TokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

    RuleTransformManager(Internal::SymbolTable<TokenMapping> &symTab)
    {

    }

private:


private:
    std::unordered_multimap<typename SpecRule::ID, Ptr<ASTTransform<TokenMapping>>>
        ASTTransforms_;
};

AGZ_NAMESPACE_END(AGZ)
