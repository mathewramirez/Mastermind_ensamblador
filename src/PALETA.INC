; GENERADO por herramientas/sprites2asm.py - NO editar a mano
; Cada entrada: R,G,B en escala VGA 0..63 (el DAC del VGA usa 6 bits: 8 bits >> 2)
PALETA_INICIO   EQU 16
PALETA_CANT     EQU 56
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
        DB 55,45,30   ;  22 spr_cafe #DCB478
        DB 17, 7, 2   ;  23 spr_cafe #461E0A
        DB 35,17, 7   ;  24 spr_cafe #8C461E
        DB 10, 3, 1   ;  25 spr_cafe #280F05
        DB  5, 5, 5   ;  26 spr_carreta #141414
        DB 63,52, 0   ;  27 spr_carreta #FFD200
        DB 63,63,63   ;  28 spr_carreta #FFFFFF
        DB 24,38,63   ;  29 spr_carreta #639BFF
        DB 31,63,14   ;  30 spr_cursor #7CFC3A
        DB 15,40, 8   ;  31 spr_cursor #3CA020
        DB 63,52, 7   ;  32 spr_derrota #FFD21E
        DB 57,10, 5   ;  33 spr_derrota #E62814
        DB 15,16,18   ;  34 spr_derrota #3C414B
        DB 27,28,31   ;  35 spr_derrota #6E737D
        DB 50,37, 2   ;  36 spr_exacto #C8960A
        DB 62,60,13   ;  37 spr_exacto #FBF236
        DB 22, 7,30   ;  38 spr_guariamorada #5A1E78
        DB 45,25,55   ;  39 spr_guariamorada #B464DC
        DB 60,60,63   ;  40 spr_guariamorada #F0F0FF
        DB 63,55,10   ;  41 spr_guariamorada #FFDC28
        DB  5, 5, 6   ;  42 spr_marimba #141419
        DB 55,12,12   ;  43 spr_marimba #DC3232
        DB 40,20,10   ;  44 spr_marimba #A05028
        DB  3, 5, 8   ;  45 spr_mariposamorpho #0F1423
        DB 30,50,63   ;  46 spr_mariposamorpho #78C8FF
        DB  7,35,63   ;  47 spr_mariposamorpho #1E8CFF
        DB 17, 8, 3   ;  48 spr_mascota #46230F
        DB 40,27,18   ;  49 spr_mascota #A06E4B
        DB 55,50,40   ;  50 spr_mascota #DCC8A0
        DB 58,53,43   ;  51 spr_mascota #EBD7AF
        DB  8, 7, 6   ;  52 spr_mascota #231E19
        DB 22,21,20   ;  53 spr_mascota #595652
        DB  7, 7, 7   ;  54 spr_mascota #1E1E1E
        DB  8, 3, 0   ;  55 spr_parcial #210C01
        DB 12,37,50   ;  56 spr_tortuga #3296C8
        DB  5,20,30   ;  57 spr_tortuga #145078
        DB 10, 5, 2   ;  58 spr_tortuga #28140A
        DB 57,50,32   ;  59 spr_tortuga #E6C882
        DB  0, 0, 0   ;  60 spr_tortuga #000000
        DB 43,12,12   ;  61 spr_victoria #AC3232
        DB 59,48,38   ;  62 spr_victoria #EEC39A
        DB 46,42,32   ;  63 spr_victoria #B8A980
        DB 58,56,51   ;  64 spr_victoria #E8E0CC
        DB  2,14,35   ;  65 spr_victoria #0A3A8C
        DB 61,61,61   ;  66 spr_victoria #F5F5F5
        DB 50, 4,11   ;  67 spr_victoria #C8102E
        DB 53,50,42   ;  68 spr_victoria #D4C9A8
        DB 40,25,10   ;  69 spr_yiguirro #A06428
        DB 63,63,63   ;  70 spr_yiguirro #FCFCFC
        DB 20,10, 5   ;  71 spr_yiguirro #502814
