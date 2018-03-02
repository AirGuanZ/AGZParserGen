/*================================================================
Filename: InternalTokenizer.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cctype>

#include "InternalTokenizer.h"

AGZ_NAMESPACE_BEGIN(AGZ)

namespace
{
    bool NextIdentifier(const Char *&p, String &output)
    {
        output = "";

        if(std::isalpha(*p) || *p == '_')
        {
            output = *p++;
            while(std::isalnum(*p) || *p == '_')
                output += *p++;
            return true;
        }

        return false;
    }
}

InternalTokenizer::InternalTokenizer(const String &src, const String &filename)
    : src_(src), filename_(filename), line_(1)
{
    pos_ = &src_[0];
    Next();
}

const Token &InternalTokenizer::Current(void) const
{
    return cur_;
}

void InternalTokenizer::Next(void)
{
    cur_ = FirstToken(pos_);
}

bool InternalTokenizer::Match(Token::Type type)
{
    if(cur_.type == type)
    {
        Next();
        return true;
    }
    return false;
}

void InternalTokenizer::DelFirstSpaces(const Char *&p)
{
    assert(p != nullptr);

    while(true)
    {
        //»»ÐÐ
        if(*p == '\n')
        {
            ++p;
            ++line_;
            continue;
        }

        //¿Õ°××Ö·û
        if(std::isspace(*p))
        {
            ++p;
            continue;
        }

        //×¢ÊÍ
        if(*p == '#')
        {
            while(*p != '\n' && *p != '\0')
                ++p;
            continue;
        }

        break;
    }
}

Token InternalTokenizer::FirstToken(const Char *&p)
{
    assert(p != nullptr);
    DelFirstSpaces(p);

    if(*p == '{')
    {
        ++p;
        return { Token::Type::LeftBrace, "{" };
    }

    if(*p == '}')
    {
        ++p;
        return { Token::Type::RightBrace, "}" };
    }

    if(*p == ':')
    {
        if(*++p == '=')
        {
            ++p;
            return { Token::Type::DefinedAs, ":=" };
        }
        throw InternalTokenException(
            String("Unknown token: :") + String(p) +
            "\n in line " + std::to_string(line_) +
            " in " + filename_);
    }

    if(*p == '"')
    {
        ++p;
        return { Token::Type::DoubleQuotation, "\"" };
    }

    if(*p == '[')
    {
        ++p;
        String path;
        while(true)
        {
            if(*p == '\0')
            {
                throw InternalTokenException(
                    "Unclosed '['"
                    "\n in line " + std::to_string(line_) +
                    "in file " + filename_);
            }

            if(*p == ']')
            {
                ++p;
                return { Token::Type::Path, path };
            }

            path += *p++;
        }
    }

    if(*p == '\0')
        return { Token::Type::End, "" };

    String nextIden;
    if(NextIdentifier(p, nextIden))
    {
        if(nextIden == "namespace")
            return { Token::Type::Kw_Namespace, "namespace" };
        if(nextIden == "AGZ_Start")
            return { Token::Type::Kw_Start, "AGZ_Start" };
        if(nextIden == "import")
            return { Token::Type::Kw_Import, "import" };
        if(nextIden == "token")
            return { Token::Type::Kw_Token, "token" };
        return { Token::Type::Identifier, nextIden };
    }

    throw InternalTokenException(
        String("Unknown token:") + String(p) +
        "\n in line " + std::to_string(line_) +
        " in " + filename_);
    return { Token::Type::End, "" };
}

AGZ_NAMESPACE_END(AGZ)
