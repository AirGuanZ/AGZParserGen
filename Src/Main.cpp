#include <iostream>

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
        

        do
        {
            const Token &tok = tokenizer.Current();
            tokenizer.Next();

            std::cout << GetTokenTypeName(tok.type) << ", " << tok.str << std::endl;
        } while(!tokenizer.Match(TokenType::EndMark));
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
}
