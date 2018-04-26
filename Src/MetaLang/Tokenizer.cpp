/*================================================================
Filename: MetaLang/Tokenizer.cpp
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cctype>
#include <map>
#include <utility>
#include <vector>

#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

const String KEYWORD_STR_NAMESPACE = "namespace";
const String KEYWORD_STR_START     = "AGZStart";
const String KEYWORD_STR_IMPORT    = "import";

const String GLOBAL_NAMESPACE_STR  = "Global";

const String TOKEN_STR_ENDMARK     = "#";

const String &GetTokenTypeName(TokenType type)
{
    static const String TOKEN_TYPE_NAMES[] =
    {
        "Identifier",
        "Path",
        "Token",
        "LeftBrace",
        "RightBrace",
        "RuleName",
        "DefinedAs",
        "Plus",
        "Point",
        "Semicolon",
        "Namespace",
        "Start",
        "Import",
        "EndMark"
    };
    auto idx = std::underlying_type<TokenType>::type(type);
    return TOKEN_TYPE_NAMES[idx];
}

Tokenizer::Tokenizer(const String &src, const String &filename)
    : src_(src), idx_(0), filename_(filename), line_(1)
{
    Next();
}

const Token &Tokenizer::Current(void) const
{
    return curTok_;
}

void Tokenizer::Next(void)
{
    curTok_ = NextToken();
}

bool Tokenizer::Match(TokenType type)
{
    if(curTok_.type == type)
    {
        Next();
        return true;
    }
    return false;
}

int Tokenizer::Line(void) const
{
    return line_;
}

const String &Tokenizer::Filename(void) const
{
    return filename_;
}

void Tokenizer::SkipWhitespaces(void)
{
    while(true)
    {
        char ch = src_[idx_];
        if(ch == '\n')
            ++idx_, ++line_;
        else if(std::isspace(ch))
            ++idx_;
        else if(ch == '#')
        {
            while(src_[++idx_] != '\n' && src_[idx_] != '\0')
                ;
        }
        else
            break;
    }
}

Token Tokenizer::NextToken(void)
{
    SkipWhitespaces();
    
    // 特殊符号

    static const Vec<std::pair<String, TokenType>> SPECIAL_SYMBOLS =
    {
        { ":=", TokenType::DefinedAs       },
        { "{",  TokenType::LeftBrace       },
        { "}",  TokenType::RightBrace      },
        { ".",  TokenType::Point           },
        { "+",  TokenType::Plus            },
        { ";",  TokenType::Semicolon       },
    };

    for(auto &sym : SPECIAL_SYMBOLS)
    {
        if(src_.find(sym.first, idx_) == idx_)
        {
            idx_ += sym.first.length();
            return Token{ sym.second, sym.first };
        }
    }

    // identifier & keywords

    char ch = src_[idx_];

    static const std::map<String, TokenType> KEYWORDS =
    {
        { KEYWORD_STR_NAMESPACE, TokenType::Kw_Namespace },
        { KEYWORD_STR_START,     TokenType::Kw_Start     },
        { KEYWORD_STR_IMPORT,    TokenType::Kw_Import    },
    };

    if(std::isalpha(ch) || ch == '_')
    {
        String iden(1, ch);
        while(std::isalnum(src_[++idx_]) || src_[idx_] == '_')
            iden += src_[idx_];
        auto it = KEYWORDS.find(iden);
        if(it != KEYWORDS.end())
            return Token{ it->second, it->first };
        return Token{ TokenType::Identifier, iden };
    }

    // Path

    if(ch == '[')
    {
        ++idx_;
        String path;
        while(true)
        {
            if(src_[idx_] == '\0')
                throw TokenException("unclosed [", filename_, line_);
            else if(src_[idx_] == ']')
            {
                ++idx_;
                return Token{ TokenType::Path, path };
            }
            path += src_[idx_++];
        }
    }

    // Token
    if(ch == '\"')
    {
        ++idx_;
        String tok;
        while(true)
        {
            if(src_[idx_] == '\0' || src_[idx_] == '\n')
                throw TokenException("unclosed \"", filename_, line_);
            else if(src_[idx_] == '\"')
            {
                ++idx_;
                return Token{ TokenType::Token, tok };
            }
            else if(src_[idx_] == '\\' && src_[idx_] == '\"')
            {
                idx_ += 2;
                tok += "\"";
            }
            else
                tok += src_[idx_++];
        }
    }

    // Rule name

    if(ch == '(')
    {
        ++idx_;
        String name;
        while(true)
        {
            if(src_[idx_] == '\0')
                throw TokenException("unclosed (", filename_, line_);
            else if(src_[idx_] == ')')
            {
                ++idx_;
                return Token{ TokenType::RuleName, name };
            }
            name += src_[idx_++];
        }
    }

    // 结束标志

    if(ch == '\0')
        return Token{ TokenType::EndMark, "" };
    
    throw TokenException("unknown token", filename_, line_);
    return Token{ TokenType::EndMark, "" };
}

NS_END(MetaLang)
NS_END(AGZ)
