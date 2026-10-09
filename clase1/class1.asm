.model small
.stack 100h

.data
    
.code
main proc
    mov ax, @data
    mov ds, ax
    mov cx, 5

printLoop:
    mov dl, '*'
    mov ah, 02h
    mov al, '*'
    cmp dl, al 
    int 21h
    loop printLoop

    mov ax, 4C00h
    int 21h

main endp
end main