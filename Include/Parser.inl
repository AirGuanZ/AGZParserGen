NS_BEGIN(AGZ)

template<typename _tA>
Parser<_tA>::Parser(const LRTable<_tA> &LRTab)
    : LRTab_(LRTab)
{

}

template<typename _tA>
Ptr<ASTNode<_tA>> Parser<_tA>::Parse(TokenStream &toks, _tA &tA) const
{
    using TransOpr = typename LRTable<_tA>::TransOperation;

    TokT<_tA> endTok = ToToken(tA, MetaLang::TOKEN_STR_ENDMARK);

    Stack<size_t> stateStk;
    Stack<Ptr<ASTSym<_tA>>> symStk;

    stateStk.push(0);

    TokenData curD = tA.Current(toks);
    TokenType curT = tA.ToType(curD);

    while(true)
    {
        size_t state = stateStk.top();

        auto itAct = LRTab_.action_.find({ state, curT });
        if(itAct == LRTab_.action_.end())
        {
            tA.ParsingError(toks);
            return MakePtr<ASTNode<_tA>>();
        }
        const TransOpr &opr = itAct->second;

        if(opr.type == TransOpr::Reduce)
        {
            Vec<Ptr<ASTSym<_tA>>> children(opr.reduceRule->right.size());
            for(int i = static_cast<int>(opr.reduceRule->right.size());
                i >= 0; --i)
            {
                children[i] = symStk.top();
                symStk.pop();
                stateStk.pop();
            }

            Ptr<ASTNode<_tA>> left = MakePtr<ASTNode<_tA>>(
                opr.reduceRule, std::move(children));
            auto leftSym = MakePtr<ASTSym<_tA>>();
            leftSym->type = SymT::NT;
            leftSym->NT   = left;
            symStk.push(std::move(leftSym));

            size_t tState = stateStk.top();
            
            auto it = LRTab_.goto_.find({ tState, opr.reduceRule->left });
            assert(it != LRTab_.goto_.end());
            stateStk.push(it->second);
        }
        else if(opr.type == TransOpr::Shift)
        {
            auto word = MakePtr<ASTSym<_tA>>();
            word->type = SymT::Token;
            word->NT   = 0;
            word->tok  = curD;
            symStk.push(word);

            stateStk.push(opr.shiftDst);

            tA.Next(toks);
            curD = tA.Current(toks);
            curT = tA.ToType(curD);
        }
        else
        {
            assert(opr.type == TransOpr::Accept);
            assert(symStk.top()->type == SymT::NT);
            return symStk.top()->NT;
        }
    }

    return MakePtr<ASTNode<_tA>>();
}

NS_END(AGZ)
