/*================================================================
Filename: Main.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <iostream>

#include "InternalParser.h"
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
                       A := token"TestTokenName";
                       B := A + B;
                       B := A;
                   }
                   AGZ_Start := B;
            )____";

        AGZ::InternalTokenizer tokenizer(src, "TestFilename");
        AGZ::InternalParser parser;

        auto AST = parser.Parse(tokenizer);
        if(AST)
            AST->PrettyPrint(std::cout, "", "    ", true, true);
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
}
