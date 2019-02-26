# Sourcetrail Language Extension Guide

## First Things First

Build on the shoulders of giants. There are many language tools and frameworks out there that you can build upon, helping you to get things done much faster:

Look for a parser that generates an Abstract Syntax Tree (AST) for the language you want to index. The parser can be written in any language, but for parsing programming languages these tools are mostly written in the language they are designed to parse. You will probably use the generated AST to visit certain nodes (e.g. all `function definition` nodes) and store all the useful information to the Sourcetrail database (e.g. a `function` with name `foo` is defined at location `<X:Y>`).

Having a parser is fine, but it is not enough. When you parse code that contains a function call, the parser may just create a `call expression` node for you. You may even be able to get the name of the called function from the AST node, but the parser certainly will not know where this function is actually defined. And if the indexed code defines two or more functions with the same name, you are in trouble.

That's why it`s best to have a framework that can resolve this info for you. Just to name a few examples:

* for Java there is the [JavaParser](https://github.com/javaparser/javaparser) for building the AST and the [JavaSymbolSolver](https://github.com/javaparser/javasymbolsolver) (now integrated into JavaParser) for resolving these references.
* for C/C++ this is all done by the [Clang](https://clang.llvm.org/) compiler frontend.
* for Python the AST can be built by [parso](https://github.com/davidhalter/parso) while the references can be solved by [Jedi](https://github.com/davidhalter/jedi).

## Getting Started
Ok, let's assume that you found some awesome frameworks that help you build your Sourcetrail Language Package. So what's next?

1. Start off by writing a small executable piece of code that reads in some input code and generates the AST. For the scope of this guide, let's assume that the following piece of Python code would be your snippet.

    <!-- language: python -->
        class Foo:
          def bar():
            return 0

        f = Foo()
        f.bar()

2. Implement an AST visitor that visits each node of the generated AST and prints that node's type. This functionality will come in handy for debugging your code later on. For our piece of sample code, the AST may look like this:

    <!-- language: language-none -->
        file_input
        |-classdef [Foo]
        | `-funcdef [bar()]
        |   `-return
        |     `-number [0]
        |-assignment
        | |-name [f]
        | `-call
        |   `-name [Foo()]
        |-call
        | `-name [bar()]
        |   `-qualifier [f]
        `-endmarker
3. Make the [SourcetrailDB interface](https://github.com/CoatiSoftware/SourcetrailDB#sourcetraildb-api) accessible from your code base. You can either use [existing bindings](https://github.com/CoatiSoftware/SourcetrailDB#supported-language-bindings) for your desired language or open an issue at the [SourcetrailDB issues page](https://github.com/CoatiSoftware/SourcetrailDB/issues). If your language package has made it to this stage, the SourcetrailDB team will be happy to support your project!
4. Use SourcetrailDB from within your code base by opening a `.srctrldb` file before traversing your AST and closing it again after the AST visitor is done.
5. Your AST should already have information available on where certain symbols are defined (e.g. `classdef [Foo]` and `funcdef [bar()]`). Try to extend your AST visitor to store this information to the Sourcetrail Database (e.g. whenever you visit a class or function declaration, try to create a node with the symbol's name (e.g. `Foo` and `Foo.bar`) in the Sourcetrail database).
6. Setup an automated test suite that runs test cases for indexing code snippets with syntax you already covered in your language package. This way you can make sure that you don`t break things as you move forward.
7. Extend your AST visitor to store additional information for the symbols that you already record (e.g. symbol kind, definition kind, source location).
8. Record references between your symbols. Now, this is the tricky part. Within your AST you may encounter a node that describe references to other nodes (e.g. `call`). Use your language processing framework to resolve those references. For our example this language processing framework would need to find out that
    * `bar()` is called on the object in variable `f` and
	* `f` contains an instance of the class `Foo` so
	* `f.bar()` actually describes a call to `Foo.bar()`.
9. If you have made it this far: Congratulations! You already have a solid Sourcetrail language package up and running! It is time to get your language package linked in the [SourcetrailDB Readme](https://github.com/CoatiSoftware/SourcetrailDB#projects-using-sourcetraildb). Just open an issue or create a pull request.
10. Now it's just about extending the package to record more and more stuff and cover the remaining edge cases. Once the package is mature, it may be integrated and shipped with the official Sourcetrail releases, if you agree and if your licensing terms allow.
