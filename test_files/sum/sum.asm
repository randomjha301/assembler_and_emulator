sum:
    ;ending if i==size
    ldc size    ;A=address of size
    ldnl 0      ;A=size
    ldc i   ;A=address of i,B=size
    ldnl 0  ;A=i,B=size
    sub     ;
    brz end

    
    ldc array       ;A=address of array
    ldc i           ;A=address of i,B=address of array
    ldnl 0          ;A=i,B=address of array
    add             ;A=array address + i
    ldnl 0          ;A=array[i]
    ldc answer      ;A=address of answer,B=array[i]
    ldnl 0          ;A=answer,B=array[i]
    add             ;A=answer+array[i]
    ldc answer      ;A=address of answer,B=answer+array[i]
    stnl 0          ;answer=answer+array[i]


    ;incresing loop pointer(i)
    ldc i      ;A=address of i
    ldnl 0     ;A=i
    adc 1     ;A=i+1
    ldc i      ;A=address of i,B=i+1
    stnl 0     ;memory[i]=i+1
    br sum
   

end: 
    HALT

size: data 5
i: data 0           ;inner loop pointer
answer: data 0      ;final answer
array:
    data 1
    data 2
    data 3
    data 4
    data 5