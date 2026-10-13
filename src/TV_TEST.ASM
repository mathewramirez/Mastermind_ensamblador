.model small
.stack 100h
N EQU 4
.data
copiaS DB 1,2,3,4
copiaI DB 1,3,2,4
vacas DB 0
toros DB 0
.code
main proc
mov ax, @data
mov ds, ax
mov cx, N
mov si, 0
cicloToros:
mov al, copiaS[si]
cmp al, copiaI[si]
JNE noEsToro
inc toros
mov copiaS[si],0
mov copiaI[si],0
noEsToro:
inc si
loop cicloToros

mov cx, N 
mov si, 0
cicloAfuera:
mov al, copiaI[si] 
cmp al, 0
je siguienteI ; 
mov  di, 0
cicloAdentro:
cmp al, copiaS[di]
JNE noEsVaca
inc vacas
mov copiaS[DI],0
jmp siguienteI
noEsVaca:
inc di
cmp di, N
jb cicloAdentro
siguienteI:
inc si
loop cicloAfuera

mov dl, toros  
add dl, '0'  
mov ah, 02h  
int 21h

mov dl, vacas
add dl, '0'
mov ah, 02h
int 21h

mov ax, 4C00h  
int 21h  

main endp
end main