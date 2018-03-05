/*================================================================
Filename: InternalTokenizer.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cctype>

#include "InternalTokenizer.h"

AGZ_NAMESPACE_BEGIN(AGZ)
AGZ_NAMESPACE_BEGIN(Internal)

const std::string Keyword_Namespace = "namespace";
const std::string Keyword_Start     = "AGZ_Start";
const std::string Keyword_Import    = "import";

Tokenizer::Tokenizer(const std::string &src, const std::string &filename)
    : src_(src), filename_(filename), line_(1)
{
    pos_ = &src_[0];
    Next();
}

const Token &Tokenizer::Current(void) const
{
    return cur_;
}

void Tokenizer::Next(void)
{
    cur_ = FirstToken();
}

bool Tokenizer::Match(Token::Type type)
{
    if(cur_.type == type)
    {
        Next();
        return true;
    }
    return false;
}

void Tokenizer::DelFirstSpaces(void)
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

bool Tokenizer::NextIdentifier(std::string &output)
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

Token Tokenizer::FirstToken(void)
{
    assert(pos_ != nullptr);
    DelFirstSpaces();

    if(*pos_ == '{')
    {
        ++pos_;
        return { Token::Type::LeftBrace, "{" };
    }

    if(*pos_ == '}')
    {
        ++pos_;
        return { Token::Type::RightBrace, "}" };
    }

    if(*pos_ == ':')
    {
        if(*++pos_ == '=')
        {
            ++pos_;
            return { Token::Type::DefinedAs, ":=" };
        }
        throw TokenException(
            std::string("Unknown token :") + std::string({ *pos_ }),
            line_, static_cast<int>(pos_ - &src_[0]), filename_);
    }

    if(*pos_ == '.')
    {
        ++pos_;
        return { Token::Type::Point, "." };
    }

    if(*pos_ == '+')
    {
        ++pos_;
        return { Token::Type::Plus, "+" };
    }

    if(*pos_ == '"')
    {
        ++pos_;
        return { Token::Type::DoubleQuotation, "\"" };
    }

    if(*pos_ == ';')
    {
        ++pos_;
        return { Token::Type::Semicolon, ";" };
    }

    if(*pos_ == '[')
    {
        ++pos_;
        std::string path;
        while(true)
        {
            if(*pos_ == '\0')
            {
                throw TokenException(
                    "Unclosed '['", line_, static_cast<int>(pos_ - &src_[0]), filename_);
            }

            if(*pos_ == ']')
            {
                ++pos_;
                return { Token::Type::Path, path };
            }

            path += *pos_++;
        }
    }

    if(*pos_ == '\0')
        return { Token::Type::End, "" };

    std::string nextIden;
    if(NextIdentifier(nextIden))
    {
        if(nextIden == Keyword_Namespace)
            return { Token::Type::Kw_Namespace, Keyword_Namespace };
        if(nextIden == Keyword_Start)
            return { Token::Type::Kw_Start, Keyword_Start };
        if(nextIden == Keyword_Import)
            return { Token::Type::Kw_Import, Keyword_Import };
        return { Token::Type::Identifier, nextIden };
    }

    throw TokenException(
        std::string("Unknown token ") + std::string({ *pos_ }),
        line_, static_cast<int>(pos_ - &src_[0]), filename_);
    return { Token::Type::End, "" };
}

AGZ_NAMESPACE_END(Internal)
AGZ_NAMESPACE_END(AGZ)
