def fib_rec(x){
    if(x <= 0){
        return 0;  
    }

    if (x == 1 || x == 2){
        return 1;
    }else{
        return fib_rec(x-2) + fib_rec(x-1);
    }
}

def fib(n){
    var s = [n]{0};
    s[0] = 1;
    s[1] = 1;

    for(i : range(2,n)){
        s[i] = s[i-1] + s[i-2];
    }
    return s;
}

var k = 13;
print("The Fibonacci number at index " + k + " is: " + fib_rec(k));
print("The first " + k + " Fibonacci numers are: " + fib(k));  