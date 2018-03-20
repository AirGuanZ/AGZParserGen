/*================================================================
Filename: ASTNode.h
Date: 2018.3.7
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include "Common.h"
#include "Rule.h"

AGZ_NAMESPACE_BEGIN(AGZ)

template<typename _tokenMapping>
class ASTNode
{
public:
    using TokenMapping = _tokenMapping;
    using TokenName = typename TokenMapping::TokenName;
    using SpecRule = Rule<TokenName>;

    struct ASTChild
    {
        enum class Type
        {
            NonTernimating,
            Token
        };

        Type type;
        Ptr<ASTNode> NTChild;
        TokenName tokenChild;;
    };
    
    typename SpecRule::ID ruleID;
    typename SpecRule::NonTerminatingName nodeName;

    WPtr<ASTNode> parent;
    std::vector<ASTChild> children;
};

AGZ_NAMESPACE_END(AGZ)
