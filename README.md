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

More math examples can be seen in `examples/math.k`

## Printing 
Using the builtin function `print`, you can print to the standard output.

The following code is an example of how to use the `print` function:
```py
print("This is printing a string");
print(41, 3.123, 1==1);
```

You can also format strings with the `+` operator. For example:
```py
print("It is " + 1==1 + " that 1 is equal to 1");
# Output: "It is True that 1 is equal to 1"

print("It is " + 2==1 + " that 2 is equal to 1");
# Output: "It is False that 2 is equal to 1"
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
# Output: This is function 'foo'
``` 

### Functions with Arguments
The following code is an example of how to define and  call a function `foo`:
```python
def foo(x){
    print("This is function 'foo' " + x);
};

foo("with arguments!");
# Output: This is function 'foo' with arguments!
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

print(foo(2)); # Output: 3
print(bar(0)); # Output: 1
print(bar(2)); # Output: 4.26
```
*Please note that you currently must return out of every nested if-else statement* 

### Functions and Recurssion
The following code is an example of how `Dot-K` can handle simple recursion.
```py js
def fact(x){
    ret if(x < 2){
            ret 1;
        }else{
            ret x * fact(x-1);
        };
};
print("5! == " + fact(5));
# Output: 5! == 120
```
*If you would like to see more, take a look at `examples/factorial.k` as well as `examples/fibonacci.k`*

More function definition examples can be seen in `examples/functions.k` 


## if-else Statements
Much like many other programming languages `Dot-K` has support for `if-else` statements. 

The following code is an example of how to use them:
```py
# Output: 'a' is less than 'z'
if("a" < "z"){
    print("'a' is less than 'z'");
}else{
    print("Something is wrong!");
};

# Output: 'a' > 'z' OR 1 == 1
if("a" > "z" || 1 == 1){
    print("'a' > 'z' OR 1 == 1");
}else{
    print("Something is wrong!");
};


# Output: 'a'<'z' OR 1 == 1
if("a" > "z" && 1 != 1){
    print("'a' > 'z' AND 1 != 1");
}else{
    print("'a'<'z' OR 1 == 1");
};
```


## Future Features

### Arrays & Array-like Structures
There is currently no way to declare an array. However, it's in the works!

### While and for loops
Since there are no arrays or array-like data structures in this language there was no real need for `while` and `for` loops. However, this is being worked in conjuntion with `arrays`.

## Implementation

### Lexing
Looking at the input file as one entire string, individual tokens are returned depending on the current `char` that the lexer sees. For example, if a `~` is encountered, all chars are ignored until another `~` is read -- this allows for a block comment. This logic extends to lexing `ints`, `floats` and `operators`. 

For example, the following code
```js
var x = 12;
```
would be lexed to the following list of tokens
```js
[
    Token("var", ID_TOKEN), 
    Token("x", ID_TOKEN), 
    Token("=",EQ_TOKEN), 
    Token("12", INT_TOKEN), 
    Token(";", SEMI_TOKEN)
]
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
sudo make install 
dotk example/factorial.k
```

_NOTE: This was meant to be run on a debian based system. Edit the `Makefile` fit your system._