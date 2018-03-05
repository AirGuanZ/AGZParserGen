/*================================================================
Filename: InternalTokenizer.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include "Common.h"
#include "Exception.h"

AGZ_NAMESPACE_BEGIN(AGZ)
AGZ_NAMESPACE_BEGIN(Internal)

struct Token
{
    enum class Type
    {
        Identifier,
        Path,

        LeftBrace,
        RightBrace,

        DefinedAs,

        Plus,
        Point,
        Semicolon,
        DoubleQuotation,

        Kw_Namespace,
        Kw_Start,
        Kw_Import,

        End,
    };
    Type type;
    std::string str;
};

extern const std::string Keyword_Namespace;
extern const std::string Keyword_Start;
extern const std::string Keyword_Import;

class TokenException : public Exception
{
public:
    TokenException(const std::string &msg, int line, int srcPos, const std::string &filename)
        : Exception(msg), line_(line), srcPos_(srcPos), filename_(filename)
    {

    }

    int line_, srcPos_;
    std::string filename_;
};

class Tokenizer
{
public:
    Tokenizer(const std::string &src, const std::string &filename);

    const Token &Current(void) const;
    
    void Next(void);

    bool Match(Token::Type type);

    int GetLine(void) const { return line_; }
    
    const std::string &GetFilename(void) const { return filename_; }

private:
    //ÌÞ³ýÇ°×ºµÄ¿Õ°××Ö·ûºÍ×¢ÊÍ
    void DelFirstSpaces(void);

    bool NextIdentifier(std::string &output);

    Token FirstToken(void);

private:
    Token cur_;

    const char *pos_;
    std::string src_;

    std::string filename_;
    int line_;
};

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
