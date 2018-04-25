#include <iostream>
#include <list>
#include <sstream>

#include <MetaLang/Parser.h>
#include <MetaLang/PrettyPrinter.h>
#include <MetaLang/RawRuleTable.h>
#include <MetaLang/Scope.h>
#include <MetaLang/Tokenizer.h>

#include <LRParser/ASTCons.h>
#include <LRParser/FirstSet.h>
#include <LRParser/LRItem.h>
#include <LRParser/LRTable.h>
#include <LRParser/RuleTable.h>

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

        AGZ::MetaLang::Tokenizer tokenizer("namespace Global {" + src + "}", "TestFilename");

        AGZ::MetaLang::Parser parser;
        auto ast = parser.ParseFromTokens(tokenizer);

        auto scopeTree = AGZ::MetaLang::BuildScopeTree(ast, { }, "");
        auto rawRuleTab = AGZ::MetaLang::RawRuleTableBuilder().Build(scopeTree);
        
        for(auto &it : *rawRuleTab)
            std::cout << it.second.ToString() << std::endl;

        TA tA;

        AGZ::RuleTable<TA> ruleTable;
        ruleTable.Build(*rawRuleTab, tA);

        AGZ::FirstSetTable<TA> fstTab(ruleTable);

        std::cout << AGZ::String(2, '\n');

        AGZ::LRItemSetConstructor<TA> LRCons;
        LRCons.Build(ruleTable, fstTab, tA);

        for(auto &set : LRCons.idx2Set_)
        {
            for(auto &item : set)
                std::cout << ToString(item, ruleTable) << std::endl;
            std::cout << std::endl;
        }

        AGZ::LRTable<TA> LRTab;
        LRTab.Build(ruleTable, LRCons, tA);

        TokenStream toks;
        toks.toks = { "(","(", ")", ")", "(", ")", "#" };
        toks.cur = toks.toks.begin();

        AGZ::ASTCons<TA> finalPsr(LRTab);
        finalPsr.Parse(toks, ruleTable, tA);
        std::cout << "Parsing completed!" << std::endl;
    }
    catch(const AGZ::MetaLang::TokenException &err)
    {
        std::cout << "Lex error around line "
                  << err.line
                  << " in file "
                  << err.filename
                  << ":\n\t"
                  << err.msg << std::endl;
    }
    catch(const AGZ::MetaLang::ParserException &err)
    {
        std::cout << "Syntax error around line "
                  << err.line
                  << " in file "
                  << err.filename
                  << ":\n\t"
                  << err.msg << std::endl;
    }
    catch(const AGZ::MetaLang::RawRuleTableException &err)
    {
        std::cout << "Symbol error around line "
                  << err.line
                  << " in file "
                  << err.filename
                  << ":\n\t"
                  << err.msg << std::endl;
    }
    catch(const AGZ::RuleTableException &err)
    {
        std::cout << err.msg << std::endl;
    }
    catch(const std::runtime_error &err)
    {
        std::cout << err.what() << std::endl;
    }
}
