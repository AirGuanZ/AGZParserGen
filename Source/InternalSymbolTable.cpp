/*================================================================
Filename: InternalSymbolTable.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <set>
#include <stack>
#include <vector>

#include "InternalASTNode.h"
#include "InternalSymbolTable.h"
#include "InternalTokenizer.h"

AGZ_NAMESPACE_BEGIN(AGZ)
AGZ_NAMESPACE_BEGIN(Internal)

AGZ_LOCAL_DEFINITIONS_BEGIN

struct ScopeNode
{
    std::vector<std::string> globalName;
    std::string globalNameStr;
    WPtr<ScopeNode> parent;

    std::map<std::string, Ptr<ScopeNode>> children;
    std::set<std::string> localSymbols;
    std::set<Ptr<ASTNode_Script>> entrys;
};

void AddChildren(
    std::map<std::string, Ptr<ScopeNode>> &children,
    const std::string &childName,
    Ptr<ScopeNode> child)
{
    auto it = children.find(childName);

    if(it == children.end())
    {
        children[childName] = child;
        return;
    }

    Ptr<ScopeNode> ori = it->second;
    assert(ori->globalName == child->globalName);
    assert(ori->parent.lock() == child->parent.lock());

    for(auto chIt : child->children)
        AddChildren(ori->children, chIt.first, chIt.second);

    for(auto &localSym : child->localSymbols)
        ori->localSymbols.insert(localSym);

    for(auto entry : child->entrys)
    {
        assert(ori->entrys.find(entry) == ori->entrys.end());
        ori->entrys.insert(entry);
    }
}

Ptr<ScopeNode> MakeScopeTree(
    Ptr<ASTNode_Script> node,
    const std::vector<std::string> &globalName,
    const std::string &globalNameStr)
{
    assert(node);

    Ptr<ScopeNode> rt = MakePtr<ScopeNode>();
    rt->entrys.insert(node);

    for(auto stmt : node->stmts_)
    {
        if(stmt->startDef_)
            rt->localSymbols.insert(Keyword_Start);
        else if(stmt->namespace_)
        {
            std::vector<std::string> childGlobalName = globalName;
            childGlobalName.push_back(stmt->namespace_->name_);

            std::string childGlobalNameStr =
                (globalNameStr.empty() ? "" : globalNameStr + ".") + stmt->namespace_->name_;

            Ptr<ScopeNode> child = MakeScopeTree(
                stmt->namespace_->content_, childGlobalName, childGlobalNameStr);
            child->parent = rt;
            child->globalName = std::move(childGlobalName);
            child->globalNameStr = std::move(childGlobalNameStr);

            AddChildren(rt->children, stmt->namespace_->name_, child);
        }
        else if(stmt->import_)
        {
            Ptr<ScopeNode> importedScope = MakeScopeTree(
                stmt->import_->imported_, globalName, globalNameStr);

            for(auto it : importedScope->children)
                AddChildren(rt->children, it.first, it.second);

            for(auto lS : importedScope->localSymbols)
                rt->localSymbols.insert(lS);
        }
        else
        {
            assert(stmt->rule_);
            rt->localSymbols.insert(stmt->rule_->id_);
        }
    }
    return rt;
}

bool FindReference(
    const std::vector<Ptr<ScopeNode>> &context,
    const Ptr<ASTNode_Symbol> sym,
    RawRule::Symbol &result)
{
    if(sym->token_.size())
    {
        result.type = RuleSymbolType::Token;
        result.name = sym->token_;
        return true;
    }

    if(context.empty())
        return false;

    assert(sym->ids_.size());
    auto ctx = context.back();
    size_t symScopeIdxEnd = sym->ids_.size() - 1;

    for(size_t symScopeIdx = 0; symScopeIdx < symScopeIdxEnd; ++symScopeIdx)
    {
        const std::string &scopeName = sym->ids_[symScopeIdx];
        while(true)
        {
            auto it = ctx->children.find(scopeName);
            if(it != ctx->children.end())
            {
                ctx = it->second;
                break;
            }

            if(ctx->parent.use_count())
            {
                ctx = ctx->parent.lock();
                continue;
            }

            return false;
        }
    }

    assert(ctx);

    if(symScopeIdxEnd > 0)
    {
        if(ctx->localSymbols.find(sym->ids_.back()) != ctx->localSymbols.end())
        {
            result.type = RuleSymbolType::NonTerminate;
            result.name = (ctx->globalNameStr.empty() ? "" : ctx->globalNameStr + ".")
                           + sym->ids_.back();
            return true;
        }
    }
    else
    {
        while(true)
        {
            if(ctx->localSymbols.find(sym->ids_.back()) != ctx->localSymbols.end())
            {
                result.type = RuleSymbolType::NonTerminate;
                result.name = (ctx->globalNameStr.empty() ? "" : ctx->globalNameStr + ".")
                               + sym->ids_.back();
                return true;
            }

            if(ctx->parent.use_count())
            {
                ctx = ctx->parent.lock();
                continue;
            }

            return false;
        }
    }

    return false;
}

void GlobalizeSymbols(
    Ptr<ScopeNode> scope,
    std::vector<Ptr<ScopeNode>> &context,
    RawSymbolTable &symbolTable)
{
    assert(scope);

    std::vector<Ptr<ASTNode_Symbol>> startDefRight;

    for(Ptr<ASTNode_Script> scopeEntry : scope->entrys)
    {
        for(Ptr<ASTNode_Statement> stmt : scopeEntry->stmts_)
        {
            const std::string *ruleLeft = nullptr;
            const std::vector<Ptr<ASTNode_Symbol>> *ruleRight = nullptr;
            int ruleLine = 0;
            const std::string *ruleFilename = nullptr;

            if(stmt->startDef_)
            {
                startDefRight = { stmt->startDef_->sym_ };
                ruleLeft     = &Keyword_Start;
                ruleRight    = &startDefRight;
                ruleLine     = stmt->startDef_->line_;
                ruleFilename = &stmt->startDef_->filename_;
            }
            else if(stmt->rule_)
            {
                ruleLeft     = &stmt->rule_->id_;
                ruleRight    = &stmt->rule_->syms_;
                ruleLine     = stmt->rule_->line_;
                ruleFilename = &stmt->rule_->filename_;
            }

            if(ruleLeft)
            {
                assert(ruleRight);

                std::string globalLeft =
                    (context.back()->globalNameStr.empty() ? "" : context.back()->globalNameStr + ".")
                    + *ruleLeft;

                std::vector<RawRule::Symbol> ruleSymbols(ruleRight->size());
                for(size_t i = 0; i < ruleRight->size(); ++i)
                {
                    if(!FindReference(context, (*ruleRight)[i], ruleSymbols[i]))
                    {
                        assert(ruleFilename);
                        throw SymbolTableException(
                            "Symbol reference(s) for rule " + globalLeft + " not found", ruleLine, *ruleFilename);
                    }
                }

                symbolTable.rules.insert(
                    std::make_pair(globalLeft, RawRule{ globalLeft, std::move(ruleSymbols) }));
            }
        }
    }

    //´¦Àísubscope
    for(auto it : scope->children)
    {
        context.push_back(it.second);
        GlobalizeSymbols(it.second, context, symbolTable);
        context.pop_back();
    }
}

AGZ_LOCAL_DEFINITIONS_END

Ptr<RawSymbolTable> SymbolTableBuilder::Build(Ptr<ASTNode_Script> root) const
{
    assert(root);

    Ptr<ScopeNode> scopeTree = MakeScopeTree(root, { }, "");
    Ptr<RawSymbolTable> globalizedSymbolTable = MakePtr<RawSymbolTable>();
    std::vector<Ptr<ScopeNode>> context = { scopeTree };
    GlobalizeSymbols(scopeTree, context, *globalizedSymbolTable);

    return globalizedSymbolTable;
}

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
