/*================================================================
Filename: MetaLang/Scope.h
Date: 2018.4.16
Created by AirGuanZ
================================================================*/
#include <MetaLang/AST.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

struct ScopeNode
{
    Vec<String> globalName;
    String globalNameStr;

    WPtr<ScopeNode> parent;
    Map<String, Ptr<ScopeNode>> children;

    Set<String> localSyms;
    Set<Ptr<ASTNode_Script>> entrys;
};

Ptr<ScopeNode> BuildScopeTree(Ptr<ASTNode_Script> script,
                              const Vec<String> &globalName,
                              const String &globalNameStr);

NS_END(MetaLang)
NS_END(AGZ)
