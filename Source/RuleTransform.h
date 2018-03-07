/*================================================================
Filename: RuleTransform.h
Date: 2018.3.6
Created by AirGuanZ
================================================================*/
#pragma once

/*
    ��ݹ��滻�ı任����任��
        ���任��A    := A b | c
             => A    := c rest | c
                rest := b | b rest
        ��任��T[ ]{ A(c) }            = A(c)
                T[ ]{ A(c, <rest>) }    = T[c]{ <rest> }
                T[L]{ rest(b, <rest>) } = T[A(L, b)]{ <rest> }
                T[L]{ rest(b) }         = A(L, b)
    ��������ȡ�ı任����任��
        ���任��A    := a B | a C
             => A    := a rest
                rest := B | C
        ��任��T[ ]{ A(a, <rest>) } = T[a]{ <rest> }
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
    ��ݹ��滻�������������Ĳ���ʽ�ṹ��
        Ai := Aj alpha
        Aj := w
    ����ĵ�һ������ʽ���任Ϊ��
        Ai := w alpha
    ������Ź��򱻼�¼ΪruleID_��Aj := w�򱻼�¼ΪsubruleID_��
    ����AST�任ʱ�������²�����
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
