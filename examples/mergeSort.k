def mergeSort(arr){
    if(len(arr) <= 1){
        return arr;
    }
    var mid = len(arr) // 2;    
    var left = mergeSort(arr[:mid]);
    var right = mergeSort(arr[mid:]);
    return merge(left, right);
}

def merge(l, r) {
    var result = [0];
    var i = 0;
    var j = 0;
    var index = 0;
    while((i < len(l)) && (j < len(r))){
        if(l[i] < r[j]){
            result = result + {l[i]};
            i = i + 1;
        }else{
            result = result + {r[j]};
            j = j + 1;
        }
    }
    result = result + l[i:];
    result = result + r[j:]; 
    return result;
}

var arr = list(range(500, 0, -1));

print("List before sorting: " + arr);
print("List after sorting: " + mergeSort(arr));