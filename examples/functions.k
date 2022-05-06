# A function can be defined using the key-word 'func' followed by the name of the function 

def foo(){
    var k = "From func 'foo'";
    print(k);
};

# Fucntions with arguments can also be defined by passing them in between paranthesis 
# separated by commas. 

def bar(x, y, z){
    print("The three are from the func 'bar'",x, y, z);
};

def foo2(x){
    ret x + 1;
};

def bar2(x){
    ret if(x == 0){
            ret 1;
        }else{
            ret 2.13 * x;
        };
};


print("Function examples");
print("-----foo-----");
foo();
print("-----bar-----");
bar(1, 2, "Third");
print("-----foo2-----");
print(foo2(2));
print("-----bar2-----");
print(bar2(0));
print(bar2(2));