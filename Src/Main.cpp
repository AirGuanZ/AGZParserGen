#include <iostream>
#include <list>
#include <sstream>

#include <Parser.h>

struct TokenStream
{
    std::list<AGZ::String>::iterator cur;
    std::list<AGZ::String> toks;
};

struct TA
{
    using TokenType   = AGZ::String;
    using TokenStream = ::TokenStream;
    using TokenData   = TokenType;

    const TokenType &ToToken(const AGZ::String &tok) const
    {
        return tok;
    }

    const TokenType &ToType(const TokenData &tok) const
    {
        return tok;
    }

    const TokenData &Current(TokenStream &toks) const
    {
        return *toks.cur;
    }

    void Next(TokenStream &toks) const
    {
        if(toks.cur != toks.toks.end())
            ++toks.cur;
    }

    void ParsingError(const TokenStream &toks, size_t curState) const
    {
        throw std::runtime_error("Syntax error!");        
    }
};

AGZ::String ToString(const AGZ::LRItem<TA> &item, AGZ::RuleTable<TA> &ruleTab)
{
    std::stringstream sst;

    auto SymToStr = [&](const AGZ::Sym<TA> &s)
    {
        if(s.type == AGZ::SymT::Token)
            sst << "\"" << s.tok << "\"";
        else
            sst << ruleTab.Detrans(s.NT).substr(7);
    };

    sst << "(";
    sst << ruleTab.Detrans(item.rule->left).substr(7);
    sst << " := ";
    for(size_t i = 0; i < item.dotPos; ++i)
    {
        SymToStr(item.rule->right[i]);
        sst << " ";
    }
    sst << "@ ";
    for(size_t i = item.dotPos; i < item.rule->right.size(); ++i)
    {
        SymToStr(item.rule->right[i]);
        sst << " ";
    }

    sst << item.lookAhead << ")";

    return sst.str();
}

int main(void)
{
    try
    {
        const std::string src =
             R"____(# this is a comment
                    L := L + P;
                    L := P;
                    P := "(" + P + ")";
                    P := "(" + ")";
                    AGZStart := L;
             )____";

        TA tA;
        AGZ::Parser<TA> parser;

        TokenStream toks;
        toks.toks = { "(","(", ")", ")", "(", ")", "#" };
        toks.cur = toks.toks.begin();

        parser.BuildFromSource(tA, src, "TestFilename");
        auto AST = parser.Parse(tA, toks);

        std::cout << "Parsing completed!" << std::endl;
    }
    catch(const AGZ::Exception &err)
    {
        std::cout << err.msg << std::endl;
    }
}
