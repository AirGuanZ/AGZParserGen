/*================================================================
Filename: InternalASTNode.cpp
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "InternalASTNode.h"
#include "Rule.h"

AGZ_NAMESPACE_BEGIN(AGZ)

void ASTNode_Script::PrettyPrint(std::ostream &out,
                                 const std::string &prefix,
                                 const std::string &tab,
                                 bool leftBracNewline,
                                 bool inlineImported) const
{
    if(stmts_.size())
        stmts_[0]->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    for(size_t i = 1; i < stmts_.size(); ++i)
    {
        out << std::endl;
        stmts_[i]->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    }
}

void ASTNode_Statement::PrettyPrint(std::ostream &out,
                                    const std::string &prefix,
                                    const std::string &tab,
                                    bool leftBracNewline,
                                    bool inlineImported) const
{
    if(startDef_)
        startDef_->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    else if(namespace_)
        namespace_->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    else if(import_)
        import_->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    else
    {
        assert(rule_);
        rule_->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    }
}

void ASTNode_Namespace::PrettyPrint(std::ostream &out,
                                    const std::string &prefix,
                                    const std::string &tab,
                                    bool leftBracNewline,
                                    bool inlineImported) const
{
    out << prefix << "namespace " << name_;
    if(leftBracNewline)
    {
        out << std::endl
            << prefix << "{" << std::endl;
    }
    else
    {
        out << " {" << std::endl;
    }
    content_->PrettyPrint(out, prefix + tab, tab, leftBracNewline, inlineImported);
    out << std::endl << prefix << "}";
}

void ASTNode_StartDefinition::PrettyPrint(std::ostream &out,
                                          const std::string &prefix,
                                          const std::string &tab,
                                          bool leftBracNewline,
                                          bool inlineImported) const
{
    out << prefix << "AGZ_Start := ";
    sym_->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    out << ";";
}

void ASTNode_Import::PrettyPrint(std::ostream &out,
                                 const std::string &prefix,
                                 const std::string &tab,
                                 bool leftBracNewline,
                                 bool inlineImported) const
{
    if(inlineImported)
        imported_->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    else
        out << prefix << "import [" << path_ << "]";
}

void ASTNode_Rule::PrettyPrint(std::ostream &out,
                               const std::string &prefix,
                               const std::string &tab,
                               bool leftBracNewline,
                               bool inlineImported) const
{
    out << prefix << id_ << " :=";
    for(auto sym : syms_)
    {
        out << " + ";
        sym->PrettyPrint(out, prefix, tab, leftBracNewline, inlineImported);
    }
    out << ";";
}

void ASTNode_Symbol::PrettyPrint(std::ostream &out,
                                 const std::string &prefix,
                                 const std::string &tab,
                                 bool leftBracNewline,
                                 bool inlineImported) const
{
    if(token_.size())
        out << "token\"" << token_ << "\"";
    else
    {
        assert(ids_.size());
        out << ids_[0];
        for(size_t i = 1; i < ids_.size(); ++i)
            out << "." << ids_[i];
    }
}

AGZ_NAMESPACE_END(AGZ)
