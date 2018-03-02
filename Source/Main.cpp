/*================================================================
Filename: Main.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <iostream>

#include "InternalTokenizer.h"

int main(void)
{
    try
    {
        const AGZ::String src =
            R"____(
                import [Others/AnotherScriptFile.agz]
                # this is a comment
                namespace # this is another comment
                {
                    import [Others\hahaha.agz]
                    A := token"TestTokenName"
                    B := A B
                    B := A
                }
                AGZ_Start := B
            )____";

        AGZ::InternalTokenizer tokenizer(src, "TestFilename");

        AGZ::Token tok = tokenizer.Current();
        while(tok.type != AGZ::Token::Type::End)
        {
            std::cout << tok.str << std::endl;
            tokenizer.Next();
            tok = tokenizer.Current();
        }
    }
    catch(const AGZ::InternalTokenException &err)
    {
        std::cout << err.What() << std::endl;
    }
}
