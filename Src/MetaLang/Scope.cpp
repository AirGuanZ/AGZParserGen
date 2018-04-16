/*================================================================
Filename: MetaLang/Scope.cpp
Date: 2018.4.16
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <MetaLang/Scope.h>
#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

static void AddScopeChild(Map<String, Ptr<ScopeNode>> &children,
                          const String &childName,
                          Ptr<ScopeNode> child)
{
    auto it = children.find(childName);
    if(it == children.end())
    {
        children[childName] = child;
        return;
    }

    auto ori = it->second;

    for(auto &it : child->children)
        AddScopeChild(ori->children, it.first, it.second);
    
    for(auto &ls : child->localSyms)
        ori->localSyms.insert(ls);
    
    for(auto entry : child->entrys)
        ori->entrys.insert(entry);
}

Ptr<ScopeNode> BuildScopeTree(Ptr<ASTNode_Script> script,
                              const Vec<String> &globalName,
                              const String &globalNameStr)
{
    assert(script);

    auto rt = MakePtr<ScopeNode>();
    rt->entrys.insert(script);

    for(auto stmt : script->stmts)
    {
        if(stmt->start)
            rt->localSyms.insert(KEYWORD_STR_START);
        else if(stmt->ns)
        {
            Vec<String> childGlobalName = globalName;
            childGlobalName.push_back(stmt->ns->name);

            String childGlobalNameStr =
                (globalNameStr.empty() ? "" : globalNameStr + ".") + stmt->ns->name;

            Ptr<ScopeNode> child = BuildScopeTree(
                stmt->ns->content, childGlobalName, childGlobalNameStr);
            child->parent        = rt;
            child->globalName    = std::move(childGlobalName);
            child->globalNameStr = std::move(childGlobalNameStr);

            AddScopeChild(rt->children, stmt->ns->name, child);
        }
        else if(stmt->import)
        {
            auto importedScope = BuildScopeTree(
                stmt->import->content, globalName, globalNameStr);
            
            for(auto &it : importedScope->children)
                AddScopeChild(rt->children, it.first, it.second);
            
            for(auto &ls : importedScope->localSyms)
                rt->localSyms.insert(ls);
        }
        else
        {
            assert(stmt->rule);
            rt->localSyms.insert(stmt->rule->left);
        }
    }
    return rt;
}

NS_END(MetaLang)
NS_END(AGZ)
