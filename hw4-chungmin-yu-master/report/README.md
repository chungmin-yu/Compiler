# hw4 report

|Field|Value|
|-:|:-|
|Name|余忠旻|
|ID|0716221|

## How much time did you spend on this project

About 4 days. (totally about 35 hours)

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.


這次因為上次已經做完 AST 了，所以整個 tree 已經建立起來了  
這次是要把 sematic analysis 做出來  
而每個 node 在 `SemanticAnalyzer.cpp` visit 時會做以下五件事  
(不一定每個 node 五件事都會做到)  
* 1. Push a new symbol table if this node forms a scope.  
* 2. Insert the symbol into current symbol table if this node is related to declaration .  
* 3. Travere child nodes of this node.  
* 4. Perform semantic analyses of this node.  
* 5. Pop the symbol table pushed at the 1st step.  

為了把 symbol table 建立起來，我根據助教提示創了 `SymbolTable.[hc]pp`   
`SymbolTable.[hc]pp` 中會保存且維護 `Name`, `Kind`, `Level`, `Type`, `Attribute` 這些資訊  
在 `SymbolTable.hpp` 中，我創立了五個 class，  
有三個是拿來做 symbol table 的，分別是 `SymbolEntry`, `SymbolTable`, `SymbolManager`  
另外兩個是拿來做 function 的 sematic analysis，分別是`FunctionEntry`, `FunctionManager`  


當 symbol 進入它的 scope 時，會呼叫 `pushScope`，  
把 symbol 放進 symbol table  
(假如沒有 symbol table 就創一個新的 symbol table 把 symbol 放進去)  
當 symbol 離開它的 scope 時，會呼叫 `popScope(opt_symbol)`，  
會看有沒有開啟 `Pseudocomments D` (Dump the contents of the symbol table)，  
有開啟的話會把 symbol table print 出來，並且把 symbol 把 pop 出來  
symbol 會離開它的 scope 時機點會在 `ProgramNode`, `FunctionNode`, `CompoundStatementNode`, `ForNode` 這四個 node 結束時  

而 symbol 在它的 scope 時，會 traverse child nodes 和 perform sematic analysis  
traverse 的部分上次作業 AST 就完成了  
sematic analysis 我則是依照 spec 和 測資一步一步做出來  

其中我在 `scanner.l` 和 `parser.y` 中，我有修改了一些部份來符合這次 spec :  
* 在 `scanner.l` 中，我加了 `opt_symbol` 來完成上述說的 `Pseudocomments D` (Dump the contents of the symbol table) 功能  
* 在 `scanner.l` 中，我創了 `char code[MAX_LINE_NUM][MAX_LINE_LENG]` 的二維 array，用來存 input 的 source code，當 sematic analysis 發現有錯誤時，就可以把 source code 有錯誤的地方print 出來  
* 在 `parser.y`中，我創了`bool error` 的 flag，來記錄做 sematic analysis 時是否有錯，有錯的話最後就不能印出 `There is no syntactic error and semantic error!` 這行  
* 在 Fornode 部分，`m_end_condition` 的 type 我從ExpressionNode 修改成 ConstantValueNode (因為我看助教的 parser.y 是傳 ConstantValueNode)，除此之外我加了`m_left` 的 member，用來存 for 迴圈初始值(左邊的數值)，來判斷有沒有`the lower bound and upper bound of iteration count must be in the incremental order` 的錯誤，這樣在第12筆測資 `12_ForLoop.p` 就能順利找到錯誤  
* 在 `parser.y`中，我額外令了`bool inFunction` 的 flag，來判斷現在是在 `function內` 還是在 `program內function外` 來處理 return 的 sematic analysis 問題，這樣在第13筆測資 `13_ret.p` 就能順利找出 `program/procedure should not return a value` 的錯誤  



## What is the hardest you think in this project

> Not required, but bonus point may be given.

1. 這次作業整體我覺得跟上次一樣都蠻難的，一開始我看 spec 就看很久，加上要讀懂就花費不少時間，然後研究整個架構怎麼運作也花很久時間  

2. 因為我平常不會用 `std::unique_ptr` 和 `std::unique_ptr`，所以要理解這個的用途，我花了不少時間上網查資料了解，除此之外因為做 sematic analysis 時，有些會需要用到 `std::unique_ptr` 或 `std::unique_ptr` 的 member 資訊，所以有需要用到 member 我會將它從 private 改成 public，方便我在`SemanticAnalyzer.cpp` 的 `class SemanticAnalyzer` 存取  

## Feedback to T.A.s

> Not required, but bonus point may be given.

&emsp; <font size=3>No suggestion.</font>  
