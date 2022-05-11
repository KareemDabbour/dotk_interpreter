def fib(x){
    
    if(x <= 0){
      ret 0;  
    };

    if (x == 1 || x == 2){
        ret 1;
    }else{
        ret fib(x-2) + fib(x-1);
    };
};

var k = 13;
print("The Fibonacci number at index " + k + " is: " + fib(k));