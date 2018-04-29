#include <iostream>
#include <list>

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

    bool ToBinaryFile(std::ofstream &fout, TokenType type)
    {
        return AGZ::WriteString(fout, type);
    }

    bool FromBinaryFile(std::ifstream &fin, TokenType &type)
    {
        return AGZ::ReadString(fin, type);
    }
};

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
