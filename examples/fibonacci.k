def fib(x){
    ret if (x <= 1){
            ret x;
        }else{
            ret fib(x-2) + fib(x-1);
        };
};

var k = 5;
print("The Fibonacci number at index " + k + " is: " + fib(k));