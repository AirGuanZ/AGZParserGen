#include <iostream>

#include <MetaLang/Parser.h>
#include <MetaLang/PrettyPrinter.h>
#include <MetaLang/RawRuleTable.h>
#include <MetaLang/Scope.h>
#include <MetaLang/Tokenizer.h>
#include <FirstSet.h>
#include <RuleTable.h>

int main(void)
{
    using namespace AGZ::MetaLang;

    try
    {
        const std::string src =
            R"____(# this is a comment
                   namespace Expr
                   {
                       F  := "integer";             (Int)
                       F  := "ParL" + F + "ParR";   (Par)
                       T  := F + T_;                (FRest)
                       T  := F;                     (F)
                       T_ := "Times" + F;           (F)
                       T_ := "Times" + F + T_;      (FRest)
                       E  := T;                     (T)
                       E  := T + E_;                (TRest)
                       E_ := "PLus" + T;            (T)
                       E_ := "PLus" + T + E_;       (TRest)
                   }
                   AGZStart := Expr.E;
            )____";

        Tokenizer tokenizer("namespace Global {" + src + "}", "TestFilename");

        Parser parser;
        auto ast = parser.ParseFromTokens(tokenizer);

        auto scopeTree = BuildScopeTree(ast, { }, "");
        auto rawRuleTab = RawRuleTableBuilder().Build(scopeTree);
        
        for(auto &it : *rawRuleTab)
            std::cout << it.second.ToString() << std::endl;

        struct TA
        {
            using Token = AGZ::String;

            Token ToToken(const AGZ::String &tok) const
            {
                return tok;
            }
        };
        AGZ::RuleTable<TA> ruleTable;
        TA tA;
        ruleTable.Build(*rawRuleTab, tA);

        AGZ::FirstSetTable<TA> fstTab(ruleTable);
        for(AGZ::NTIdx NT = 0;NT != ruleTable.GetNTCount(); ++NT)
        {
            std::cout << AGZ::String(20, '=') << std::endl;
            std::cout << ruleTable.Detrans(NT) << std::endl;
            for(auto &tok : fstTab.GetFirstSet(NT))
                std::cout << tok << " ";
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
}
