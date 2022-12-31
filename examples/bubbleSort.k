def swap(arr, i, j){
    var temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

def bubbleSort(a){
    var n = len(a);
    for(i : range(n)){
        for(j : range(n - i - 1)){
            if (a[j] < a[j + 1]){
                continue;
            }
            swap(a, j, j + 1);
        }
    }
}

var arr = list(range(50, 0, -1));

print("List before sorting: " + arr);
bubbleSort(arr);
print("List after sorting: " + arr);
