# Dot-K Programming Language

## Description
`Dot-K` is a basic dynamically typed  interpreted language written in C. 

*To read more about the implementation, please see the `Implementation` section.*

## Math
`Dot-K` Can handle very basic math,  including addition, subtraction, multiplication, division and integer division.
It naturally follows the order of operations unless specified otherwise with parentheses.
```py
+  # Addition
-  # Subtraction
*  # Multiplication
/  # Division
// # Integer Division
```

## Printing 
Using the builtin function `print`, you can print to the standard output.

The following code is an example of how to use the `print` function:
```py
print("This is printing a string");
print(41, 3.123, 1==1);
```

More print examples can be seen in `examples/print.k` 

## Variable Declarations
Float, Int, String and Boolean variables can be declared by using the key word `var`.

The following code is an example of how to declare an int var `x`:
```js
var x = 41;
```
More variable declaration examples can be seen in `examples/variables.k` 

## Function Definitions
Using the key word `def`, various functions can be defined with the `Dot-K` language.

### Functions without Arguments
The following code is an example of how to define and  call a function `foo`:
```python
def foo(){
    print("This is function 'foo'");
};

foo();
``` 

### Functions with Arguments
The following code is an example of how to define and  call a function `foo`:
```python
def foo(x){
    print("This is function 'foo' " + x);
};

foo("with arguments!");
``` 

### Functions with Return Statements
The following code is an example of how to define and  call functions `foo` and `bar` that take advantage of the `ret` key word to return data:
```python
def foo(x){
    ret x + 1;
};

def bar(x){
    ret if(x == 0){
            ret 1;
        }else{
            ret 2.13 * x;
        };
};

print(foo(2));
print(bar(0));
print(bar(2));
```
*Please note that you currently must return out of every nested if-else statement* 

More function definition examples can be seen in `examples/functions.k` 


## if-else Statements
Much like many other programming languages `Dot-K` has support for `if-else` statements. 

The following code is an example of how to use them:
```py
if("a" < "z"){
    print("'a' is less than 'z'");
}else{
    print("Something is wrong!");
};

if("a" > "z" || 1 == 1){
    print("'a' > 'z' OR 1 == 1");
}else{
    print("Something is wrong!");
};

if("a" > "z" && 1 != 1){
    print("'a' > 'z' AND 1 != 1");
}else{
    print("'a'<'z' or 1 == 1");
};
```


## Future Features
### Unary Operators
There is currently no way to express `-2` explicitly since `-` only represents a binary subtraction. 

### Arrays & Array-like Structures
There is currently no way to declare an array. However, it's in the works!

### While and for loops
Since there are no arrays or array-like data structures in this language there was no real need for `while` and `for` loops. However, this is being worked in conjuntion with `arrays`.

### Call stack
Currently, there is no call stack, so the most recent argument variable will be used rather than the appropriate value. This affects recursive functions that have multiple calls to themselves -- for example the function to calculate the nth fibonacci term. 


## Implementation

### Lexing
Looking at the input file as one entire string, individual tokens are returned depending on the current `char` that the lexer sees. For example, if a `~` is encountered, all chars are ignored until another `~` is read -- this allows for a block comment. This logic extends to lexing `ints`, `floats` and `operators`. 

For example, the following code
```js
var x = 12;
```
would be lexed to the following list of tokens
```js
[Token("var", ID_TOKEN), Token("x", ID_TOKEN), Token("=",EQ_TOKEN), Token("12", INT_TOKEN), Token(";", SEMI_TOKEN)]
```

### Parsing
Proceeding the lexing process tokens are looked at one at a time and parsed into Abstract Syntax Trees (ASTs). This structures the data into trees that when transverse, execute the code as intended.

For example, the two following expressions would be expressed as such: 
![AST Example](./README/imgs/AST_example.png)


### Visiting
Now that we have our ASTs lined up, all that's left is to transverse them properly. This is done by controlling whether we use an inorder, preorder or postorder to visit the AST subtree of a given node.  

## Installing & Running
From the root directory, run the following:
```bash 
make install 
dotk example/factorial.k
```

_NOTE: This was meant to be run on a debian based system. Edit the `Makefile` fit your system._