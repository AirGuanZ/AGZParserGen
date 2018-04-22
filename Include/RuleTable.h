/*================================================================
Filename: RuleTable.h
Date: 2018.4.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <cassert>

#include <MetaLang/RawRuleTable.h>
#include <Utility.h>

NS_BEGIN(AGZ)

using SymT = MetaLang::SymT;

/* 非终结符id */
using NTIdx = size_t;

template<typename _tA>
using TokT = typename _tA::TokenType;

template<typename _tA>
inline TokT<_tA> ToToken(_tA &tA, const String &tok);

/* 单个符号 */
template<typename _tA>
struct Sym
{
    SymT type;
    NTIdx NT;
    TokT<_tA> tok;
};

/* 单条规则 */
template<typename _tA>
struct Rule
{
    String name;
    NTIdx left;
    Vec<Sym<_tA>> right;
};

class RuleTableException
{
public:
    RuleTableException(const String &msg);
    
    const String msg;
};

template<typename _tA>
class RuleTable
{
public:
    RuleTable(void);

    void Build(const MetaLang::RawRuleTableBuilder::RawRuleTable &rawRules,
               _tA &tA);

    void Append(const MetaLang::RawRuleTableBuilder::RawRuleTable &rawRules,
                _tA &tA);

    void Clear(void);

    const String &Detrans(NTIdx NT) const;

    NTIdx Trans(const String &NT) const;

    bool HasNTDef(const String &NT) const;

    // 合法的NT index范围为[0, ruleTable.GetNTCount() - 1]
    NTIdx GetNTCount(void) const;

    const Map<String, Ptr<Rule<_tA>>> &GetRulesByLeft(NTIdx left) const;

private:
    NTIdx GetNTIdx(const String &NT);

    Sym<_tA> GetSym(const MetaLang::RawSym &sym, _tA &tA);

    String GenRuleName(void);

private:
    // nt index -> nt name
    UrdMap<NTIdx, String> idx2NT_;
    // nt name -> nt index
    UrdMap<String, NTIdx> NT2Idx_;

    // 下一个新来的nt应分配的nt index
    NTIdx nextNTIdx_;

    // rule的名字生成下标
    size_t autoRuleNameIdx_;

    // non-terminating symbol -> (rule name -> pointer to rule)
    UrdMap<NTIdx, Map<String, Ptr<Rule<_tA>>>> rules_;
};

NS_END(AGZ)

#include <RuleTable.inl>
