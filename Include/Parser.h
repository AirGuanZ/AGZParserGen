/*================================================================
Filename: Parser.h
Date: 2018.4.25
Created by AirGuanZ
================================================================*/
#pragma once

#include <Common/Utility.h>
#include <LRParser/ASTCons.h>
#include <LRParser/LRTable.h>

NS_BEGIN(AGZ)

class Exception
{
public:
    Exception(const String &msg);
    String msg;
};

class FileException : public Exception
{
public:
    FileException(const String &msg, const String &filename);

    String filename;
};

class MetaLangException : public Exception
{
public:
    MetaLangException(const String &filename, int line,
                      const String &msg);
    String filename;
    int line;
};

class LRBuildingException : public Exception
{
public:
    LRBuildingException(const String &msg);
};

class SymbolException : public Exception
{
public:
    SymbolException(const String &msg);
};

template<typename _tA>
class Parser
{
public:
    using TokenAdaptor = _tA;
    using TokenStream  = typename _tA::TokenStream;
    using TokenType    = TokT<_tA>;
    using TokenData    = TokD<_tA>;

    void BuildFromSource(TokenAdaptor &tA, const String &src,
                                           const String &srcFilename);

    void BuildFromSourceFile(TokenAdaptor &tA, const String &filename);

    void Clear(void);

    Ptr<ASTNode<_tA>> Parse(TokenAdaptor &tA,
                            TokenStream &toks) const;
    
    NTIdx NTName2Idx(const String &name) const;

    const String &NTIdx2Name(NTIdx idx) const;

    bool ToBinaryFile(const String &filename, _tA &tA) const;

    bool FromBinaryFile(const String &filename, _tA &tA);

private:
    RuleTable<_tA> ruleTab_;
    LRTable<_tA> LRTab_;
};

NS_END(AGZ)

#include <LRParser/Parser.inl>
