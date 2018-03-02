/*================================================================
Filename: InternalTokenizer.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include "Common.h"
#include "Exception.h"

AGZ_NAMESPACE_BEGIN(AGZ)

struct Token
{
    enum class Type
    {
        Identifier,
        Path,

        LeftBrace,
        RightBrace,

        DefinedAs,

        DoubleQuotation,

        Kw_Namespace,
        Kw_Start,
        Kw_Import,
        Kw_Token,

        End,
    };
    Type type;
    String str;
};

class InternalTokenException : public Exception
{
public:
    InternalTokenException(const String &msg)
        : Exception(msg)
    {

    }
};

class InternalTokenizer
{
public:
    InternalTokenizer(const String &src, const String &filename);

    const Token &Current(void) const;
    void Next(void);

    bool Match(Token::Type type);

private:
    //ÌÞ³ýÇ°×ºµÄ¿Õ°××Ö·ûºÍ×¢ÊÍ
    void DelFirstSpaces(const Char *&p);

    Token FirstToken(const Char *&p);

private:
    Token cur_;

    const Char *pos_;
    String src_;

    String filename_;
    int line_;
};

AGZ_NAMESPACE_END(AGZ)
