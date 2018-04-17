/*================================================================
Filename: MetaLang/RuleTable.h
Date: 2018.4.17
Created by AirGuanZ
================================================================*/
#pragma once

#include <MetaLang/Scope.h>
#include <Utility.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

class RuleTableException
{
public:
    RuleTableException(const String &msg, const String &filename, int line)
        : msg(msg), filename(filename), line(line)
    {

    }

    String msg;
    String filename;
    int line;
};

enum class SymbolType
{
    Token,
    NT
};

struct RawSymbol
{
    SymbolType type;
    String sym;
};

struct RawRule
{
    String name;
    String left;
    Vec<RawSymbol> right;

    String ToString(void) const;
};

class RawRuleTableBuilder
{
public:
    using RawRuleTable = UrdMulMap<String, RawRule>;

    Ptr<RawRuleTable> Build(Ptr<ScopeNode> root) const;
};

NS_END(MetaLang)
NS_END(AGZ)
