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
    //给定一棵AST，提取出所有规则，其中涉及到的名字都被展开到全局作用域中
    Ptr<RawSymbolTable> Build(Ptr<ASTNode_Script> root) const;
};

template<typename _TokenMapping>
class SymbolTable
{
public:
    using TokenMapping = _TokenMapping;
    using TokenName    = typename TokenMapping::TokenName;
    using SpecRule     = Rule<TokenName>;
    using Symbol       = typename SpecRule::Symbol;
    using Name         = typename SpecRule::Name;

    SymbolTable(const TokenMapping &tokenMapping, const RawSymbolTable &rawSymTab)
    {
        for(auto &it : rawSymTab.rules)
        {
            const RawRule &rawRule = it.second;
            Name left = AddRawNonTernimatingSymbol(rawRule.left);

            std::vector<Symbol> right(rawRule.syms.size());
            for(size_t i = 0; i < rawRule.syms.size(); ++i)
                right[i] = AddRawSymbol(tokenMapping, rawRule.syms[i]);

            rules.insert(std::make_pair(left, Rule{ left, std::move(right) }));
        }
    }

private:
    Name AddRawNonTernimatingSymbol(const std::string &sym)
    {
        auto it = transTable_.find(sym);
        if(it != transTable_.end())
            return it->second;

        Name newName = transTable_.size();
        transTable_[sym]        = newName;
        invTransTable_[newName] = sym;

        return newName;
    }

    Symbol AddRawSymbol(const TokenMapping &tokenMapping, const RawRule::Symbol &sym)
    {
        if(sym.type == RuleSymbolType::NonTerminate)
        {
            Symbol rt;
            rt.type = RuleSymbolType::NonTerminate;
            rt.NTName = AddRawNonTernimatingSymbol(sym.name);
            return rt;
        }

        Symbol rt;
        rt.type = RuleSymbolType::Token;
        rt.tokenName = tokenMapping(sym.name);
        return rt;
    }

private:
    //用来进行non-ternimating symbol在RawSymbol和Symbol之间的互转的表格

    std::unordered_map<std::string, Name> transTable_;
    std::unordered_map<Name, std::string> invTransTable_;

public:
    using RuleTable = std::unordered_map<Name, SpecRule>;

    RuleTable rules;
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
