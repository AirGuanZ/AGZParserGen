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
AGZ_NAMESPACE_BEGIN(Internal)

class ParserException : public Exception
{
public:
    ParserException(const std::string &msg, int line, const std::string &filename)
        : Exception(msg), line_(line), filename_(filename)
    {

    }

    int line_;
    std::string filename_;
};

class Parser
{
public:
    Ptr<ASTNode_Script> Parse(Tokenizer &toks);
    Ptr<ASTNode_Script> ParseFromFile(const std::string &path, int line, const std::string &filename);

private:
    Ptr<ASTNode_Script> ParseScript(Tokenizer &toks);
    Ptr<ASTNode_Statement> ParseStatement(Tokenizer &toks);
    Ptr<ASTNode_Symbol> ParseSymbol(Tokenizer &toks);
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
