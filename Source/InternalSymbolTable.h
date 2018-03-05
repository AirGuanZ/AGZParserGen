/*================================================================
Filename: InternalSymbolTable.h
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <unordered_map>

#include "Common.h"
#include "Exception.h"
#include "Rule.h"

AGZ_NAMESPACE_BEGIN(AGZ)
AGZ_NAMESPACE_BEGIN(Internal)

class ASTNode_Script;

struct RawSymbolTable
{
    std::unordered_multimap<std::string, RawRule> rules;
};

class SymbolTableException : public Exception
{
public:
    SymbolTableException(const std::string &msg, int line, const std::string &filename)
        : Exception(msg), line_(line), filename_(filename)
    {

    }

    int line_;
    std::string filename_;
};

class SymbolTableBuilder
{
public:
    Ptr<RawSymbolTable> Build(Ptr<ASTNode_Script> root) const;
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
