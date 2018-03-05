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
AGZ_NAMESPACE_BEGIN(Internal)

namespace filesystem = std::experimental::filesystem;

Ptr<ASTNode_Script> Parser::Parse(Tokenizer &toks)
{
    Ptr<ASTNode_Script> rt = ParseScript(toks);
    if(toks.Current().type != Token::Type::End)
    {
        throw ParserException(
            "unexpected token " + toks.Current().str, toks.GetLine(), toks.GetFilename());
    }
    return rt;
}

Ptr<ASTNode_Script> Parser::ParseFromFile(const std::string &path, int line, const std::string &filename)
{
    std::string src;
    if(!ReadFile(path, src))
    {
        throw ParserException(
            "Failed to open file: " + path, line, filename);
    }

    Tokenizer toks(src, path);
    return Parse(toks);
}

Ptr<ASTNode_Script> Parser::ParseScript(Tokenizer &toks)
{
    std::vector<Ptr<ASTNode_Statement>> stmts;
    Ptr<ASTNode_Statement> stmt;
    while((stmt = ParseStatement(toks)))
        stmts.push_back(std::move(stmt));
    return MakePtr<ASTNode_Script>(std::move(stmts));
}

Ptr<ASTNode_Statement> Parser::ParseStatement(Tokenizer &toks)
{
    //START_DEFINITION
    //AGZ_Start := symbol;
    int startDefinitionLine = toks.GetLine();
    if(toks.Match(Token::Type::Kw_Start))
    {
        if(!toks.Match(Token::Type::DefinedAs))
        {
            throw ParserException(
                "':=' expected", toks.GetLine(), toks.GetFilename());
        }

        Ptr<ASTNode_Statement> rt =  MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_StartDefinition>(
                ParseSymbol(toks), startDefinitionLine, toks.GetFilename()));

        if(!toks.Match(Token::Type::Semicolon))
        {
            throw ParserException(
                "';' expected", toks.GetLine(), toks.GetFilename());
        }

        return rt;
    }

    //NAMESPACE
    //namespace identifier { script }
    if(toks.Match(Token::Type::Kw_Namespace))
    {
        if(toks.Current().type != Token::Type::Identifier)
        {
            throw ParserException(
                "namespace name expected", toks.GetLine(), toks.GetFilename());
        }
        std::string name = toks.Current().str;
        toks.Next();

        if(!toks.Match(Token::Type::LeftBrace))
        {
            throw ParserException(
                "'{' expected", toks.GetLine(), toks.GetFilename());
        }

        Ptr<ASTNode_Script> content = ParseScript(toks);

        if(!toks.Match(Token::Type::RightBrace))
        {
            throw ParserException(
                "'}' expected", toks.GetLine(), toks.GetFilename());
        }

        return MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_Namespace>(std::move(name), std::move(content)));
    }

    //IMPORT
    //import path
    if(toks.Match(Token::Type::Kw_Import))
    {
        if(toks.Current().type != Token::Type::Path)
        {
            throw ParserException(
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
    if(toks.Current().type == Token::Type::Identifier)
    {
        std::string id = toks.Current().str;
        int idLine = toks.GetLine();
        toks.Next();

        if(!toks.Match(Token::Type::DefinedAs))
        {
            throw ParserException(
                "':=' excepted", toks.GetLine(), toks.GetFilename());
        }

        std::vector<Ptr<ASTNode_Symbol>> syms;
        do
        {
            Ptr<ASTNode_Symbol> sym = ParseSymbol(toks);
            if(!sym)
            {
                throw ParserException(
                    "symbol reference excepted", toks.GetLine(), toks.GetFilename());
            }
            syms.push_back(std::move(sym));
        } while(toks.Match(Token::Type::Plus));

        Ptr<ASTNode_Statement> rt = MakePtr<ASTNode_Statement>(
            MakePtr<ASTNode_Rule>(std::move(id), std::move(syms), idLine, toks.GetFilename()));

        if(!toks.Match(Token::Type::Semicolon))
        {
            throw ParserException(
                "';' excepted", toks.GetLine(), toks.GetFilename());
        }

        return rt;
    }

    return Ptr<ASTNode_Statement>();
}

Ptr<ASTNode_Symbol> Parser::ParseSymbol(Tokenizer &toks)
{
    //TOKEN
    //"identifier"
    if(toks.Match(Token::Type::DoubleQuotation))
    {
        if(toks.Current().type != Token::Type::Identifier)
        {
            throw ParserException(
                "token name excepted", toks.GetLine(), toks.GetFilename());
        }

        std::string id = toks.Current().str;
        toks.Next();
        Ptr<ASTNode_Symbol> rt = MakePtr<ASTNode_Symbol>(std::move(id));

        if(!toks.Match(Token::Type::DoubleQuotation))
        {
            throw ParserException(
                "'\"' excepted", toks.GetLine(), toks.GetFilename());
        }

        return rt;
    }

    //REFERENCE
    //identifier(.identifier)*
    if(toks.Current().type == Token::Type::Identifier)
    {
        std::vector<std::string> ids;
        do
        {
            if(toks.Current().type != Token::Type::Identifier)
            {
                throw ParserException(
                    "identifier excepted", toks.GetLine(), toks.GetFilename());
            }
            ids.push_back(toks.Current().str);
            toks.Next();
        } while(toks.Match(Token::Type::Point));

        return MakePtr<ASTNode_Symbol>(std::move(ids));
    }

    return Ptr<ASTNode_Symbol>();
}

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
