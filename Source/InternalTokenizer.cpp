/*================================================================
Filename: InternalTokenizer.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cctype>

#include "InternalTokenizer.h"

AGZ_NAMESPACE_BEGIN(AGZ)

InternalTokenizer::InternalTokenizer(const std::string &src, const std::string &filename)
    : src_(src), filename_(filename), line_(1)
{
    pos_ = &src_[0];
    Next();
}

const InternalToken &InternalTokenizer::Current(void) const
{
    return cur_;
}

void InternalTokenizer::Next(void)
{
    cur_ = FirstToken();
}

bool InternalTokenizer::Match(InternalToken::Type type)
{
    if(cur_.type == type)
    {
        Next();
        return true;
    }
    return false;
}

void InternalTokenizer::DelFirstSpaces(void)
{
    assert(pos_ != nullptr);

    while(true)
    {
        //»»ÐÐ
        if(*pos_ == '\n')
        {
            ++pos_;
            ++line_;
            continue;
        }

        //¿Õ°××Ö·û
        if(std::isspace(*pos_))
        {
            ++pos_;
            continue;
        }

        //×¢ÊÍ
        if(*pos_ == '#')
        {
            while(*pos_ != '\n' && *pos_ != '\0')
                ++pos_;
            continue;
        }

        break;
    }
}

bool InternalTokenizer::NextIdentifier(std::string &output)
{
    output = "";

    if(std::isalpha(*pos_) || *pos_ == '_')
    {
        output = *pos_++;
        while(std::isalnum(*pos_) || *pos_ == '_')
            output += *pos_++;
        return true;
    }

    return false;
}

InternalToken InternalTokenizer::FirstToken(void)
{
    assert(pos_ != nullptr);
    DelFirstSpaces();

    if(*pos_ == '{')
    {
        ++pos_;
        return { InternalToken::Type::LeftBrace, "{" };
    }

    if(*pos_ == '}')
    {
        ++pos_;
        return { InternalToken::Type::RightBrace, "}" };
    }

    if(*pos_ == ':')
    {
        if(*++pos_ == '=')
        {
            ++pos_;
            return { InternalToken::Type::DefinedAs, ":=" };
        }
        throw InternalTokenException(
            std::string("Unknown token :") + std::string({ *pos_ }),
            line_, pos_ - &src_[0], filename_);
    }

    if(*pos_ == '.')
    {
        ++pos_;
        return { InternalToken::Type::Point, "." };
    }

    if(*pos_ == '+')
    {
        ++pos_;
        return { InternalToken::Type::Plus, "+" };
    }

    if(*pos_ == '"')
    {
        ++pos_;
        return { InternalToken::Type::DoubleQuotation, "\"" };
    }

    if(*pos_ == ';')
    {
        ++pos_;
        return { InternalToken::Type::Semicolon, ";" };
    }

    if(*pos_ == '[')
    {
        ++pos_;
        std::string path;
        while(true)
        {
            if(*pos_ == '\0')
            {
                throw InternalTokenException(
                    "Unclosed '['", line_, pos_ - &src_[0], filename_);
            }

            if(*pos_ == ']')
            {
                ++pos_;
                return { InternalToken::Type::Path, path };
            }

            path += *pos_++;
        }
    }

    if(*pos_ == '\0')
        return { InternalToken::Type::End, "" };

    std::string nextIden;
    if(NextIdentifier(nextIden))
    {
        if(nextIden == "namespace")
            return { InternalToken::Type::Kw_Namespace, "namespace" };
        if(nextIden == "AGZ_Start")
            return { InternalToken::Type::Kw_Start, "AGZ_Start" };
        if(nextIden == "import")
            return { InternalToken::Type::Kw_Import, "import" };
        if(nextIden == "token")
            return { InternalToken::Type::Kw_Token, "token" };
        return { InternalToken::Type::Identifier, nextIden };
    }

    throw InternalTokenException(
        std::string("Unknown token ") + std::string({ *pos_ }),
        line_, pos_ - &src_[0], filename_);
    return { InternalToken::Type::End, "" };
}

AGZ_NAMESPACE_END(AGZ)
