data 1  ; Linear search program 
data 2
data 3
data 4
data 5
data 6
data 7
size: SET 7   ;Size of the input array 
ldc 1 
ldc size 
search: SET 8  ; Set the value to be searched for 
find:
ldl 0
ldc size
sub
brz notFound  ; If reached past the end jump to notFound
ldl 0
ldnl 0
ldc search
sub
brz found ; If Found jumps to found
ldc 1
ldl 0
add 
stl 0
br find
found:
ldc 1
ldc 1
HALT
notFound:
ldc 0
ldc 0
HALT 
;If element is found A and B are both set to 1 and if not foudn both are set to 0

