/*================================================================
Filename: InternalParser.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <filesystem>

#include "InternalASTNode.h"
#include "InternalParser.h"
#include "InternalTokenizer.h"

AGZ_NAMESPACE_BEGIN(AGZ)

namespace filesystem = std::experimental::filesystem;

Ptr<ASTNode_Script> InternalParser::Parse(InternalTokenizer &toks)
{
    Ptr<ASTNode_Script> rt = ParseScript(toks);
    if(toks.Current().type != InternalToken::Type::End)
    {
        throw InternalParserException(
            "unexpected token " + toks.Current().str, toks.GetLine(), toks.GetFilename());
    }
    return rt;
}

Ptr<ASTNode_Script> InternalParser::ParseFromFile(const std::string &path, int line, const std::string &filename)
{
    std::string src;
    if(!ReadFile(path, src))
    {
        throw InternalParserException(
            "Failed to open file: " + path, line, filename);
    }

    InternalTokenizer toks(src, filename);
    return Parse(toks);
}

Ptr<ASTNode_Script> InternalParser::ParseScript(InternalTokenizer &toks)
{
    std::vector<Ptr<ASTNode_Statement>> stmts;
    Ptr<ASTNode_Statement> stmt;
    while((stmt = ParseStatement(toks)))
        stmts.push_back(std::move(stmt));
    return MakePtr<ASTNode_Script>(std::move(stmts));
}

Ptr<ASTNode_Statement> InternalParser::ParseStatement(InternalTokenizer &toks)
{
    //START_DEFINITION
    //AGZ_Start := symbol;
    if(toks.Match(InternalToken::Type::Kw_Start))
    {
        if(!toks.Match(InternalToken::Type::DefinedAs))
        {
            throw InternalParserException(
                "':=' expected", toks.GetLine(), toks.GetFilename());
        }

        Ptr<ASTNode_Statement> rt =  MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_StartDefinition>(ParseSymbol(toks)));

        if(!toks.Match(InternalToken::Type::Semicolon))
        {
            throw InternalParserException(
                "';' expected", toks.GetLine(), toks.GetFilename());
        }

        return rt;
    }

    //NAMESPACE
    //namespace identifier { script }
    if(toks.Match(InternalToken::Type::Kw_Namespace))
    {
        if(toks.Current().type != InternalToken::Type::Identifier)
        {
            throw InternalParserException(
                "namespace name expected", toks.GetLine(), toks.GetFilename());
        }
        std::string name = toks.Current().str;
        toks.Next();

        if(!toks.Match(InternalToken::Type::LeftBrace))
        {
            throw InternalParserException(
                "'{' expected", toks.GetLine(), toks.GetFilename());
        }

        Ptr<ASTNode_Script> content = ParseScript(toks);

        if(!toks.Match(InternalToken::Type::RightBrace))
        {
            throw InternalParserException(
                "'}' expected", toks.GetLine(), toks.GetFilename());
        }

        return MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_Namespace>(std::move(name), std::move(content)));
    }

    //IMPORT
    //import path
    if(toks.Match(InternalToken::Type::Kw_Import))
    {
        if(toks.Current().type != InternalToken::Type::Path)
        {
            throw InternalParserException(
                "filepath excepted", toks.GetLine(), toks.GetFilename());
        }

        std::string path = toks.Current().str;
        toks.Next();
        {
            //计算正确的文件相对路径
            filesystem::path curFilePath(toks.GetFilename());
            if(curFilePath.has_parent_path())
                path = curFilePath.parent_path().string() + "/" + path;
        }
        Ptr<ASTNode_Script> content = ParseFromFile(
            path, toks.GetLine(), toks.GetFilename());

        return MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_Import>(std::move(path), std::move(content)));
    }

    //RULE
    //identifier := symbol (plus symbol)*;
    if(toks.Current().type == InternalToken::Type::Identifier)
    {
        std::string id = toks.Current().str;
        toks.Next();

        if(!toks.Match(InternalToken::Type::DefinedAs))
        {
            throw InternalParserException(
                "':=' excepted", toks.GetLine(), toks.GetFilename());
        }

        std::vector<Ptr<ASTNode_Symbol>> syms;
        do
        {
            Ptr<ASTNode_Symbol> sym = ParseSymbol(toks);
            if(!sym)
            {
                throw InternalParserException(
                    "symbol reference excepted", toks.GetLine(), toks.GetFilename());
            }
            syms.push_back(std::move(sym));
        } while(toks.Match(InternalToken::Type::Plus));

        Ptr<ASTNode_Statement> rt = MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_Rule>(std::move(id), std::move(syms)));

        if(!toks.Match(InternalToken::Type::Semicolon))
        {
            throw InternalParserException(
                "';' excepted", toks.GetLine(), toks.GetFilename());
        }

        return rt;
    }

    return Ptr<ASTNode_Statement>();
}

Ptr<ASTNode_Symbol> InternalParser::ParseSymbol(InternalTokenizer &toks)
{
    //TOKEN
    //token"identifier"
    if(toks.Match(InternalToken::Type::Kw_Token))
    {
        if(!toks.Match(InternalToken::Type::DoubleQuotation))
        {
            throw InternalParserException(
                "'\"' excepted", toks.GetLine(), toks.GetFilename());
        }

        if(toks.Current().type != InternalToken::Type::Identifier)
        {
            throw InternalParserException(
                "token name excepted", toks.GetLine(), toks.GetFilename());
        }

        std::string id = toks.Current().str;
        toks.Next();
        Ptr<ASTNode_Symbol> rt = MakePtr<ASTNode_Symbol>(std::move(id));

        if(!toks.Match(InternalToken::Type::DoubleQuotation))
        {
            throw InternalParserException(
                "'\"' excepted", toks.GetLine(), toks.GetFilename());
        }

        return rt;
    }

    //REFERENCE
    //identifier(.identifier)*
    if(toks.Current().type == InternalToken::Type::Identifier)
    {
        std::vector<std::string> ids;
        do
        {
            if(toks.Current().type != InternalToken::Type::Identifier)
            {
                throw InternalParserException(
                    "identifier excepted", toks.GetLine(), toks.GetFilename());
            }
            ids.push_back(toks.Current().str);
            toks.Next();
        } while(toks.Match(InternalToken::Type::Point));

        return MakePtr<ASTNode_Symbol>(std::move(ids));
    }

    return Ptr<ASTNode_Symbol>();
}

AGZ_NAMESPACE_END(AGZ)
