def mergeSort(arr){
    if(len(arr) <= 1){
        return arr;
    }
    var mid = len(arr) // 2;

    var left = arr[:mid];    
    var right = arr[mid:];
    mergeSort(left);
    mergeSort(right);
    merge(arr, left, right);
}

def merge(arr, left, right) {
    var i = var j = var index = 0;

    while((i < len(left)) && (j < len(right))){
        if(left[i] < right[j]){
            arr[index] = left[i];
            i = i + 1;
        }else{
            arr[index] = right[j];
            j = j + 1;
        }
        index = index + 1;
    }
    while (i < len(left)){
        arr[index] = left[i];
        i = i + 1;
        index = index + 1;
    }
    while (j < len(right)){
        a[index] = right[j];
        j = j + 1;
        index = index + 1;
    }
}

var arr = list(range(100000, 0, -1));

print("List before sorting: " + arr);
mergeSort(arr);
print("List after sorting: " + arr);