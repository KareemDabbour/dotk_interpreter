var a = [2];
var b = [2]{0};
var c = [2][2];
var d = [2][2]{0};
var e = {1,2,3,4};
var f = {{1},2,[3]{"Hello"}};
var g = e + f[-1];
var h = e[2:];

print(
    "a: " + a,
    "b: " + b,
    "c: " + c,
    "d: " + d,
    "e: " + e,
    "f: " + f,
    "g: "+g,
    "f[0]: "  + f[0],
    "f[-1]: " + f[-1], 
    "e[2:]: " + h,
    "f[True]: " + f[True] # Will throw an indexing error!
    );
