/*================================================================
Filename: MetaLang/Parser.h
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#pragma once

#include <Common/Utility.h>
#include <MetaLang/AST.h>
#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

class ParserException
{
public:
    ParserException(const String &msg, const String &filename, int line)
        : msg(msg), filename(filename), line(line)
    {

    }

    String msg;
    String filename;
    int line;
};

class Parser
{
public:
    Ptr<ASTNode_Script> ParseFromTokens(Tokenizer &toks);
    Ptr<ASTNode_Script> ParseFromFile  (const String &dstFilename,
                                        const String &callerFilename,
                                        int callerLine);
    
private:
    Ptr<ASTNode_Script>    ParseScript   (Tokenizer &toks);
    Ptr<ASTNode_Statement> ParseStatement(Tokenizer &toks);
    Ptr<ASTNode_Symbol>    ParseSymbol   (Tokenizer &toks);
};

NS_END(MetaLang)
NS_END(AGZ)

#include <MetaLang/Parser.inl>
