def fizzbuzz(n){
    print("Fizzbuzz from 1 -- " + n);
    var fz = NULL;
    for(x : range(1, n)){
        fz = "";
        if ((x%3) == 0){
            fz = fz + "fizz";
        }
        if ((x%5) == 0){
            fz = fz + "buzz";
        }
        if(fz){
            print(fz);
        }else{
            print(x);
        }
    }   
}

fizzbuzz(16);