/*================================================================
Filename: MetaLang/Tokenizer.h
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#pragma once

#include <ostream>

#include <Common/Utility.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

enum class TokenType
{
    Identifier,
    Path,
    Token,

    LeftBrace,
    RightBrace,

    RuleName,

    DefinedAs,

    Plus,
    Point,
    Semicolon,

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

class TokenException
{
public:
    TokenException(const String &msg,
                   const String &filename,
                   int line)
        : msg(msg), filename(filename), line(line)
    {

    }

    const String msg;
    const String filename;
    const int line;
};

inline const String &KEYWORD_STR_NAMESPACE(void);
inline const String &KEYWORD_STR_START    (void);
inline const String &KEYWORD_STR_IMPORT   (void);
inline const String &GLOBAL_NAMESPACE_STR (void);
inline const String &TOKEN_STR_ENDMARK    (void);

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

#include <MetaLang/Tokenizer.inl>
