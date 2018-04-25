NS_BEGIN(AGZ)

template<typename _tA>
ASTCons<_tA>::ASTCons(const LRTable<_tA> &LRTab)
    : LRTab_(LRTab)
{

}

template<typename _tA>
Ptr<ASTNode<_tA>> ASTCons<_tA>::Parse(TokenStream &toks,
                                     const RuleTable<_tA> &ruleTab,
                                     _tA &tA) const
{
    using TransOpr = typename LRTable<_tA>::TransOperation;

    TokT<_tA> endTok = ToToken(tA, MetaLang::TOKEN_STR_ENDMARK);
    NTIdx startIdx = ruleTab.Trans(MetaLang::GLOBAL_NAMESPACE_STR + "." +
                                   MetaLang::KEYWORD_STR_START);

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
            tA.ParsingError(toks, state);
            return MakePtr<ASTNode<_tA>>();
        }
        const TransOpr &opr = itAct->second;

        if(opr.type == TransOpr::Reduce)
        {
            Vec<Ptr<ASTSym<_tA>>> children(opr.reduceRule->right.size());
            for(int i = static_cast<int>(opr.reduceRule->right.size()) - 1;
                i >= 0; --i)
            {
                children[i] = symStk.top();
                symStk.pop();
                stateStk.pop();
            }

            Ptr<ASTNode<_tA>> left = MakePtr<ASTNode<_tA>>(
                opr.reduceRule, std::move(children));

            if(opr.reduceRule->left == startIdx && curT == endTok)
                return left;

            auto leftSym = MakePtr<ASTSym<_tA>>();
            leftSym->type = SymT::NT;
            leftSym->NT   = left;
            symStk.push(leftSym);

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
