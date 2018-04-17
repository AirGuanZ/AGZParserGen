/*================================================================
Filename: MetaLang/RuleTable.cpp
Date: 2018.4.17
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <sstream>

#include <MetaLang/RuleTable.h>
#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

namespace
{
    bool FindReference(const Ptr<ScopeNode> context,
                       const Ptr<ASTNode_Symbol> sym,
                       RawSymbol &result)
    {
        if(sym->token.size())
        {
            result.type = SymbolType::Token;
            result.sym  = sym->token;
            return true;
        }

        assert(sym->idens.size());
        if(!context)
            return false;
        auto ctx = context;
        
        size_t symScopeIdxEnd = sym->idens.size() - 1;
        for(size_t symScopeIdx = 0; symScopeIdx < symScopeIdxEnd; ++symScopeIdx)
        {
            const String &scopeName = sym->idens[symScopeIdx];
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
            if(ctx->localSyms.find(sym->idens.back()) != ctx->localSyms.end())
            {
                result.type = SymbolType::NT;
                result.sym  = (ctx->globalNameStr.empty() ? "" : ctx->globalNameStr + ".")
                            + sym->idens.back();
                return true;
            }
        }
        else
        {
            while(true)
            {
                if(ctx->localSyms.find(sym->idens.back()) != ctx->localSyms.end())
                {
                    result.type = SymbolType::NT;
                    result.sym  = (ctx->globalNameStr.empty() ? "" : ctx->globalNameStr + ".")
                                + sym->idens.back();
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

    void Globalize(const Ptr<ScopeNode> scope,
                   const Ptr<ScopeNode> ctx,
                   RawRuleTableBuilder::RawRuleTable &symTab)
    {
        assert(scope && ctx);

        Vec<Ptr<ASTNode_Symbol>> startDefRight;

        for(Ptr<ASTNode_Script> entry : scope->entrys)
        {
            for(Ptr<ASTNode_Statement> stmt : entry->stmts)
            {
                int line = 0;
                const String *name = nullptr;
                const String *left = nullptr;
                const Vec<Ptr<ASTNode_Symbol>> *right = nullptr;
                const String *filename = nullptr;

                if(stmt->start)
                {
                    startDefRight = { stmt->start->sym };
                    line          = stmt->start->line;
                    name          = nullptr;
                    left          = &KEYWORD_STR_START;
                    right         = &startDefRight;
                    filename      = &stmt->start->filename;
                }
                else if(stmt->rule)
                {
                    line     = stmt->rule->line;
                    name     = &stmt->rule->rulename;
                    left     = &stmt->rule->left;
                    right    = &stmt->rule->right;
                    filename = &stmt->rule->filename;
                }

                if(left)
                {
                    String globalLeft =
                        (ctx->globalNameStr.empty() ? "" : ctx->globalNameStr + ".")
                      + *left;
                    
                    Vec<RawSymbol> ruleSyms(right->size());
                    for(size_t i = 0;i != right->size(); ++i)
                    {
                        if(!FindReference(ctx, (*right)[i], ruleSyms[i]))
                        {
                            throw RuleTableException(
                                "Symbol reference(s) for "
                              + (name ? *name : globalLeft)
                              + " not found", *filename, line);
                        }
                    }

                    symTab.insert({ globalLeft, RawRule{ (name ? *name : ""),
                                    globalLeft, std::move(ruleSyms) } });
                }
            }
        }

        for(auto it : scope->children)
            Globalize(it.second, it.second, symTab);
    }
}

Ptr<RawRuleTableBuilder::RawRuleTable>
RawRuleTableBuilder::Build(Ptr<ScopeNode> root) const
{
    assert(root);

    auto rt = MakePtr<RawRuleTable>();
    Globalize(root, root, *rt);

    return rt;
}

NS_END(MetaLang)
NS_END(AGZ)
