/*================================================================
Filename: InternalTokenizer.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include "Common.h"
#include "Exception.h"

AGZ_NAMESPACE_BEGIN(AGZ)

struct InternalToken
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
        Kw_Token,

        End,
    };
    Type type;
    std::string str;
};

class InternalTokenException : public Exception
{
public:
    InternalTokenException(const std::string &msg, int line, int srcPos, const std::string &filename)
        : Exception(msg), line_(line), srcPos_(srcPos), filename_(filename)
    {

    }

    int line_, srcPos_;
    std::string filename_;
};

class InternalTokenizer
{
public:
    InternalTokenizer(const std::string &src, const std::string &filename);

    const InternalToken &Current(void) const;
    
    void Next(void);

    bool Match(InternalToken::Type type);

    int GetLine(void) const { return line_; }
    
    const std::string &GetFilename(void) const { return filename_; }

private:
    //ÌÞ³ýÇ°×ºµÄ¿Õ°××Ö·ûºÍ×¢ÊÍ
    void DelFirstSpaces(void);

    bool NextIdentifier(std::string &output);

    InternalToken FirstToken(void);

private:
    InternalToken cur_;

    const char *pos_;
    std::string src_;

    std::string filename_;
    int line_;
};

AGZ_NAMESPACE_END(AGZ)
