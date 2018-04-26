# Usage

AGZParserGen的常规使用流程是：

1. 编写用来描述文法的脚本。
2. 用C++编写具有要求接口的`TokenAdaptor`。`TokanAdaptor`向AGZParserGen描述了作为语法分析输入的词法单元流具有怎样的形式，因此AGZParserGen可以灵活地应对各种各样的词法单元输入。
3. 在程序中定义`AGZ::Parser<TokenAdaptor>`实例，用它加载文法脚本。
4. 至此，已经可以调用`AGZ::Parser<TokenAdaptor>::Parser`方法进行语法分析，输出AST。

## 文法描述语言简介

在理论计算机科学中，上下文无关文法被形式化地定义为一个四元组$(N, T, S, R)$，分别表示非终结符号集合（Non-terminating symbols），终结符号集合（Terminating symbols），开始符号（Start symbol）和规则集合（Rules）。AGZParserGen用来描述文法的语言以列出规则集合为主，其间使用到的非终结符和终结符都被自动记录下来，无需显式定义。以带括号的整数四则运算文法为例：

```
F := "integer";
F := "(" + E + ")";
T := F;
T := T + "*" + F;
E := T;
E := E + "+" + T;
AGZStart := E;
```

可以看到，整个文法完全由其规则所描述。和C语言标识符相似的符号为非终结符；`:=`将一条规则的左侧和右侧区分开来，规则结尾处用`;`进行标记；终结符则用双引号包起来，将会以字符串的形式被传给`TokenAdaptor`并转换为用户指定的词法单元类型；最后一条规则是`AGZStart := E`，表示将非终结符`E`定义为文法的开始符号。

下面详细地介绍文法描述语言的语法和语义。

### 终结符和非终结符

终结符指的是哪些可能出现在语法分析树内部节点处的符号，用于表示语法推导的中间过程。在AGZParserGen中，任何非保留字的标识符都被视为一个非终结符，其正则表达式为：
$$
('\_'  \mid [a..z] \mid [A-Z])('\_' \mid [a..z] \mid [A..Z] \mid [0..9])^*
$$
非终结符则用一对双引号包含起来，中间可以是除了双引号/换行符以外的任意字符。

### 注释

每一行第一个`#`以及其后该行所有的内容都被视为注释。譬如：

```
# 这是注释
A := B; # 这是注释
```

### 使用命名空间

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
        B := A;	# 把B定义为Global.X.A
    }
    A := "minecraft" + A; # 在X的作用域内，这个定义屏蔽了全局的A
    C := Global.A;		  # 通过Global.XXX的写法可以引用被内层作用域屏蔽的外层符号
}
A := "dark souls"; # 在全局作用域内定义符号A
AGZStart := X.C;   # 把开始符号定义为命名空间X中的符号C
```

一个命名空间的定义可以由多个部分构成，譬如下面的代码：

```
namespace MyNamespace {
	A := "minecraft"    
}

namespace MyNamespace {
    B := "dark souls"
}
```

就在命名空间`MyNamespace`中定义了`A`和`B`两个非终结符。

### 文法类型

AGZParserGen支持的文法范围为LR(1)文法，对超出AGZParserGen处理能力的文法，`AGZ::Parser`在解析文法脚本时会抛出`AGZ::LRBuildingException`异常。

## 在C++中使用AGZParserGen

施工中……