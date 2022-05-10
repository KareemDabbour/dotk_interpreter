def fact(x){
    ret if(x < 2){
            ret 1;
        }else{
            ret x * fact(x-1);
        };
};
print("3! ==");
print(fact(3));