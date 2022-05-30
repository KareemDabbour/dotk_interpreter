def fib_rec(x){
    if(x <= 0){
        ret 0;  
    }

    if (x == 1 || x == 2){
        ret 1;
    }else{
        ret fib_rec(x-2) + fib_rec(x-1);
    }
}

def fib(n){
    var s = [n]{0};
    s[0] = 1;
    s[1] = 1;

    var i = 2;
    while(i<n){
        s[i] = s[i-1] + s[i-2];
        i = i + 1;
    }
    ret s;
}

var k = 13;
print("The Fibonacci number at index " + k + " is: " + fib_rec(k));
print("The first " + k + " Fibonacci numers are: " + fib(k));  