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
                   }
                   AGZ_Start := MyNamespace.B;
            )____";

        AGZ::InternalTokenizer tokenizer(src, "TestFilename");
        AGZ::InternalParser parser;

        auto AST = parser.Parse(tokenizer);

        auto symTable = AGZ::InternalSymbolTableBuilder().Build(AST);
        for(auto &it : symTable->rules)
        {
            const AGZ::Rule &rule = it.second;
            std::cout << rule.left << " := ";
            for(auto &sym : rule.syms)
            {
                if(sym.type == AGZ::Rule::Symbol::Type::Token)
                    std::cout << "\"" << sym.name << "\" ";
                else
                    std::cout << sym.name << " ";
            }
            std::cout << std::endl;
        }
    }
    catch(const AGZ::InternalTokenException &err)
    {
        std::cout << "Lex error around line " + std::to_string(err.line_)
                     + ", " + err.filename_ << ":\n\t";
        std::cout << err.What() << std::endl;
    }
    catch(const AGZ::InternalParserException &err)
    {
        std::cout << "Syntax error around line " + std::to_string(err.line_)
                     + ", " + err.filename_ << ":\n\t";
        std::cout << err.What() << std::endl;
    }
    catch(const AGZ::InternalSymbolTableException &err)
    {
        std::cout << "Syntax error around line " + std::to_string(err.line_)
                     + ", " + err.filename_ << ":\n\t";
        std::cout << err.What() << std::endl;
    }
}
