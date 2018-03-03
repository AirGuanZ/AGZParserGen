/*================================================================
Filename: InternalASTNode.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <memory>

#include "Common.h"
#include "Rule.h"

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

};

class ASTNode_Script : public ASTNode
{
public:
    ASTNode_Script(std::vector<Ptr<ASTNode_Statement>> &&stmts)
        : stmts_(stmts)
    {

    }
    std::vector<Ptr<ASTNode_Statement>> stmts_;
};

class ASTNode_Statement : public ASTNode
{
public:
    ASTNode_Statement(Ptr<ASTNode_StartDefinition> &&startDef)
        : startDef_(startDef)
    {

    }

    ASTNode_Statement(Ptr<ASTNode_Namespace> &&_namespace)
        : namespace_(_namespace)
    {

    }

    ASTNode_Statement(Ptr<ASTNode_Import> &&importStmt)
        : import_(importStmt)
    {

    }

    ASTNode_Statement(Ptr<ASTNode_Rule> &&rule)
        : rule_(rule)
    {

    }

    Ptr<ASTNode_StartDefinition> startDef_;
    Ptr<ASTNode_Namespace>       namespace_;
    Ptr<ASTNode_Import>          import_;
    Ptr<ASTNode_Rule>            rule_;
};

class ASTNode_Namespace : public ASTNode
{
public:
    ASTNode_Namespace(String &&name, Ptr<ASTNode_Script> &&content)
        : name_(name), content_(content)
    {

    }

    String name_;
    Ptr<ASTNode_Script> content_;
};

class ASTNode_StartDefinition : public ASTNode
{
public:
    ASTNode_StartDefinition(Ptr<ASTNode_Symbol> &&sym)
        : sym_(sym)
    {

    }

    Ptr<ASTNode_Symbol> sym_;
};

class ASTNode_Import : public ASTNode
{
public:
    ASTNode_Import(String &&path, Ptr<ASTNode_Script> &&imported)
        : path_(path), imported_(imported)
    {

    }

    String path_;
    Ptr<ASTNode_Script> imported_;
};

class ASTNode_Rule : public ASTNode
{
public:
    ASTNode_Rule(String &&id, std::vector<Ptr<ASTNode_Symbol>> &&syms)
        : id_(id), syms_(syms)
    {

    }

    String id_;
    std::vector<Ptr<ASTNode_Symbol>> syms_;
};

class ASTNode_Symbol : public ASTNode
{
public:
    ASTNode_Symbol(String &&token)
        : token_(token)
    {

    }

    ASTNode_Symbol(std::vector<String> &&ids)
        : ids_(ids)
    {

    }

    String token_;
    std::vector<String> ids_;
};

AGZ_NAMESPACE_END(AGZ)
