/*================================================================
Filename: Parser.h
Date: 2018.4.24
Created by AirGuanZ
================================================================*/
#pragma once

#include <LRParser/AST.h>
#include <LRParser/LRTable.h>
#include <LRParser/RuleTable.h>
#include <Utility.h>

NS_BEGIN(AGZ)

template<typename _tA>
class ASTCons
{
public:
    using TokenAdaptor = _tA;
    using TokenStream  = typename TokenAdaptor::TokenStream;
    using TokenType    = TokT<_tA>;
    using TokenData    = TokD<_tA>;

    ASTCons(const LRTable<_tA> &LRTab);

    Ptr<ASTNode<_tA>> Parse(TokenStream &toks,
                            const RuleTable<_tA> &ruleTab,
                            _tA &tA) const;

private:
    const LRTable<_tA> &LRTab_;
};

NS_END(AGZ)

#include <LRParser/ASTCons.inl>
