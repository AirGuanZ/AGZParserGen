/*================================================================
Filename: InternalSymbolTable.h
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>

#include "Common.h"
#include "Exception.h"
#include "Rule.h"

AGZ_NAMESPACE_BEGIN(AGZ)

class ASTNode_Script;

struct InternalSymbolTable
{
    std::multimap<std::string, Rule> rules;
};

class InternalSymbolTableException : public Exception
{
public:
    InternalSymbolTableException(const std::string &msg, int line, const std::string &filename)
        : Exception(msg), line_(line), filename_(filename)
    {

    }

    int line_;
    std::string filename_;
};

class InternalSymbolTableBuilder
{
public:
    Ptr<InternalSymbolTable> Build(Ptr<ASTNode_Script> root) const;
};

AGZ_NAMESPACE_END(AGZ)
