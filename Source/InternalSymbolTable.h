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

class SymbolTable
{
public:
    SymbolTable(const RawSymbolTable &rawSymTab);

private:
    Rule::Name AddRawNonTernimatingSymbol(const std::string &sym);
    Rule::Symbol AddRawSymbol(const RawRule::Symbol &sym);

private:
    //用来进行RawSymbol和Symbol之间的互转的表格

    //对non-ternimating symbol，索引字符串是其名字
    //对token symbol，索引字符串是# + 其名字
    std::unordered_map<std::string, Rule::Name> transTable_;

    std::unordered_map<Rule::Name, RawRule::Symbol> invTransTable_;

private:
    std::unordered_multimap<Rule::Name, Rule> rules_;
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
