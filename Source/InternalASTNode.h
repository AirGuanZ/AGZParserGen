/*================================================================
Filename: InternalASTNode.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <cassert>
#include <map>
#include <memory>
#include <ostream>

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

class ASTNode
{
public:
    virtual void PrettyPrint(std::ostream &out,
                             const std::string &prefix,
                             const std::string &tab,
                             bool leftBracNewline,
                             bool inlineImported) const
    {  }
};

class ASTNode_Script : public ASTNode
{
public:
    ASTNode_Script(std::vector<Ptr<ASTNode_Statement>> &&stmts)
        : stmts_(stmts)
    {

    }

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

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

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

    Ptr<ASTNode_StartDefinition> startDef_;
    Ptr<ASTNode_Namespace>       namespace_;
    Ptr<ASTNode_Import>          import_;
    Ptr<ASTNode_Rule>            rule_;
};

class ASTNode_Namespace : public ASTNode
{
public:
    ASTNode_Namespace(std::string &&name, Ptr<ASTNode_Script> &&content)
        : name_(name), content_(content)
    {

    }

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

    std::string name_;
    Ptr<ASTNode_Script> content_;
};

class ASTNode_StartDefinition : public ASTNode
{
public:
    ASTNode_StartDefinition(Ptr<ASTNode_Symbol> &&sym)
        : sym_(sym)
    {

    }

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

    Ptr<ASTNode_Symbol> sym_;
};

class ASTNode_Import : public ASTNode
{
public:
    ASTNode_Import(std::string &&path, Ptr<ASTNode_Script> &&imported)
        : path_(path), imported_(imported)
    {

    }

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

    std::string path_;
    Ptr<ASTNode_Script> imported_;
};

class ASTNode_Rule : public ASTNode
{
public:
    ASTNode_Rule(std::string &&id, std::vector<Ptr<ASTNode_Symbol>> &&syms)
        : id_(id), syms_(syms)
    {

    }

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

    std::string id_;
    std::vector<Ptr<ASTNode_Symbol>> syms_;
};

class ASTNode_Symbol : public ASTNode
{
public:
    ASTNode_Symbol(std::string &&token)
        : token_(token)
    {
        assert(token_.size());
    }

    ASTNode_Symbol(std::vector<std::string> &&ids)
        : ids_(ids)
    {
        assert(ids_.size());
    }

    void PrettyPrint(std::ostream &out,
                     const std::string &prefix,
                     const std::string &tab,
                     bool leftBracNewline,
                     bool inlineImported) const;

    std::string token_;
    std::vector<std::string> ids_;
};

AGZ_NAMESPACE_END(AGZ)
