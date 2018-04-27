/*================================================================
Filename: MetaLang/RawRuleTable.h
Date: 2018.4.17
Created by AirGuanZ
================================================================*/
#pragma once

#include <Common/Utility.h>
#include <MetaLang/Scope.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

class RawRuleTableException
{
public:
    RawRuleTableException(const String &msg, const String &filename, int line)
        : msg(msg), filename(filename), line(line)
    {

    }

    const String msg;
    const String filename;
    const int line;
};

/* 符号类型 */
enum class SymT
{
    Token,  // 终结符
    NT      // 非终结符
};

struct RawSym
{
    SymT type;
    String sym;
};

struct RawRule
{
    String name;
    String left;
    Vec<RawSym> right;

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

#include <MetaLang/RawRuleTable.inl>
