var a = 41;
var b = "String Var";
var c = 3.14;
var d = 1 < 3;
var e = a + 1;
var f = False;
var g = NULL;
print(
    "a: " + a, 
    "b: " + b, 
    "c: " + c, 
    "d: " + d, 
    "e: " + e, 
    "f: " + f, 
    "g: " + g
    );

print("-------------");

print("f = " + f);
print("Running: f = f || True");
f = f || True;
print("f = " + f);
print("f || True: " + f);