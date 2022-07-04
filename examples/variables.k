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

# Type Casting examples
print("a as a FLOAT: " + float(a));
print("c as an INT: " + int(c));
print("0 as a BOOLEAN: " + bool(0));
print("1 as a BOOLEAN: " + bool(1));
print("b as a BOOLEAN: " + bool(b));
print("[] as a BOOLEAN: " + bool({}));
print("[1, 2, 3] as a BOOLEAN: " + bool({1,2,3}));
print("\"\" as a BOOLEAN: " + bool(""));
print("b as an ARRAY: " + arr(b));