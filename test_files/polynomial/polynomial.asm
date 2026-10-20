;computing ax^2 + bx + c

;creating ax^2
ldc a   ;A=address of a
ldnl 0  ;A=a 
ldc x   ;A=address of x,B=a
ldnl 0  ;A=x,B=a 
mul     ;A=x*a 
ldc x   ;A=address of x,B=x*a
ldnl 0  ;A=x,B=x*a
mul     ;A=x*x*a 

;storing ax^2 in ans
ldc ans ;A=address of ans,B=ax^2
stnl 0  ;memory[ans]=ax^2

;creating bx
ldc x   ;A=address of x
ldnl 0  ;A=x
ldc b   ;A=address of b,B=x
ldnl 0  ;A=b,B=x
mul     ;A=x*b 

;adding bx in ans
ldc ans ;A=address of ans,B=x*b 
ldnl 0  ;A=ans,B=x*b 
add     ;A=ax^2+bx

;storing ax^2+bx in ans
ldc ans ;A=address of ans,B=ax^2+bx
stnl 0  ;memory[ans]=ax^2+bx 

;adding c in ans
ldc ans ;A=address of ans
ldnl 0  ;A=ans 
ldc c   ;A=address of c,B=ans
ldnl 0  ;A=c,B=ans
add     ;A=ax^2+bx+c 

;storing ax^2+bx+c  in ans
ldc ans ;A=address of ans,B=ax^2+bx+c
stnl 0  ;memory[ans]=ax^2+bx+c 

HALT



a: data 2
b: data 3
c: data 4
x: data 3
ans: data 0
