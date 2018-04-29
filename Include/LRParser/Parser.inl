#include <LRParser/AST.h>
#include <LRParser/ASTCons.h>
#include <LRParser/FirstSet.h>
#include <LRParser/LRItem.h>
#include <LRParser/LRTable.h>
#include <LRParser/RuleTable.h>

#include <MetaLang/AST.h>
#include <MetaLang/Parser.h>
#include <MetaLang/RawRuleTable.h>
#include <MetaLang/Scope.h>
#include <MetaLang/Tokenizer.h>

NS_BEGIN(AGZ)

inline Exception::Exception(const String &msg)
    : msg(msg)
{

}

inline FileException::FileException(const String &msg, const String &filename)
    : Exception(msg), filename(filename)
{

}

inline MetaLangException::MetaLangException(const String &filename, int line,
                                            const String &msg)
    : Exception(msg), filename(filename), line(line)
{

}

inline LRBuildingException::LRBuildingException(const String &msg)
    : Exception(msg)
{

}

inline SymbolException::SymbolException(const String &msg)
    : Exception(msg)
{

}

template<typename _tA>
inline void Parser<_tA>::BuildFromSource(TokenAdaptor &tA, const String &src,
                                                           const String &srcFilename)
{
    Clear();

    try
    {
        // 分析元语言代码
        MetaLang::Tokenizer srcTokenizer(
            "namespace Global {" + src + "}", srcFilename);
        auto srcAST = MetaLang::Parser().ParseFromTokens(srcTokenizer);

        // 构造作用域和符号表
        auto scopeTree  = MetaLang::BuildScopeTree(srcAST, { }, "");
        auto rawRuleTab = MetaLang::RawRuleTableBuilder().Build(scopeTree);

        // 构造规则表
        ruleTab_.Build(*rawRuleTab, tA);

        // 构造FIRST集合
        FirstSetTable<_tA> fstTab(ruleTab_);

        // 构造LR自动机
        LRItemSetConstructor<_tA> LRCons;
        LRCons.Build(ruleTab_, fstTab, tA);

        // 构造LR转移表
        LRTab_.Build(ruleTab_, LRCons, tA);
    }
    catch(const MetaLang::TokenException &err)
    {
        Clear();
        throw MetaLangException(err.filename, err.line, err.msg);
    }
    catch(const MetaLang::ParserException &err)
    {
        Clear();
        throw MetaLangException(err.filename, err.line, err.msg);
    }
    catch(const MetaLang::RawRuleTableException &err)
    {
        Clear();
        throw MetaLangException(err.filename, err.line, err.msg);
    }
    catch(const RuleTableException &err)
    {
        Clear();
        throw LRBuildingException(err.msg);
    }
}

template<typename _tA>
inline void Parser<_tA>::BuildFromSourceFile(TokenAdaptor &tA, const String &filename)
{
    String src;
    if(!ReadTxt(filename, src))
        throw FileException("Failed to read source file", filename);
    
    BuildFromSource(tA, src, filename);
}

template<typename _tA>
inline void Parser<_tA>::Clear(void)
{
    ruleTab_.Clear();
    LRTab_.Clear();
}

template<typename _tA>
inline Ptr<ASTNode<_tA>> Parser<_tA>::Parse(TokenAdaptor &tA, TokenStream &toks) const
{
    return ASTCons<_tA>(LRTab_).Parse(toks, ruleTab_, tA);
}

template<typename _tA>
inline NTIdx Parser<_tA>::NTName2Idx(const String &name) const
{
    try
    {
        return ruleTab_.Trans(name);
    }
    catch(const RuleTableException &err)
    {
        throw SymbolException(err.msg);
    }
}

template<typename _tA>
inline const String &Parser<_tA>::NTIdx2Name(NTIdx idx) const
{
    try
    {
        return ruleTab_.Detrans(idx);
    }
    catch(const RuleTableException &err)
    {
        throw SymbolException(err.msg);
    }
}

template<typename _tA>
inline bool Parser<_tA>::ToBinaryFile(const String &filename, _tA &tA) const
{
    std::ofstream fout(filename, std::ofstream::out |
                                 std::ofstream::binary |
                                 std::ofstream::trunc);
    return fout && ruleTab_.ToBinaryFile(fout, tA)
                && LRTab_.ToBinaryFile(fout, tA);
}

template<typename _tA>
inline bool Parser<_tA>::FromBinaryFile(const String &filename, _tA &tA)
{
    ruleTab_.Clear();
    LRTab_.Clear();

    std::ifstream fin(filename, std::ofstream::in |
                                std::ofstream::binary);
    return fin && ruleTab_.FromBinaryFile(fin, tA)
               && LRTab_.FromBinaryFile(fin, tA, ruleTab_);
}

NS_END(AGZ)
