/*================================================================
Filename: MetaLang/Parser.cpp
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#include <MetaLang/Parser.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

namespace
{
    bool GetParentPath(const String &path, String &parentPath)
    {
        size_t i = path.rfind('/');
        if(i == String::npos)
            return false;
        parentPath = path.substr(0, i);
        return true;
    }
}

Ptr<ASTNode_Script> Parser::ParseFromTokens(Tokenizer &toks)
{
    auto rt = ParseScript(toks);
    if(!toks.Match(TokenType::EndMark))
    {
        throw ParserException(
            "unexpected token " + toks.Current().str,
            toks.Filename(), toks.Line());
    }
    return rt;
}

Ptr<ASTNode_Script> Parser::ParseFromFile(const String &dstFilename,
    const String &callerFilename,
    int callerLine)
{
    String src;
    if(!ReadTxt(dstFilename, src))
    {
        throw ParserException(
            "failed to open file: " + dstFilename,
            callerFilename, callerLine);
    }
    Tokenizer toks(src, dstFilename);
    return ParseFromTokens(toks);
}

Ptr<ASTNode_Script> Parser::ParseScript(Tokenizer &toks)
{
    Vec<Ptr<ASTNode_Statement>> stmts;
    Ptr<ASTNode_Statement> stmt;
    while((stmt = ParseStatement(toks)))
        stmts.push_back(std::move(stmt));

    auto rt = MakePtr<ASTNode_Script>();
    rt->stmts = std::move(stmts);
    return rt;
}

Ptr<ASTNode_Statement> Parser::ParseStatement(Tokenizer &toks)
{
    // Start definition
    // AGZStart := Symbol;

    int startLine = toks.Line();
    if(toks.Match(TokenType::Kw_Start))
    {
        if(!toks.Match(TokenType::DefinedAs))
        {
            throw ParserException(
                ":= expected", toks.Filename(), toks.Line());
        }

        auto rt = MakePtr<ASTNode_Statement>();
        rt->start = MakePtr<ASTNode_Start>();
        rt->start->filename = toks.Filename();
        rt->start->line = toks.Line();
        rt->start->sym = ParseSymbol(toks);

        if(!toks.Match(TokenType::Semicolon))
        {
            throw ParserException(
                "; expected", toks.Filename(), toks.Line());
        }

        return rt;
    }

    // namespace definition
    // Namespace Identifier { Statement* }

    if(toks.Match(TokenType::Kw_Namespace))
    {
        if(toks.Current().type != TokenType::Identifier)
        {
            throw ParserException(
                "namespace name expected", toks.Filename(), toks.Line());
        }
        String name = std::move(toks.Current().str);
        toks.Next();

        if(!toks.Match(TokenType::LeftBrace))
        {
            throw ParserException(
                "{ expected", toks.Filename(), toks.Line());
        }

        auto content = ParseScript(toks);

        if(!toks.Match(TokenType::RightBrace))
        {
            throw ParserException(
                "} expected", toks.Filename(), toks.Line());
        }

        auto rt = MakePtr<ASTNode_Statement>();
        rt->ns = MakePtr<ASTNode_Namespace>();
        rt->ns->name = name;
        rt->ns->content = MakePtr<ASTNode_Script>();
        rt->ns->content->stmts = std::move(content->stmts);
        return rt;
    }

    // import statement
    // Import Path
    if(toks.Match(TokenType::Kw_Import))
    {
        if(toks.Current().type != TokenType::Path)
        {
            throw ParserException(
                "filename expected", toks.Filename(), toks.Line());
        }
        String path = std::move(toks.Current().str);
        toks.Next();

        // 计算path相对于本文件的正确路径
        String parentPath;
        if(GetParentPath(toks.Filename(), parentPath))
            path = parentPath + "/" + path;

        auto content = ParseFromFile(
            path, toks.Filename(), toks.Line());
        auto rt = MakePtr<ASTNode_Statement>();
        rt->import = MakePtr<ASTNode_Import>();
        rt->import->path = std::move(path);
        rt->import->content = std::move(content);
        return rt;
    }

    // rule definition
    // Identifier := Symbol (Plus Symbol)* Rulename?;
    if(toks.Current().type == TokenType::Identifier)
    {
        String left = std::move(toks.Current().str);
        int leftLine = toks.Line();
        toks.Next();

        if(!toks.Match(TokenType::DefinedAs))
        {
            throw ParserException(
                ":= expected", toks.Filename(), toks.Line());
        }

        Vec<Ptr<ASTNode_Symbol>> right;
        do {
            auto sym = ParseSymbol(toks);
            if(!sym)
            {
                throw ParserException(
                    "symbol reference expected", toks.Filename(), toks.Line());
            }
            right.push_back(std::move(sym));
        } while(toks.Match(TokenType::Plus));

        auto rt = MakePtr<ASTNode_Statement>();
        rt->rule = MakePtr<ASTNode_Rule>();
        rt->rule->line = leftLine;
        rt->rule->filename = toks.Filename();
        rt->rule->left = std::move(left);
        rt->rule->right = std::move(right);

        if(!toks.Match(TokenType::Semicolon))
        {
            throw ParserException(
                "; expected", toks.Filename(), toks.Line());
        }

        if(toks.Current().type == TokenType::RuleName)
        {
            rt->rule->rulename = std::move(toks.Current().str);
            toks.Next();
        }

        return rt;
    }

    return Ptr<ASTNode_Statement>();
}

Ptr<ASTNode_Symbol> Parser::ParseSymbol(Tokenizer &toks)
{
    // token
    // "Identifier"
    if(toks.Match(TokenType::DoubleQuotation))
    {
        if(toks.Current().type != TokenType::Identifier)
        {
            throw ParserException(
                "token name expected", toks.Filename(), toks.Line());
        }

        String id = std::move(toks.Current().str);
        toks.Next();

        if(!toks.Match(TokenType::DoubleQuotation))
        {
            throw ParserException(
                "\" expected", toks.Filename(), toks.Line());
        }

        auto rt = MakePtr<ASTNode_Symbol>();
        rt->token = std::move(id);
        return rt;
    }

    // reference
    // Identifier(.Identifier)*
    if(toks.Current().type == TokenType::Identifier)
    {
        Vec<String> idens;
        do {
            if(toks.Current().type != TokenType::Identifier)
            {
                throw ParserException(
                    "identifier expected", toks.Filename(), toks.Line());
            }
            idens.push_back(std::move(toks.Current().str));
            toks.Next();
        } while(toks.Match(TokenType::Point));

        auto rt = MakePtr<ASTNode_Symbol>();
        rt->idens = std::move(idens);
        return rt;
    }

    return Ptr<ASTNode_Symbol>();
}

NS_END(MetaLang)
NS_END(AGZ)
