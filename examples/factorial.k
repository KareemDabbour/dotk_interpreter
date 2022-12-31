def fact(x){
    if(x < 2){
        return 1;
    }else{
        return x * fact(x-1);
    }
}

var k = 5;
print(k + "! == " + fact(k));