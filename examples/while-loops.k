var x = 3;

while(x>0){
    print("First while loop: " + x);
    x = x - 1;
}

x = 3;

while(True){
    if(x>=0){
        print("breaking at x = " + x);
        break;
    }
    print("Shouldn't be printed");
}

def while_test(k){
    if(k>0){
        while(True){
            if(k == 0){
                ret k;
            }
            k = k - 1;
        }
    }else{
        ret -1;
    }
}

print("While test with 3: " + while_test(3));
print("While test with -3: " + while_test(-3));