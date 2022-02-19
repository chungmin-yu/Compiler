# hw2 report

|Field|Value|
|-:|:-|
|Name|余忠旻|
|ID|0716221|

## How much time did you spend on this project

About 9 hours.

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way. \
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here. 

1. 在 **`scanner.l`** 中，要先加上要 return 的 token ，這樣才能在下面的 **`parser.y`** 中使用  

2. 實作 **`parser.y`** 一開始需將之前在 **`scanner.l`** 中 return 的 token 宣告在上面為 `%token {$token}` ，  
再開始完成下述作業要求的`Syntactic Definitions` 

3. **`Syntactic Definitions`**： 
    - [Program Units](#Program-Units) 
      - [Program](#Program) 
      - [Function](#Function) 
    - [Data Types and Declarations](#Data-Types-and-Declarations) 
      - [Variable and Constant](#Variable-and-Constant) 
    - [Statements](#Statements) 
      - [Compound](#Compound) 
      - [Simple](#Simple) 
      - [Conditional](#Conditional) 
      - [While](#While) 
      - [For](#For) 
      - [Return](#Return) 
      - [Function Call](#Function-call) 
    - [Expressions](#Expressions) 

4. 按照作業規定一條一條rule慢慢加進去  

### Program Units  

#### Program  

直接依照作業規定把他轉 CFG rules 為  

```
Program			: IDENTIFIER ';' Variables Functions Compound END
			;
```

因為`<zero or more variable and constant declaration>`  
以及`<zero or more function declaration and definition>`，  
我將 `Variables` 表示為 0 或多個的 variable and constant declaration  
和 `Functions` 則表示為 0 或多個的 function declaration and definition  

#### Function 

Function可以分為兩種: `function declaration` or `function definition`  
這兩種各有兩種狀況，可寫成:  

```
Function_declaration 	: IDENTIFIER '(' Argument_list ')' ':' TYPE ';' 
			| IDENTIFIER '(' Argument_list ')' ';'
			;

Function_definition 	: IDENTIFIER '(' Argument_list ')' ':' TYPE Compound END 
			| IDENTIFIER '(' Argument_list ')' Compound END
			;
```

需要注意的是上述 [Program](#Program) 提到的 0 或多個 Function，所以要多加這條rule:  

```
Functions		: Functions Function
			| Function
			;
```

接著上述會用到的`Argument_list` 的 nonterminal  
一開始會先判斷是否為 epsilon，  
若不是的話則進到第二層，第二層就是負責一個或是多個 TYPE 的判斷  
前面 `Argument` 部分以 `Identifier_list` 表示，  
後面接冒號和 TYPE，有多個 `Arguments` 則需用分號隔開  

### Data Types and Declarations 

我將下述兩種合併為 `Variable_declaration` 的 nonterminal  
方便前面 [Program](#Program) 的實作  
並且跟Function一樣，實作中會用到 0 或多個 Variable_declaration，所以要多加類似的rule  

#### Variable and Constant
依作業規定可寫成下列 CFG rules:  
其中 `Literal_constant` 的 nonterminal 來表示 a constant of the proper type  
(e.g., an integer/real literal with or without a negative sign, a string literal, true, or false)  

```
Variable_declaration	: VAR Identifier_list ':' TYPE ';'
			| VAR Identifier_list ':' ARRAY INTEGER OF ARRAY_TYPES ';'
			| VAR Identifier_list ':' Literal_constant ';'
			;

Literal_constant 	: INTEGER | OCTAL | FLOATINGPOINT | SCIENTIFIC  
			| STRING
			| TRUE
			| FALSE
			;
``` 
### Statements 
Statements 可分成七種:  
compound, simple, conditional, while, for, return, function_call  
下面會一一實作出來  
其中 `Statement` 也要跟上述 `Function` 以及 `Variable_declaration`  
需要多加一條rule來表示 0 或多個 `Statement` ，下面會用到  

#### Compound 
依作業規定可寫成:  
```
Compound 		: BEG Variables Statements END 
			;
```
這裡就有用到剛剛提到的 zero or more statements 這條rule  

#### Simple 
依作業規定 simple 總共有四種可能:  
```
Simple 			: Variable_reference ASSIGN Expression ';'
            		| PRINT Expression ';'
            		| PRINT Variable_reference ';'
        		| READ Variable_reference ';'
            		;
```

其中`variable_reference` 可為 identifier 或者 array_reference  
array_reference 是用中括號包起來的 0 或多個 `Expression`  
( [Expressions](#Expressions)下面會講到)  

#### Conditional 
依作業規定寫就能順利完成，這裡就不加贅述  
#### While 
依作業規定寫就能順利完成，這裡就不加贅述  
#### For 
依作業規定寫就能順利完成，這裡就不加贅述  
#### Return 
依作業規定寫就能順利完成，這裡就不加贅述  

#### Function call 
這裡需要注意的是雖然作業規定是  
`identifier(<zero or more expressions, separated by commas>);`
但我沒把分號放在 `function_call` 的 production body 中  
因為後面 `Expression` 會用到 function call without the semicolon，  
所以我將分號移到 `Statement` 的 production body 中 `function_call` nonterminal後面  

=> *`Statement -> Function_call ';'`*  

### Expressions
Expression 有四種可能，分別為:  
`a literal constant`, `a variable reference`, `a function call without the semicolon`, `an arithmetic expression` 
前三個上述都已經實作完成，可以直拿下來使用  

實作`arithmetic expression`時，其中需要注意  
1.  Associativity is left. 所以我們一開始要加 `%left {$token}` 在前面  
2.  token "-" 為 unary negation operator 時，優先權要提高  
3.  Parentheses 用來 group subexpressions 時，優先權也要提高  

## What is the hardest you think in this project

> Not required, but bonus point may be given.

* 剛開始在 make 時，報了一堆 error， 
其中讓我debug最久的是  
我在`scanner.l` 中讀到 `begin` 時，是 return `BEGIN`  
結果就噴了一大堆error，  
其中包含下列圖片顯示錯誤加一堆 `scanner.l` 的 undeclared錯誤  
![](https://i.imgur.com/BquRWMw.jpg)  
我找很久後才發現把 return `BEGIN` 改成 return `BEG` 就可以執行了  

* 在寫`parser.y` 時也有遇到問題，主要是文法寫錯，  
我則是比對sample_solution 以及在 production body加 printf 的 action 來找出自己寫錯的bug  


## Feedback to T.A.s

> Not required, but bonus point may be given.

&emsp; <font size=3>No suggestion.</font>

