def fact(x){
    if(x < 2){
        ret 1;
    }else{
        ret x * fact(x-1);
    }
}

var k = 5;
print(k + "! == " + fact(k));