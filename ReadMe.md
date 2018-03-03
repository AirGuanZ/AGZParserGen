试着做个Parser Generator，提供：

- [ ] 输入文法后动态生成对应的Parser，Parser输出AST
- [ ] 允许注册事件回调，在任意文法Symbol被识别时被调用
- [ ] 可以把生成出来的Parser编译成其他语言的代码，我会写个输出为C++的后端

短期内不会考虑运行效率，算是一次做一百个人的编译原理作业吧XD……

目前设计的使用姿势是：

1. 先写文法描述脚本，描述想要的Parser的上下文无关文法，脚本大概长这样：

   ```
   # 注释以'#'开头
   import [Utility/Utility.agz] # 支持模块导入

   # 支持命名空间
   namespace MyNamespace
   {
   	# 定义非终结符
   	A := Utility.Comma + A; # 使用'.'访问命名空间成员
   	A := Utility.Comma;
   }

   # 定义开始符号
   AGZ_Start := MyNamespace.A;
   ```

2. 然后在自己的程序中调用AGZParserGen的生成接口，得到Parser实例：

   ```cpp
   auto myParser = AGZ::MakeParserFromFile<TokenMapping>(scriptFilename);
   ```

   其中`TokenMapping`是词法单元类型识别器，用来实现文法脚本中的词法单元和进行实际语法分析时使用的词法单元之间的映射，由用户自己指定。当然，我会写个简单的字符串映射版本作为默认。这么设计一是为了更加灵活，二是我不想处理字符编码问题XD……

3. 接下来就想怎么用就怎么用了，可以直接拿来分析，也可以输出静态分析器的代码：

   ```cpp
   // 注册识别到符号"A"时的回调函数
   myParser.Callback(AGZ::ParsingEvent::Parsed, "A", myCallbackFunction);

   //直接分析源代码，得到AST
   auto myAST = myParser.Parse(src);

   //输出成C++静态分析代码
   myParser.PrintAs<AGZ::CppBackend>("staticCppParser.h", "staticCppParser.cpp");
   ```

可以说是非常美好的设想了，尽量在大部分2018年度大三下的编译原理课程设计提交之前完成。