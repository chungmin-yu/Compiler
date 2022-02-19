# hw3 report

|Field|Value|
|-:|:-|
|Name|余忠旻|
|ID|0716221|

## How much time did you spend on this project

About 5 days. (totally about 40 hours)

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.

### 架構 

* `scanner.l` :  會將 token 存到 yylval 讓 parser使用，有identifier, int_val, float_val 三種type  

* `parser.y` : 實作AST，首先在 `Code Directive` 部分，要在`%code requires {}` 中加上需要用到的標頭檔(包含 include/AST/*.hpp 和 \<vector\> )， 接著在 `union` 部分，`%union{}` specify the entire collections of possible data types for semantic values stored in variable yylval，我們要把在CFG會用到的 data type先設好，在 `%type`中宣告 token 和 nonterminal 的 type，之後在CFG部分，把會用到 node 呼叫出來並對相應的 node 做相應的事  

* `include/AST/*.hpp` 和 `lib/AST/*.cpp` : 這裡是實作node的細節部分，包含把資訊 print 出來，以及做 visitor pattern，在 visitor pattern 部分，Node 都是繼承 AstNode，因此為了要做 `visit`，我在 `ast.hpp` 中加入兩個 vitual function(分別為 `accept` 和`visitChildNodes`)，給其他 Node `override` 來實作出 visitor pattern  

* `Makefile` : 因為 visitor pattern 會用到 `include/visitor/AstNodeVisitor.hpp` 和 `lib/visitor/AstNodeVisitor.cpp` ，所以 Makefile 要加入這幾行，才能在 make 時將它 link 起來  
```
VISITORDIR = lib/visitor/ 
VISITOR := $(shell find $(VISITORDIR) -name '*.cpp') 

SRC := $(AST)\ 
	$(VISITOR) 
``` 

### 流程 

* 一開始，AstDumper visitor 會 `visit` ProgramNode *root，所以在第一個 CFG 我們就先建出 ProgramNode， 並依照 hint 加入該加入的memeber 和 member function, AstDumper 的 visit function 會依據多型做不同的事，一開始是 ProgramNode ，所以會呼叫 ProgramNode 的 visitChildNodes function ，這時我們要 `override` 它讓它往下 `visit`  

* 第一筆測資是 PragramNode 和 CompoundStatementNode，我就繼續把 CompoundStatementNode 做完，一樣依照 hint 加入該加入的memeber 和 member function, 並 `override` accept 和 visitChildNodes function  

* 接下來我就按看著測資一步一步改其餘的 Node，其中我將 `LiteralConstant`, `StringAndBoolean`, `IntegerAndReal` 這三種 Nonterminal 的 data type 設為自己創的 `struct constValue` ，用來存各種constant value 的 type 值，是哪一種 type 和 column值 也存起來，並將 `struct constValue` 宣告在 `ast.h`，在 DeclNode 和 ConstantValueNode 就方便實作了  

* 在 `Type` `ArrType` `ArrDecl` 這三個 Nonterminal 我設為 `vector<string>` 的 data type，為了存 array type 的資訊，vector 的第一個 element 是存 array 屬於哪一種 type，接下來的 element 就是依序存 array 的大小 (如果是 scalarType 就只會有一個element)，這樣在 VariableNode 的 getType function 中就可以取出正確的 type 資訊  

* `IdList` Nonterminal 的 data type 也是用 `vector<string>`，是用來存 ID 和 column值 (index 偶數是存 ID， index 奇數是存 column值)，這樣之後就能把 `IdList` 用 `vector<string>` 傳進 DeclNode 做使用  


## What is the hardest you think in this project

> Not required, but bonus point may be given.

1. 這次作業整體我覺得都蠻難的，一開始我看 spec 就看很久，加上要讀懂就花費不少時間，然後研究整個架構怎麼運作也花很久時間  

2. 由於我對 OOP 概念沒有很熟悉，所以在用到 OOP 繼承概念需要上網找資料重新了解，雖然花了不少時間，但做完作業同時也重新複習許多 OOP 概念  

3. 在有一些特別的資料型態要存起來再傳到 Node，這裡讓我研究了一陣子，因為要思考 Nonterminal 應該屬於哪一種 data type，存哪些資料。除此之外，因為我存資料會用到一些 type 轉換(像是 const char* 和 string 轉換、 int 和 string 轉換)，很常 type 沒對應好就 `make` 失敗了，我有特地上網查了一下資料來了解怎麼轉換  

## Feedback to T.A.s

> Not required, but bonus point may be given.

&emsp; <font size=3>No suggestion.</font>