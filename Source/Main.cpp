/*================================================================
Filename: Main.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <iostream>

#include "InternalParser.h"
#include "InternalSymbolTable.h"
#include "InternalTokenizer.h"

int main(void)
{
    using namespace AGZ::Internal;

    try
    {
        const std::string src =
            R"____(# this is a comment
                   import [TestFolder/TestFile.agz]
                   namespace MyNamespace # this is another comment
                   {
                       A := "TestTokenName";
                       B := A + B;
                       B := A;
                       B := NamespaceHaHaHa.Y;
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

        auto AST = parser.Parse(tokenizer);

        auto symTable = SymbolTableBuilder().Build(AST);
        for(auto &it : symTable->rules)
        {
            const AGZ::RawRule &rule = it.second;
            std::cout << rule.left << " := ";
            for(auto &sym : rule.syms)
            {
                if(sym.type == AGZ::RuleSymbolType::Token)
                    std::cout << "\"" << sym.name << "\" ";
                else
                    std::cout << sym.name << " ";
            }
            std::cout << std::endl;
        }

        struct TokenMapping
        {
            using TokenName = int;
            int operator()(const std::string &name) const
            {
                return 0;
            }
        };
        TokenMapping tokenMapping;
        AGZ::Internal::SymbolTable<TokenMapping> sT(tokenMapping, *symTable);

        auto r = sT.GetRuleByID(0);
        sT.EraseRule(r);
        sT.AddRule(r);
    }
    catch(const TokenException &err)
    {
        std::cout << "Lex error around line " + std::to_string(err.line_)
                     + ", " + err.filename_ << ":\n\t";
        std::cout << err.What() << std::endl;
    }
    catch(const ParserException &err)
    {
        std::cout << "Syntax error around line " + std::to_string(err.line_)
                     + ", " + err.filename_ << ":\n\t";
        std::cout << err.What() << std::endl;
    }
    catch(const SymbolTableException &err)
    {
        std::cout << "Syntax error around line " + std::to_string(err.line_)
                     + ", " + err.filename_ << ":\n\t";
        std::cout << err.What() << std::endl;
    }
}
