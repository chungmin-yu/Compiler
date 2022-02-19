# hw5 report

|Field|Value|
|-:|:-|
|Name|余忠旻|
|ID|0716221|

## How much time did you spend on this project

About 4 days. (totally about 35 hours)

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.

這次作業是 visit AST tree，並在每個 node 輸出對應的 RISC-V assembly code  
主要是修改 `/include/codegen/CodeGenerator.hpp` 和 `/lib/codegen/CodeGenerator.cpp`  
在一開始，要做 Initialization  
Function 除了 Function body 要產生 assembly 之外，還需要 **`Function Prologue`** 和 **`Function Epilogue`** 的幫忙  
Function Prologue: 在一開始，allocate 一段空間來存放function的local variable 並把一些register的值(ra, s0)存起來  
Function Epilogue: 在 return 之前，要歸還先前 allocate 的空間並把 register 的值(ra, s0)還原  

在 CodeGenerator中，我將這兩個寫成 function ，方便之後呼叫，並且添加一個 offset 的 member variable，來記錄 stack 中的 offset變化  
這次作業有說 allocate 128 bytes of local memories is sufficient for the parameters and the local variables.  
所以在`Function Prologue` 和 `Function Epilogue` 中就可以直接 allocate 128 bytes  

接著，我按照 spec 一步一步完成，  
有一些 node 中，我沒用 `visitChildNodes()` ，而是將其實作內容稍作修改寫在 node 中，  
因為我想在各個 `accept()` function 中插入對應的 assembly code，這樣比較好實作  
其中，我用 label 來記錄要產生的 Label 數，並用對應的 Label Number 在相對應的 assembly code 位置中產生 Label  
另外，我用 local 來記錄 variable 是 global 還是 local，因為 global variable 和 local variable 產生的 assembly code 不同  
在 spec 中的 *Declarations of Variables and Constants* 有寫  
CodeGenerator 的 member variable 中，我還用 isRead 來判斷是否要做 read functions  

## What is the hardest you think in this project

> Not required, but bonus point may be given.

1. 這次作業整體我覺得比上次好理解，spec 和投影片蠻好理解的，但實作起來蠻難的，因為沒有給測資對應的 assembly code， solution 只有給執行結果，有時會不知道對應的 assembly code 長怎樣，  
這時就要用 `riscv32-unknown-elf-gcc -c -S [input C file] -o [output assembly file]` ，  
先產生對應 C language 再轉成 RISC-V 的 assembly code 觀察   

2. 在 bonus 部分，我寫 `stringtest` 有遇到 `User fetch segfault` 的問題，執行答案是對的，但最後就會一直出現 `User fetch segfault`，  
最後我才發現是因為先前測資沒測到 `FunctionInvocation` 的回傳值是 `void` type，所以我多做一些指令導致 `User fetch segfault`  

3. 在 bonus 部分我覺得相對困難許多，  
Array type 沒有想法怎麼處理，Real type 則是因為要 `lw, sw` 要修改成 `flw, fsw` 操作，  
整體操作(許多 node)變成要先判斷是否 Real type 來決定用何者指令，我沒想到好的寫法，所以我也沒做  

4. &ensp; Q: Is the spec and the tutorial clear? Is hw5 too easy?  
&emsp;A: 我覺得 spec 在 basic case 部分蠻清楚的，但在 advance case 和 bonus case 就感覺提示有點少，沒有很多頭緒怎麼操作的  
&emsp;&emsp; 難度方面我覺得主要難在 RISC-V 架構和指令的理解，整體難度比 hw3 和 hw4 簡單一點  

## Feedback to T.A.s

> Not required, but bonus point may be given.

&emsp; <font size=3>No suggestion.</font>  
