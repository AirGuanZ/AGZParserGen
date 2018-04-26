# Usage

AGZParserGen的常规使用流程是：

1. 编写用来描述文法的脚本。
2. 用C++编写具有要求接口的`TokenAdaptor`。`TokanAdaptor`向AGZParserGen描述了作为语法分析输入的词法单元流具有怎样的形式，因此AGZParserGen可以灵活地应对各种各样的词法单元输入。
3. 在程序中定义`AGZ::Parser<TokenAdaptor>`实例，用它加载文法脚本。
4. 至此，已经可以调用`AGZ::Parser<TokenAdaptor>::Parser`方法进行语法分析，输出AST。

## 文法描述语言

在理论计算机科学中，上下文无关文法被形式化地定义为一个四元组$(N, T, S, R)$，分别表示非终结符号集合（Non-terminating symbols），终结符号集合（Terminating symbols），开始符号（Start symbol）和规则集合（Rules）。AGZParserGen用来描述文法的语言以列出规则集合为主，其间使用到的非终结符和终结符都被自动记录下来，无需显式定义。以带括号的整数四则运算文法为例：

```
F := "integer"     ;
F := "(" + E + ")" ;
T := F             ;
T := T + "*" + F   ;
E := T             ;
E := E + "+" + T   ;
AGZStart := E      ;
```

可以看到，整个文法完全由其规则所描述。和C语言标识符相似的符号为非终结符；`:=`将一条规则的左侧和右侧区分开来，规则结尾处用`;`进行标记；终结符则用双引号包起来，将会以字符串的形式被传给`TokenAdaptor`并转换为用户指定的词法单元类型；最后一条规则是`AGZStart := E`，表示将非终结符`E`定义为文法的开始符号。

下面详细地介绍文法描述语言的语法和语义。

### 终结符和非终结符

终结符指的是哪些可能出现在语法分析树内部节点处的符号，用于表示语法推导的中间过程。在AGZParserGen中，任何非保留字的标识符都被视为一个非终结符，其正则表达式为：
$$
('\_'  \mid [a..z] \mid [A-Z])('\_' \mid [a..z] \mid [A..Z] \mid [0..9])^*
$$
非终结符则用一对双引号包含起来，中间可以是除了双引号/换行符以外的任意字符。

### 结束符

一般而言，词法单元流的末尾应有一个特殊的词法单元，专门用来标记词法单元流的结束。AGZParserGen将字符串`“#”`作为结束单元的非终结符名字，其使用在稍后的TokenAdaptor一节中介绍。

### 规则

一条规则由左右两边构成，中间由`:=`连接，末尾用一个分号表示结束。规则左边必须是一个非终结符或关键字`AGZStart`；右边则是一连串用`+`分隔的符号，每个符号要么是一个终结符，要么是对其他非终结符的引用。

一个非终结符可以出现在多条规则的左边，表示它可以被展开为这些规则右边中的任意一个。为了在最终得到的语法分析树中方便地区分每个内部节点使用了哪条规则，可以在规则定义的分号之后加上一个规则名。规则名用一对圆括号包裹起来，除了未命名规则外，每个终结符不能有两条以上名字相同的规则。譬如：

```
A := "minecraft" + A;           # 普通的规则定义，+表示连接运算
A := B;                         # 这条规则和上一条构成“或”关系
                                # 这两条A的规则都没有命名

B := "token"; (First Rule Of B) # 将这条规则命名为“First Rule Of B”
B := B + "token"; (X)           # 将这条规则命名为“X”
B := A + "dark souls"; (X)      # 这条规则和上一条名字重复了，因此会报错

C := A; (X)                     # 这条规则和B的某规则名字相同
                                # 但其左侧的非终结符不同，因此是合法的
```

### 注释

每一行第一个`#`以及其后该行所有的内容都被视为注释。譬如：

```
# 这是注释
A := B; # 这是注释
```

### 命名空间

一个实际意义的文法可能会非常复杂，对此，AGZParserGen提供了命名空间来避免非终结符名字污染。命名空间语法为：

```
namespace 命名空间名字 {
    命名空间内容
}
```

命名空间名字和非终结符名字构成规则相同。

定义一个命名空间会产生一个对应的作用域，全局定义都处于一个名为`Global`的作用域内。AGZParserGen采用和C++名字查找规则相似的词法作用域。设`A`是某个可见的作用域，则可以通过`A.B`来表示作用域`A`中的符号`B`。命名空间可以是嵌套的，因此作用域也可以是嵌套的。下面是一个嵌套的命名空间例子：

```
namespace X {
    namespace Y {
        B := A;	          # 把B定义为Global.X.A
    }
    A := "minecraft" + A; # 在X的作用域内，这个定义屏蔽了全局的A
    C := Global.A;		  # 通过Global.XXX的写法可以引用被内层作用域屏蔽的外层符号
}
A := "dark souls";        # 在全局作用域内定义符号A
AGZStart := X.C;          # 把开始符号定义为命名空间X中的符号C
```

一个命名空间的定义可以由多个部分构成，譬如下面的代码：

```
namespace MyNamespace {
	A := "minecraft";
}

namespace MyNamespace {
    B := "dark souls";
}
```

就在命名空间`MyNamespace`中定义了`A`和`B`两个非终结符。

### 文法类型

AGZParserGen支持的文法范围为LR(1)文法，对超出AGZParserGen处理能力的文法，`AGZ::Parser`在解析文法脚本时会抛出`AGZ::LRBuildingException`异常。

## 在C++中使用AGZParserGen

AGZ所有的组件均定义在`AGZ`命名空间中，使用时引入头文件`Include/Parser.h`即可。

### TokenAdaptor

AGZParserGen可以应对各种类型的词法单元流，因而需要用户提供用于描述词法单元流的`TokenAdaptor`。通常，一个合法的`TokenAdaptor`定义包含以下内容：

```cpp
class TokenAdaptor
{
    using TokenType   = 用来表示词法单元类型的类型
    using TokenStream = 词法单元流
    using TokenData   = 词法单元本身的定义

    // 把文法脚本中的词法单元名字映射为对应的词法单元类型
    const TokenType &ToToken(const AGZ::String &tok) const;

    // 给定任意词法单元，返回其类型
    const TokenType &ToType(const TokenData &tok) const;

    // 取得一个词法单元流当前的词法单元
    const TokenData &Current(TokenStream &toks) const;

    // 让词法单元流前进一个符号
    void Next(TokenStream &toks) const;

    // 探测到语法错误时会调用该函数
    void ParsingError(const TokenStream &toks, size_t curState) const;
};
```

上面代码中出现的`const`限定符和引用都不是必须的，只需要成员函数的调用形式符合要求即可。比如，若`TokenData`是个庞大的结构体，那么应当使用`const TypeData &`作为`TokenAdaptor::Current`的返回值类型；反之，若`TokenData`很小，那么直接用`TypeData`类型进行值传递也是可行的。

举个例子，假设我们正在编写一个四则运算计算器，它的文法脚本是：

```
F := "number";
F := "-" + F;
F := "(" + E + ")";
T := F;
T := T + "*" + F;
T := T + "/" + F;
E := T;
E := E + "+" + T;
E := E + "-" + T;
AGZStart := E;
```

我们用下面的枚举类型表示词法单元的类型：

```cpp
enum class MyTokenType
{
    EndMark,
    NumberLiteral,
    LeftPar,
    RightPar,
    Add, Sub, Mul, Div
};

struct MyToken
{
    MyTokenType type;     // 该词法单元的类型
    double numberLiteral; // 如果是数字字面量，那么值就存放在numberLiteral中
};
```

不妨假设我们已经实现了一个词法分析器，它输出一个`std::list<MyToken>`，其中最后一个词法单元固定为`MyTokenType::EndMark`类型（用于标志词法单元流的结尾）。此时，此时，我们的词法单元流可以被定义为：

```cpp
struct MyTokenStream
{
    std::list<MyToken> tokens;            // 所有词法单元的链表
                                          // 由词法分析器输出
    std::list<MyToken>::iterator current; // 当前正在扫描的词法单元
                                          // 应初始化为tokens.begin()
};
```

综上，我们针对这一四则运算语言的`TokenAdaptor`可以被实现为：

```cpp
class MyTokenAdaptor
{
public:
    using TokenType   = MyTokenType;
    using TokenStream = MyTokenStream;
    using TokenData   = MyToken;
    
    // 把文法脚本中的词法单元名字映射为对应的词法单元类型
    TokenType ToToken(const AGZ::String &tok) const
    {
        static const std::map<AGZ::String, MyTokenType> m =
        {
            { "number", MyTokenType::NumberLiteral },
            { "(",      MyTokenType::LeftPar       },
            { ")",      MyTokenType::RightPar      },
            { "*",      MyTokenType::Mul           },
            { "/",      MyTokenType::Div           },
            { "+",      MyTokenType::Add           },
            { "-",      MyTokenType::Sub           },
            { "#",      MyTokenType::EndMark       } // 规定“#”表示结束符
        };
        auto it = m.find(tok);
        assert(it != m.end());
        return it->second;
    }
    
    // 给定任意词法单元，返回其类型
    TokenType ToType(const TokenData &tok) const
    {
        return tok.type;
    }
    
    // 取得一个词法单元流当前的词法单元
    const TokenData &Current(TokenStream &toks) const
    {
        return *toks.current;
    }
    
    // 让词法单元流前进一个符号
    void Next(TokenStream &toks) const
    {
        if(toks.current != toks.tokens.end())
            ++toks.current;
    }
    
    // 探测到语法错误时会调用该函数
    void ParsingError(const TokenStream &toks, size_t curState) const
    {
        throw std::runtime_error("Syntax error!");
    }
};
```

有了上述的`MyTokenAdaptor`，就能够使用`AGZ::Parser<MyTokenAdaptor>`分析四则运算语言了。

### 使用AGZ::Parser

仍然以上述的四则运算语言为例，假设我们已经写好了文法描述脚本（保存在`script.txt`中）和需要的TokenAdaptor，那么在程序中可以这样使用`AGZ::Parser`得到AST：

```cpp
MyTokenAdaptor tokenAdaptor;
AGZ::Parser<MyTokenAdaptor> parser;

// 从文法脚本构建语法分析器
try
{
    parser.BuildFromSourceFile(tokenAdaptor, "script.txt");
}
catch(const AGZ::Exception &err)
{
    std::cerr << err.msg << std::endl;
}

// 用语法分析器从词法单元流创建AST
auto AST = parser.Parse(tokenAdaptor, myTokenStream);
```

在进行语法分析时，若遇到语法错误，`TokenAdaptor::ParsingError`会被调用，且返回的AST为空值（表达式`!AST`为真）。

### 生成的AST结构

（施工中……）