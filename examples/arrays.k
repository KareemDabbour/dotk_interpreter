var a = [2];
var b = [2]{0};
var c = [2][2];
var d = [2][2]{0};
var e = {1,2,3,4};
var f = {{1},2,[3]{"Hello"}};


print(
    "a: " + a,
    "b: " + b,
    "c: " + c,
    "d: " + d,
    "e: " + e,
    "f: " + f,
    "f[0]: "  + f[0],
    "f[-1]: " + f[-1], 
    "f[True]: " + f[True] # Will throw an indexing error!
    );
