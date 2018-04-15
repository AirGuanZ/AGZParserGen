/*================================================================
Filename: MetaLang/Tokenizer.h
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#pragma once

#include <ostream>

#include <Utility.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

enum class TokenType
{
    Identifier,
    Path,

    LeftBrace,
    RightBrace,

    RuleName,

    DefinedAs,

    Plus,
    Point,
    Semicolon,
    DoubleQuotation,

    Kw_Namespace,
    Kw_Start,
    Kw_Import,

    EndMark
};

const String &GetTokenTypeName(TokenType type);

struct Token
{
    TokenType type;
    String str;
};

extern const String KEYWORD_STR_NAMESPACE;
extern const String KEYWORD_STR_START;
extern const String KEYWORD_STR_IMPORT;

class TokenException
{
public:
    TokenException(const String &msg,
                   const String &filename,
                   int line, int srcPos)
        : msg(msg), filename(filename), line(line), srcPos(srcPos)
    {

    }

    String msg;
    String filename;
    int line;
    int srcPos;
};

class Tokenizer
{
public:
    Tokenizer(const String &src, const String &filename);

    const Token &Current(void) const;

    void Next(void);

    bool Match(TokenType type);

    int Line(void) const;

    const String &Filename(void) const;

private:
    void SkipWhitespaces(void);

    Token NextToken(void);

private:
    Token curTok_;

    String src_;
    size_t idx_;

    String filename_;
    int line_;
};

NS_END(MetaLang)
NS_END(AGZ)
