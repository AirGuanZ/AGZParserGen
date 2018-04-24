/*================================================================
Filename: Parser.h
Date: 2018.4.24
Created by AirGuanZ
================================================================*/
#pragma once

#include <AST.h>
#include <LRTable.h>
#include <Utility.h>

NS_BEGIN(AGZ)

template<typename _tA>
class Parser
{
public:
    using TokenAdaptor = _tA;
    using TokenStream  = typename TokenAdaptor::TokenStream;
    using TokenType    = TokT<_tA>;
    using TokenData    = TokD<_tA>;

    Parser(const LRTable<_tA> &LRTab);

    Ptr<ASTNode<_tA>> Parse(TokenStream &toks, _tA &tA) const;

private:
    const LRTable<_tA> &LRTab_;
};

NS_END(AGZ)

#include <Parser.inl>
