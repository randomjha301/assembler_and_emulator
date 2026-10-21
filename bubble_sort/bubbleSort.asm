

ldc size    ;outer loop counter
ldnl 0      ;A=5
adc -1      ;A=4

ldc i  ;A=address of i,B=4
stnl 0 ;memory[i]=4

bubbleSort:
    ;ending if i==0
    ldc i   ;loading i into A
    ldnl 0  ;A=outer loop pointer(i)
    brz end ;exiting if A=0(i=0)

    ;resetting j=0
    ldc 0       ; A = 0
    ldc j       ; A = address of j, B = 0
    stnl 0      ; memory[j] = B 

    sort:
        ;ending the loop if j==i
        ldc j   ;loading j address in A
        ldnl 0  ;A=j
        ldc i   ;A=address of i,B=j
        ldnl 0  ;A=i,B=j
        sub     ;A=B-A
        brz bubbleEnd

        ;writing swap logic

        ldc array   ;loading array pointer
        ldc j       ;A=address of j;B=address of array
        ldnl 0      ;A=value of j;B=address of array
        add         ;A=array+j
        a2sp        ;sp=array+j

        ldl 1   ;A=memory[array+j+1]
        ldl 0   ;A=memory[array+j],B=memory[array+j+1]
        sub 

        brlz swap
        br nextj

        swap:
            ldl 0   ;A=array[j]
            ldl 1   ;A=array[j+1],B=array[j]
            stl 0   ;memory[sp]=A(array[j+1])
            stl 1   ;memory[sp+1]=A(array[j])


        nextj:
            ;increaing the inner loop pointer j
            ldc j
            ldnl 0  ;A=j
            adc 1  ;A=j-1
            ldc j   ;A=address of j, B=j-1
            stnl 0  ;memory[j]=B
            br sort
    bubbleEnd:
        ;decreasing the outer loop pointer i
        ldc i
        ldnl 0 ;A=i
        adc -1 ;A=i-1
        ldc i  ;A=address of i, B=i-1
        stnl 0 ;memory[i]=B
        
        br bubbleSort

end:
    HALT



size:   data 5 ;array size
i:      data 0 ;outer loop pointer
j:      data 0 ;inner loop pointer

array:
    data 1
    data 3
    data 2
    data 9
    data 7

