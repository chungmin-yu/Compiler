# hw1 report

|Field|Value|
|-:|:-|
|Name|余忠旻|
|ID|0716221|

## How much time did you spend on this project

About 7 hours.

## Project overview

> Describe the project structure and how you implemented it.

這次project有以下這些需要實作出來  
digit\[0-9\] 和 letter\[a-zA-Z\]下面實作會需要用到我先定義出來  
* **Keywords**  
我將所有keywords用or串接寫成一個Regular Expression  
然後我再寫一個TOKEN_KEYWORD的function可以印出\<KWxxxx\>  
  
* **Delimiters**  
我一個一個用Regular Expression表示  
用TOKEN_CHAR的function印出來  
  
* **Arithmetic, Relational, and Logical Operators**  
跟上述delimiters實作類似  
但有一點不同的是假如超過兩個CHAR(含)，我是用TOKEN的function而不是TOKEN_CHAR  
  
* **Identifiers**  
是由letter和digit組成，但開頭必須是letter  
因此Regular Expression是 `{letter}({letter}|{digit})*`  
之後用TOKEN_STRING的function印出<id: xxx>  
  
* **Integer Constants**  
Integer Constants分成 `decimal integer` 和 `octal integer`  
decimal interger是開頭不為0的數字或integer 0  
=>Regular Expression是 `0|[1-9][0-9]*`  
octal integer則是開頭為0的數字，後面接0-7(但integer 0不包含在內)  
=>Regular Expression是 `0[0-7]+`  
這兩個用TOKEN_STRING的function分別印出<integer: xxx>和<oct_integer: xxx>  
  
* **Floating-Point Constants**  
floating-point Constants分為 `integral part` 和 `fractional part`  
integral part也就是上述deciaml integer的部分  
fractional part則是不能有多餘的0，也就是這部分不能以0結尾  
合起來的Regular Expression為 `{integer}\.(0|[0-9]*[1-9])`  
用TOKEN_STRING的function印出<float: xxx>  
  
* **Scientific Notations**  
scientific notations可以用 aEb or aeb 表示  
a是上述decimal integer或floating-point的部分  
b則是deciaml integer，但前面可加+/-符號  
合起來的Regular Expression為 `({integer}|{float})[Ee][\+\-]?{integer}`  
  
* **String Constants**  
string是以頭尾雙引號 `""` 包起來的字串，字串中不包含 `\n`  
當遇到頭尾雙引號`""`包起來的字串中有連續兩個雙引號 `""`  
這時要用一個雙引號表示就好 `"`  
因此Regualar Expression為 `\"([^"\n]|\"\")*\"`  
印出來時不能直接把 yytext 印出來，要去掉頭尾雙引號，並且要處理連續兩個雙引號  
因此我指派一個char array，把yytext值複製到char array  
並檢查連續兩個雙引號做處理  
當複製到 yytext 最後一個char，也就是尾巴雙引號`"`  
這是我會把 `\0` 放進 char array來當作string結束  
最後用TOKEN_STRING的function印出來char array  
  
* **Whitespace**  
這裡先探討 blank spaces 和 tabs，newlines在下面講述  
當遇到 blank spaces 和 tabs 就 discard token  
做LIST就好，把 yytext 放進 buffer  
(在newlines時，source program listing開啟時還是要把whitespace印出來)  
  
* **Newlines**  
當遇到 `\n` 時，如果source program listing有開啟  
則會將buffer印出來並把buffer清空  
  
* **Comments**  
comments有分C-style和C++ -style  
我分別用兩種Start Condition紀錄  
C-style是 `/*` 和 `*/`包裹起來的  
當遇到 `/*` 我就進入C-style的Start Condition，遇到 `*/` 才回到 \<INITIAL\> 的State  
C++-style則是以`//`開頭的one-line comment  
當遇到 `//` 我就進入C++ -style的Start Condition，遇到 `\n` (換行) 才回到 \<INITIAL\> 的State  
在兩種Start Condition中都要做LIST，把 yytext 放進 buffer  
(source program listing開啟時才能把 Comments 印出來)  
  
* **Pseudocomments**  
分別有`//&S+` , `//&S-` , `//&T+` , `//&T-` 四種狀況  
把 `opt_src` , `opt_tok` 設為0或1來控制 `source program listing` , `token listing`的開啟與否  
一樣要做LIST，把 yytext 放進 buffer  
(source program listing開啟時才能把 Comments 印出來)  
  



## What is the hardest you think in this project

***我認為 octal integer 和 string 的部分最難***
1. `octal integer` 要和 `decimal integer` 作區別  
這時就要考慮各種狀況，他們到底是屬於 `octal integer` 還是`deciaml interger`  
  
2. 當`octal integer` Regular Expression 沒寫好時  
它和 `floating-point` 或 `scientific notations`切割token就會錯誤 (這花我不少時間debug)  
  
3. `string` 印出token時要去掉頭尾雙引號，還要處理連續兩個雙引號  
action就要自己實作，不能直接套用上面預定好的function  

## Feedback to T.A.s

> Please help us improve our assignment, thanks.
 
&emsp; <font size=3>No suggestion.</font>
