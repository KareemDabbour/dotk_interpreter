# While loops examples
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
                return k;
            }
            k = k - 1;
        }
    }else{
        return -1;
    }
}

print("While test with 3: " + while_test(3));
print("While test with -3: " + while_test(-3));

# For each loop examples 

var stringArr = {"first", "second",  "third", "last?", "last"};

for(string : stringArr){
    if(string == "last?"){
        continue;
    }
    print(string);  
}

print("For each with break:");
for(x : range(10)){
    print(x);
    if(x == 5){
        break;
    }
}


print("For each with continue:");
for(x : range(10)){
    if(x == 5){
        continue;
    }
    print(x);
}

print("Nested for each loops:");
for(x : range(2)){
    print("outterLoop: " + x);
    for(y : range(2)){
        print("innerLoop: " + y);
    }
}
