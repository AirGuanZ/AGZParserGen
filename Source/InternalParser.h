/*================================================================
Filename: InternalParser.h
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#pragma once

#include "Common.h"
#include "Exception.h"
#include "InternalASTNode.h"
#include "InternalTokenizer.h"

AGZ_NAMESPACE_BEGIN(AGZ)

class InternalParserException : public Exception
{
public:
    InternalParserException(const std::string &msg, int line, const String &filename)
        : Exception(msg), line_(line), filename_(filename)
    {

    }

    int line_;
    String filename_;
};

class InternalParser
{
public:
    Ptr<ASTNode_Script> Parse(InternalTokenizer &toks);
    Ptr<ASTNode_Script> ParseFromFile(const String &path, int line, const String &filename);

private:
    Ptr<ASTNode_Script> ParseScript(InternalTokenizer &toks);
    Ptr<ASTNode_Statement> ParseStatement(InternalTokenizer &toks);
    Ptr<ASTNode_Symbol> ParseSymbol(InternalTokenizer &toks);
};

AGZ_NAMESPACE_END(AGZ)
