#include <iostream>

#include <MetaLang/Parser.h>
#include <MetaLang/PrettyPrinter.h>
#include <MetaLang/RuleTable.h>
#include <MetaLang/Scope.h>
#include <MetaLang/Tokenizer.h>

int main(void)
{
    using namespace AGZ::MetaLang;

    try
    {
        const std::string src =
            R"____(# this is a comment
                   import [TestFolder/TestFile.agz]
                   namespace MyNamespace # this is another comment
                   {
                       A := "TestTokenName";
                       B := A + B;
                       B := A;                 (DefinitionOfB1)
                       B := NamespaceHaHaHa.Y; (DefinitionOfB2)
                       namespace MyNamespace
                       {
                           B := A;
                           B := Global.MyNamespace.B;
                       }
                   }
                   M := "hahaha" + MyNamespace.A;
                   Z := M;
                   AGZ_Start := MyNamespace.B;
            )____";

        Tokenizer tokenizer("namespace Global {" + src + "}", "TestFilename");
        Parser parser;
        auto ast = parser.ParseFromTokens(tokenizer);
        PrettyPrinter().Print(std::cout, ast, "");

        auto scopeTree = BuildScopeTree(ast, { }, "");
        auto rawRuleTab = RawRuleTableBuilder().Build(scopeTree);
        for(auto &it : *rawRuleTab)
            std::cout << it.second.ToString() << std::endl;            
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
    catch(const RuleTableException &err)
    {
        std::cout << "Symbol error around line "
                  << err.line
                  << " in file "
                  << err.filename
                  << ":\n\t"
                  << err.msg << std::endl;
    }
}
