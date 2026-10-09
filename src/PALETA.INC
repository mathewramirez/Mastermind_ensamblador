; GENERADO por herramientas/sprites2asm.py - NO editar a mano
; Cada entrada: R,G,B en escala VGA 0..63 (el DAC del VGA usa 6 bits: 8 bits >> 2)
PALETA_INICIO   EQU 16
PALETA_CANT     EQU 37
C_FONDO          EQU 16
C_PANEL          EQU 17
C_FILA           EQU 18
C_BORDE          EQU 19
C_TEXTO          EQU 20
C_ACENTO         EQU 21
tablaPaleta LABEL BYTE
        DB  2, 5,12   ;  16 fondo #0B1530
        DB  5, 9,19   ;  17 panel #14264F
        DB  6,12,24   ;  18 fila #1B3060
        DB 15,27,46   ;  19 borde #3D6FB8
        DB 58,59,61   ;  20 texto #E8ECF4
        DB 52, 7, 7   ;  21 acento #D21E1E
        DB 17, 7, 2   ;  22 spr_cafe #461E0A
        DB 35,17, 7   ;  23 spr_cafe #8C461E
        DB 55,45,30   ;  24 spr_cafe #DCB478
        DB 10, 3, 1   ;  25 spr_cafe #280F05
        DB  5, 5, 5   ;  26 spr_carreta #141414
        DB 63,52, 0   ;  27 spr_carreta #FFD200
        DB 63,63,63   ;  28 spr_carreta #FFFFFF
        DB 24,38,63   ;  29 spr_carreta #639BFF
        DB 22, 7,30   ;  30 spr_guariamorada #5A1E78
        DB 45,25,55   ;  31 spr_guariamorada #B464DC
        DB 60,60,63   ;  32 spr_guariamorada #F0F0FF
        DB 63,55,10   ;  33 spr_guariamorada #FFDC28
        DB  5, 5, 6   ;  34 spr_marimba #141419
        DB 55,12,12   ;  35 spr_marimba #DC3232
        DB 40,20,10   ;  36 spr_marimba #A05028
        DB  3, 5, 8   ;  37 spr_mariposamorpho #0F1423
        DB 30,50,63   ;  38 spr_mariposamorpho #78C8FF
        DB  7,35,63   ;  39 spr_mariposamorpho #1E8CFF
        DB 12,37,50   ;  40 spr_tortuga #3296C8
        DB  5,20,30   ;  41 spr_tortuga #145078
        DB 10, 5, 2   ;  42 spr_tortuga #28140A
        DB 57,50,32   ;  43 spr_tortuga #E6C882
        DB  0, 0, 0   ;  44 spr_tortuga #000000
        DB 63,52, 7   ;  45 spr_volcan #FFD21E
        DB 57,10, 5   ;  46 spr_volcan #E62814
        DB 15,16,18   ;  47 spr_volcan #3C414B
        DB 27,28,31   ;  48 spr_volcan #6E737D
        DB 63,50,10   ;  49 spr_yiguirro #FCC828
        DB 40,25,10   ;  50 spr_yiguirro #A06428
        DB 63,63,63   ;  51 spr_yiguirro #FCFCFC
        DB 20,10, 5   ;  52 spr_yiguirro #502814
