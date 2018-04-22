#include <iostream>
#include <sstream>

#include <MetaLang/Parser.h>
#include <MetaLang/PrettyPrinter.h>
#include <MetaLang/RawRuleTable.h>
#include <MetaLang/Scope.h>
#include <MetaLang/Tokenizer.h>
#include <FirstSet.h>
#include <LRItem.h>
#include <RuleTable.h>

using namespace AGZ;

struct TA
{
    using TokenType = AGZ::String;

    TokenType ToToken(const AGZ::String &tok) const
    {
        return tok;
    }
};

String ToString(const LRItem<TA> &item, RuleTable<TA> &ruleTab)
{
    std::stringstream sst;

    auto SymToStr = [&](const Sym<TA> &s)
    {
        if(s.type == SymT::Token)
            sst << "\"" << s.tok << "\"";
        else
            sst << ruleTab.Detrans(s.NT);
    };

    sst << "(";
    sst << ruleTab.Detrans(item.rule->left);
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
    using namespace AGZ::MetaLang;

    try
    {
        const std::string src =
             R"____(# this is a comment
                    L := L + P;
                    L := P;
                    P := "lpar" + P + "rpar";
                    P := "lpar" + "rpar";
                    AGZStart := L;
             )____";

        Tokenizer tokenizer("namespace Global {" + src + "}", "TestFilename");

        Parser parser;
        auto ast = parser.ParseFromTokens(tokenizer);

        auto scopeTree = BuildScopeTree(ast, { }, "");
        auto rawRuleTab = RawRuleTableBuilder().Build(scopeTree);
        
        for(auto &it : *rawRuleTab)
            std::cout << it.second.ToString() << std::endl;

        TA tA;

        AGZ::RuleTable<TA> ruleTable;
        ruleTable.Build(*rawRuleTab, tA);

        AGZ::FirstSetTable<TA> fstTab(ruleTable);
        /*for(AGZ::NTIdx NT = 0;NT != ruleTable.GetNTCount(); ++NT)
        {
            std::cout << AGZ::String(20, '=') << std::endl;
            std::cout << ruleTable.Detrans(NT) << std::endl;
            for(auto &tok : fstTab.GetFirstSet(NT))
                std::cout << tok << " ";
            std::cout << std::endl;
        }*/
        std::cout << String(2, '\n');

        AGZ::LRItemSetConstructor<TA> LRCons;
        LRCons.Build(ruleTable, fstTab, tA);

        for(auto &set : LRCons.idx2Set_)
        {
            for(auto &item : set)
                std::cout << ToString(item, ruleTable) << std::endl;
            std::cout << std::endl;
        }
    }
    catch(const TokenException &err)
    {
        std::cout << "Lex error around line "
                  << err.line
                  << " in file "
                  << err.filename
                  << ":\n\t"
                  << err.msg << std::endl;
    }
    catch(const ParserException &err)
    {
        std::cout << "Syntax error around line "
                  << err.line
                  << " in file "
                  << err.filename
                  << ":\n\t"
                  << err.msg << std::endl;
    }
    catch(const RawRuleTableException &err)
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
}
