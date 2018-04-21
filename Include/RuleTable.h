/*================================================================
Filename: RuleTable.h
Date: 2018.4.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <MetaLang/RawRuleTable.h>
#include <Utility.h>

NS_BEGIN(AGZ)

using SymT = MetaLang::SymT;

/* 非终结符id */
using NTIdx = size_t;

template<typename _tA>
using Tok = typename _tA::Token;

template<typename _tA>
inline Tok<_tA> ToToken(_tA &tA, const String &tok)
{
    return tA.ToToken(tok);
}

/* 单个符号 */
template<typename _tA>
struct Sym
{
    SymT type;
    NTIdx NT;
    Tok<_tA> tok;
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
    RuleTableException(const String &msg)
        : msg(msg)
    {

    }
    
    const String msg;
};

template<typename _tA>
class RuleTable
{
public:
    RuleTable(void)
        : nextNTIdx_(1), autoRuleNameIdx_(1)
    {

    }

    void Build(const MetaLang::RawRuleTableBuilder::RawRuleTable &rawRules,
               _tA &tA)
    {
        Clear();
        Append(rawRules, tA);
    }

    void Append(const MetaLang::RawRuleTableBuilder::RawRuleTable &rawRules,
                _tA &tA)
    {
        for(auto &it : rawRules)
        {
            const String &left = it.first;
            const auto &rule   = it.second;

            NTIdx newLeft = GetNTIdx(left);
            Vec<Sym<_tA>> newRight(rule.right.size());
            for(size_t i = 0;i != newRight.size(); ++i)
                newRight[i] = GetSym(rule.right[i], tA);
            
            auto newRule = MakePtr<Rule<_tA>>();
            newRule->name  = rule.name.empty() ? GenRuleName() :
                                                 rule.name;
            newRule->left  = newLeft;
            newRule->right = std::move(newRight);

            auto &nMap = rules_[newLeft];
            auto oldIt = nMap.find(newRule->name);
            if(oldIt != nMap.end())
            {
                throw RuleTableException(
                    "rule name conflict between " +
                    rule.ToString() + " and a previous rule");
            }
            else
                nMap[newRule->name] = newRule;
        }
    }

    void Clear(void)
    {
        idx2NT_.clear();
        NT2Idx_.clear();
        nextNTIdx_ = 1;
        autoRuleNameIdx_ = 1;
        rules_.clear();
    }

private:
    NTIdx GetNTIdx(const String &NT)
    {
        auto it = NT2Idx_.find(NT);
        if(it == NT2Idx_.end())
        {
            NT2Idx_[NT] = nextNTIdx_;
            return nextNTIdx_++;
        }
        return it->second;
    }

    Sym<_tA> GetSym(const MetaLang::RawSym &sym, _tA &tA)
    {
        if(sym.type == SymT::NT)
            return Sym<_tA>{ SymT::NT, GetNTIdx(sym.sym) };
        return Sym<_tA>{ SymT::Token, 0, ToToken(tA, sym.sym) };
    }

    String GenRuleName(void)
    {
        return "@rulename" + std::to_string(autoRuleNameIdx_++);
    }

private:
    // nt index -> nt name
    UrdMap<NTIdx, String> idx2NT_;
    // nt name -> nt index
    UrdMap<String, NTIdx> NT2Idx_;

    // 下一个新来的nt应分配的nt index
    NTIdx nextNTIdx_;

    // rule的自动名字生成下标
    size_t autoRuleNameIdx_;

    // non-terminating symbol -> (rule name -> pointer to rule)
    UrdMap<NTIdx, Map<String, Ptr<Rule<_tA>>>> rules_;
};

NS_END(AGZ)
