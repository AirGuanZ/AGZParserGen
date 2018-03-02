/*================================================================
Filename: ASTNode.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <ASTNode.h>
#include <Rule.h>

AGZ_NAMESPACE_BEGIN(AGZ)

void ASTNode_Script::FetchRules(std::vector<Rule> &rules,
                                const std::string &namePrefix) const
{
    for(auto &stmt : stmts_)
        stmt->FetchRules(rules, namePrefix);
}

void ASTNode_Statement::FetchRules(std::vector<Rule> &rules,
                                   const std::string &namePrefix) const
{
    if(startDef_)
        startDef_->FetchRules(rules, namePrefix);
    else if(import_)
        import_->FetchRules(rules, namePrefix);
    else
    {
        assert(rule_);
        rule_->FetchRules(rules, namePrefix);
    }
}

void ASTNode_Namespace::FetchRules(std::vector<Rule> &rules,
                                   const std::string &namePrefix) const
{
    assert(name_.size());
    content_->FetchRules(rules, namePrefix + "::" + name_);
}

void ASTNode_StartDefinition::FetchRules(std::vector<Rule> &rules,
                                         const std::string &namePrefix) const
{
    //do nothing
}

void ASTNode_Import::FetchRules(std::vector<Rule> &rules,
                                const std::string &namePrefix) const
{
    assert(imported_);
    imported_->FetchRules(rules, namePrefix);
}

void ASTNode_Rule::FetchRules(std::vector<Rule> &rules,
                              const std::string &namePrefix) const
{
    Rule rule;
    rule.left = namePrefix + id_;
    for(auto &sym : syms_)
        rule.syms.push_back(sym->FetchSymbol());
}

void ASTNode_Symbol::FetchRules(std::vector<Rule> &rules,
                                const std::string &namePrefix) const
{
    //do nothing
}

Rule::Symbol ASTNode_Symbol::FetchSymbol(void) const
{
    if(token_)
        return { Rule::Symbol::Type::Token, token_->GetToken() };
    return { Rule::Symbol::Type::NonTerminate, id_ };
}

void ASTNode_Token::FetchRules(std::vector<Rule> &rules,
                               const std::string &namePrefix) const
{
    //do nothing
}

const String &ASTNode_Token::GetToken(void) const
{
    return token_;
}

AGZ_NAMESPACE_END(AGZ)
