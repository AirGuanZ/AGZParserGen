/*================================================================
Filename: MetaLang/PrettyPrinter.h
Date: 2018.4.16
Created by AirGuanZ
================================================================*/
#pragma once

#include <ostream>

#include <Common/Utility.h>
#include <MetaLang/AST.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

class PrettyPrinter
{
public:
    void Print(std::ostream &out, Ptr<ASTNode_Script> script,
               const String &linePrefix) const;
    void Print(std::ostream &out, Ptr<ASTNode_Statement> stmt,
               const String &linePrefix) const;
               
    String ToString(Ptr<ASTNode_Symbol> sym) const;
};

NS_END(MetaLang)
NS_END(AGZ)
