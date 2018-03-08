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

struct RawSymbolTable
{
    std::unordered_multimap<std::string, RawRule> rules;
};

class SymbolTableBuilder
{
public:
    //给定一棵AST，提取出所有规则，其中涉及到的名字都被展开到全局作用域中
    Ptr<RawSymbolTable> Build(Ptr<ASTNode_Script> root) const;
};

/*
    维护两个映射：
        NonTerminatingName -> Rules
        RuleID             -> Rule
*/
template<typename _TokenMapping>
class SymbolTable
{
public:
    using TokenMapping = _TokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

    SymbolTable(const TokenMapping &tokenMapping, const RawSymbolTable &rawSymTab)
    {
        for(auto &it : rawSymTab.rules)
        {
            const RawRule &rawRule = it.second;
            typename SpecRule::NonTerminatingName left = AddRawNonTernimatingSymbol(rawRule.left);

            std::vector<typename SpecRule::Symbol> right(rawRule.syms.size());
            for(size_t i = 0; i < rawRule.syms.size(); ++i)
                right[i] = AddRawSymbol(tokenMapping, rawRule.syms[i]);

            Ptr<SpecRule> newRule = MakePtr<SpecRule>();
            newRule->id    = leftToRules.size();
            newRule->left  = left;
            newRule->syms  = std::move(right);

            leftToRules.insert(std::make_pair(left, newRule));
            IDToRules.insert(std::make_pair(newRule->id, newRule));
        }
    }

    size_t GetOriginNonTerminatingSymbolCount(void) const
    {
        return transTable_.size();
    }

    const Ptr<SpecRule> GetRuleByID(typename SpecRule::ID id)
    {
        auto it = IDToRules.find(id);
        if(it == IDToRules.end())
            return Ptr<SpecRule>();
        return it->second;
    }

    auto GetRulesByLeft(typename SpecRule::NonTerminatingName left)
    {
        return leftToRules.equal_range(left);
    }

    void AddRule(Ptr<SpecRule> newRule)
    {
        assert(newRule && newRule->syms.size());
        auto range = GetRulesByLeft(newRule->left);
        for(auto it = range.first; it != range.second; ++it)
        {
            if(RuleEqual(*it->second, *newRule))
                return;
        }
        
        typename SpecRule::ID newID = leftToRules.size();
        newRule->id = newID;
        leftToRules.insert(std::make_pair(newRule->left, newRule));
        IDToRules.insert(std::make_pair(newID, newRule));
    }

    void EraseRule(Ptr<SpecRule> rule)
    {
        assert(rule);

        auto it = IDToRules.find(rule->id);
        if(it == IDToRules.end())
            return;

        IDToRules.erase(it);

        auto range = GetRulesByLeft(rule->left);
        for(auto it = range.first; it != range.second; ++it)
        {
            if(it->second->id == rule->id)
            {
                leftToRules.erase(it);
                break;
            }
        }
    }

private:
    typename SpecRule::NonTerminatingName AddRawNonTernimatingSymbol(const std::string &sym)
    {
        auto it = transTable_.find(sym);
        if(it != transTable_.end())
            return it->second;

        typename SpecRule::NonTerminatingName newName = transTable_.size();
        transTable_[sym]        = newName;
        invTransTable_[newName] = sym;

        return newName;
    }

    typename SpecRule::Symbol AddRawSymbol(const TokenMapping &tokenMapping, const RawRule::Symbol &sym)
    {
        if(sym.type == RuleSymbolType::NonTerminate)
        {
            typename SpecRule::Symbol rt;
            rt.type = RuleSymbolType::NonTerminate;
            rt.NTName = AddRawNonTernimatingSymbol(sym.name);
            return rt;
        }

        typename SpecRule::Symbol rt;
        rt.type = RuleSymbolType::Token;
        rt.tokenName = tokenMapping(sym.name);
        return rt;
    }

private:
    //用来进行non-ternimating symbol在RawSymbol和Symbol之间的互转的表格

    std::unordered_map<std::string, typename SpecRule::NonTerminatingName> transTable_;
    std::unordered_map<typename SpecRule::NonTerminatingName, std::string> invTransTable_;

    std::unordered_multimap<typename SpecRule::NonTerminatingName, Ptr<SpecRule>> leftToRules;
    std::unordered_map<typename SpecRule::ID, Ptr<SpecRule>> IDToRules;
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
