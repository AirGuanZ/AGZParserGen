/*================================================================
Filename: MetaLang/AST.h
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#pragma once

/*
    元语言AST

    Script    := Statement*

    Statement := Namespace | Start | Import | Rule
    Namespace := "namespace" Identifier "{" Script "}"
    Start     := "AGZStart" := Symbol
    Import    := "import" Path
    Rule      := Identifier ":=" Symbol(+Symbol)*

    Symbol    := Token | Identifier(.Identifier)*
*/

#include <vector>

#include <Utility.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

struct ASTNode_Script;
struct ASTNode_Statement;
struct ASTNode_Namespace;
struct ASTNode_Start;
struct ASTNode_Import;
struct ASTNode_Rule;
struct ASTNode_Symbol;

struct ASTNode_Script
{
    Vec<Ptr<ASTNode_Statement>> stmts;
};

struct ASTNode_Statement
{
    Ptr<ASTNode_Namespace> ns;
    Ptr<ASTNode_Start>     start;
    Ptr<ASTNode_Import>    import;
    Ptr<ASTNode_Rule>      rule;
};

struct ASTNode_Namespace
{
    String name;
    Ptr<ASTNode_Script> content;
};

struct ASTNode_Start
{
    int line;
    String filename;
    Ptr<ASTNode_Symbol> sym;
};

struct ASTNode_Import
{
    String path;
    Ptr<ASTNode_Script> content;
};

struct ASTNode_Rule
{
    int line;
    String filename;
    String rulename;
    String left;
    Vec<Ptr<ASTNode_Symbol>> right;
};

struct ASTNode_Symbol
{
    String token;
    Vec<String> idens;
};

NS_END(MetaLang)
NS_END(AGZ)
