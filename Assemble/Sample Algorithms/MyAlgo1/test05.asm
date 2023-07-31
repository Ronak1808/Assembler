; Program to find sum of n numbers 
a: SET 250  ; a is the value of n
ldc a
stl 0 ; sets sum to top of stack
ldc -1
ldc a ; Sets A = a and B = -1
loop:
add
ldl 0
add
stl 0
stl -1
ldc -1
ldc -1
ldl -1
brlz done
br loop
done:
ldc 1
ldl 0
add  ; at the end A contains the sum of numbers 
HALT