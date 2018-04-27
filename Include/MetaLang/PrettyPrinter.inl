#include <cassert>

#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)
NS_BEGIN(MetaLang)

inline void PrettyPrinter::Print(std::ostream &out, Ptr<ASTNode_Script> script,
                          const String &lp) const
{
    for(auto stmt : script->stmts)
        Print(out, stmt, lp);
}

inline void PrettyPrinter::Print(std::ostream &out, Ptr<ASTNode_Statement> stmt,
                          const String &lp) const
{
    using std::endl;

    if(stmt->ns)
    {
        out << lp << KEYWORD_STR_NAMESPACE << " " << stmt->ns->name << endl;
        out << lp << "{" << endl;
        Print(out, stmt->ns->content, lp + "    ");
        out << lp << "}" << endl;
    }
    else if(stmt->start)
    {
        out << lp << KEYWORD_STR_START << " := " << ToString(stmt->start->sym) << endl;
    }
    else if(stmt->import)
    {
        Print(out, stmt->import->content, lp);
    }
    else
    {
        assert(stmt->rule);
        out << lp << stmt->rule->left << " := " << ToString(stmt->rule->right[0]);
        for(size_t i = 1; i != stmt->rule->right.size(); ++i)
            out << " + " << ToString(stmt->rule->right[i]);
        out << ";";
        if(stmt->rule->rulename.size())
            out << "(" << stmt->rule->rulename << ")";
        out << endl;
    }
}

inline String PrettyPrinter::ToString(Ptr<ASTNode_Symbol> sym) const
{
    if(sym->token.size())
    {
        return "\"" + sym->token + "\"";
    }
    else
    {
        assert(sym->idens.size());
        return StrJoin(sym->idens, ".");
    }
}

NS_END(MetaLang)
NS_END(AGZ)
