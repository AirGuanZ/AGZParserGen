/*================================================================
Filename: ASTNode.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <memory>

#include <Common.h>
#include <Rule.h>

AGZ_NAMESPACE_BEGIN(AGZ)

class ASTNode;
class ASTNode_Script;
class ASTNode_Statement;
class ASTNode_Namespace;
class ASTNode_StartDefinition;
class ASTNode_Import;
class ASTNode_Rule;
class ASTNode_Symbol;
class ASTNode_Token;

class ASTNode
{
public:
    virtual void FetchRules(std::vector<Rule> &rules,
                            const std::string &namePrefix = "") const = 0;
};

class ASTNode_Script : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;
    
private:
    std::vector<Ptr<ASTNode_Statement>> stmts_;
};

class ASTNode_Statement : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

private:
    Ptr<ASTNode_StartDefinition> startDef_;
    Ptr<ASTNode_Import>          import_;
    Ptr<ASTNode_Rule>            rule_;
};

class ASTNode_Namespace : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

private:
    String name_;
    Ptr<ASTNode_Script> content_;
};

class ASTNode_StartDefinition : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

private:
    String id_;
};

class ASTNode_Import : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

private:
    String path_;
    Ptr<ASTNode_Script> imported_;
};

class ASTNode_Rule : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

private:
    String id_;
    std::vector<Ptr<ASTNode_Symbol>> syms_;
};

class ASTNode_Symbol : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

    Rule::Symbol FetchSymbol(void) const;

private:
    Ptr<ASTNode_Token> token_;
    String id_;
};

class ASTNode_Token : public ASTNode
{
public:
    void FetchRules(std::vector<Rule> &rules,
                    const std::string &namePrefix = "") const;

    const String &GetToken(void) const;

private:
    String token_;
};

AGZ_NAMESPACE_END(AGZ)
