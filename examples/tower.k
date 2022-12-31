def tower(x, src, dest, aux){
    if(x==0){
        return;
    }
    tower(x-1, src, aux, dest);
    print("Move the disk " + x + " from " + src + " to " + dest);
    tower(x-1, aux, dest, src);
}

tower(3,"SOURCE","DESTINATION","AUXILIARY");