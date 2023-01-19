var BUFF_SIZE = 100;
var ITERATIONS = 100;

# Rule 110 cell rules:
var table = {
    0,
    1,
    1,
    1,
    0,
    1,
    1,
    0
};

# Creating cell buffer:
var buffer = [BUFF_SIZE]{0};

# Seed original generation:
buffer[-1] = 1;

# Getting the corresponding cell rule index 
def get_index(a, b, c){
    return a * 4 + b * 2 + c;
}

# Printing each generation
def print_gen(buff){
    var ret = "";
    for (cell : buff){
        if (cell){
            ret = ret + "|";
        }else{
            ret = ret + " ";
        }
    }
    print(ret);
}


def gen(input){
    var output = [BUFF_SIZE]{0};
    var index = -1;
    var a = 0.0;
    var b = 0;
    var c = 0;
    for (i : range(-2, BUFF_SIZE - 2) ){
        a = input[i];
        b = input[i + 1];
        c = input[i + 2];
        output[index] = table[get_index(a, b, c)];
        index = index + 1;
    }
    return output;
}

for(i : range(ITERATIONS)){
    print_gen(buffer);
    buffer = gen(buffer);
}
