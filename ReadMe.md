试着做个Parser Generator，算是一次做一百个人的编译原理作业吧XD……

使用姿势大概是这样的：

1. 先写文法描述脚本，描述想要的Parser的上下文无关文法，脚本大概长这样：

   ```
    # 注释以'#'开头
    import [Utility/Utility.agz] # 支持从其他文件导入

    # 支持命名空间
    namespace MyNamespace
    {
        # 定义非终结符
        A := Utility.S + A; # 使用'.'访问命名空间成员
        A := "token_name";  # 使用双引号包住token类型名
    }

    # 定义开始符号
    AGZ_Start := MyNamespace.A;
   ```

2. 然后在自己的程序中调用AGZParserGen的生成接口，得到Parser实例：

   ```cpp
   Parser<TokenAdaptor> parser;
   parser.BuildFromSourceFile(tokenAdaptor, filename);
   ```

   其中`TokenAdaptor`是词法单元类型适配器，用来实现文法脚本中的词法单元和进行实际语法分析时使用的词法单元之间的映射，由用户自己指定。当然，我会写个简单的字符串映射版本作为默认。这么设计一是为了更加灵活，二是我不想处理字符编码问题XD……

3. 接下来就想怎么用就怎么用了，可以直接拿来分析，也可以输出静态分析器的代码：

   ```cpp
   // 直接分析源代码，得到AST
   auto AST = parser.Parse(tokenAdaptor, tokemStream);

   // 输出成C++静态分析代码
   // 待完成
   parser.PrintAs<AGZ::CppBackend>("staticCppParser.h", "staticCppParser.cpp");
   ```

详细的用法参见[Usage](./Usage.md)。

到目前为止已经完成了Parser的主体部分，接下来要做的是：

- [x] 支持把建立好的LR分析表保存到文件，以免每次使用都重新建立
- [ ] 使用文档
- [ ] 设计好输出到其他语言代码的后端接口
- [ ] 实现一个输出到C++的后端
- [ ] 优化性能
