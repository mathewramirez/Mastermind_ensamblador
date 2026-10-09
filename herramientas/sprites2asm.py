"""
sprites2asm.py - Herramienta de DISENO (no forma parte del juego entregado).

Lee los sprites de /sprites (archivos .ase de Aseprite o .png) y genera
las tablas de bytes que usa el juego en ensamblador.

Uso (desde la raiz del proyecto, en Windows):
    pip install pillow
    python herramientas\\sprites2asm.py

Como arma la paleta VGA:
  1. Primero los colores de interfaz de paleta.json (fondo, paneles, texto...)
  2. Despues, automaticamente, cada color distinto que aparezca en los sprites
  Todo empieza en el indice 'inicio' de paleta.json (0 = transparente;
  1..15 se dejan con los colores por defecto del VGA para depurar).

Sprites:
  * Pixel con alfa < 128 -> byte 0 (transparente, RG-03)
  * .ase con varios frames  -> varios cuadros de animacion (NOMBRE_0, NOMBRE_1...)
  * .png mas ancho que alto -> tira de cuadros cuadrados (lado = alto)
  * El nombre en ensamblador sale del archivo: "guariaMorada-1.ase" -> GUARIAMORADA

Genera:
  src/PALETA.INC        tabla R,G,B (escala VGA 0..63) para los puertos 3C8h/3C9h
  src/SPRITES.INC       tablas DB + constantes _ANCHO, _ALTO, _CUADROS
  doc/hoja_sprites.png  todos los sprites ampliados (entregable E5)
  doc/tabla_paleta.md   tabla de la paleta usada (entregable E4/E5)
"""
import json
import re
import struct
import sys
import zlib
from pathlib import Path

from PIL import Image

RAIZ = Path(__file__).resolve().parent.parent
DIR_SPRITES = RAIZ / "sprites"
ARCH_PALETA = Path(__file__).resolve().parent / "paleta.json"


# ---------------------------------------------------------------------------
# Lectura de archivos .ase (formato documentado por Aseprite)
# ---------------------------------------------------------------------------
def leer_ase(ruta):
    d = ruta.read_bytes()
    _, magic, nframes, W, H, prof = struct.unpack_from("<IHHHHH", d, 0)
    if magic != 0xA5E0:
        sys.exit(f"{ruta.name}: no es un archivo de Aseprite valido")
    idx_transp = d[28]
    pos, paleta, cuadros = 128, {}, []
    for _ in range(nframes):
        tam_frame, _, viejos = struct.unpack_from("<IHH", d, pos)
        nuevos = struct.unpack_from("<I", d, pos + 12)[0]
        cp = pos + 16
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        for _ in range(nuevos or viejos):
            tam, tipo = struct.unpack_from("<IH", d, cp)
            cuerpo = cp + 6
            if tipo == 0x2019:  # paleta
                _, prim, ult = struct.unpack_from("<III", d, cuerpo)
                q = cuerpo + 20
                for i in range(prim, ult + 1):
                    flags = struct.unpack_from("<H", d, q)[0]
                    paleta[i] = tuple(d[q + 2:q + 6])
                    q += 6
                    if flags & 1:
                        q += 2 + struct.unpack_from("<H", d, q)[0]
            elif tipo == 0x2005:  # cel (pixeles de una capa)
                _, x, y, _, tipo_cel = struct.unpack_from("<HhhBH", d, cuerpo)
                q = cuerpo + 16
                if tipo_cel in (0, 2):
                    w, h = struct.unpack_from("<HH", d, q)
                    crudo = d[q + 4:cp + tam]
                    if tipo_cel == 2:
                        crudo = zlib.decompress(crudo)
                    bpp = prof // 8
                    for j in range(h):
                        for i in range(w):
                            o = (j * w + i) * bpp
                            if prof == 32:
                                px = tuple(crudo[o:o + 4])
                            elif prof == 8:
                                k = crudo[o]
                                px = (0, 0, 0, 0) if k == idx_transp else paleta.get(k, (0, 0, 0, 0))
                            else:
                                px = (crudo[o], crudo[o], crudo[o], crudo[o + 1])
                            if px[3] > 0 and 0 <= x + i < W and 0 <= y + j < H:
                                img.putpixel((x + i, y + j), px)
            cp += tam
        cuadros.append(img)
        pos += tam_frame
    return cuadros


def leer_png(ruta):
    img = Image.open(ruta).convert("RGBA")
    lado = img.height
    if img.width % lado:
        sys.exit(f"{ruta.name}: {img.width}x{img.height} no se divide en cuadros de {lado}x{lado}")
    return [img.crop((k * lado, 0, (k + 1) * lado, lado)) for k in range(img.width // lado)]


def nombre_asm(stem):
    stem = re.sub(r"-\d+$", "", stem)  # "cafe-1" -> "cafe"
    return re.sub(r"[^A-Za-z0-9]", "_", stem).upper()


def hexa(rgb):
    return "#%02X%02X%02X" % rgb


# ---------------------------------------------------------------------------
def main():
    cfg = json.loads(ARCH_PALETA.read_text(encoding="utf-8"))
    inicio = int(cfg["inicio"])

    # 1) leer sprites
    sprites = []
    for ruta in sorted(DIR_SPRITES.iterdir()):
        ext = ruta.suffix.lower()
        if ext == ".ase" or ext == ".aseprite":
            cuadros = leer_ase(ruta)
        elif ext == ".png":
            cuadros = leer_png(ruta)
        else:
            continue
        sprites.append((ruta.name, nombre_asm(ruta.stem), cuadros))

    # 2) armar la paleta: interfaz primero, luego colores de sprites
    colores = []  # (nombre, (r,g,b))
    indice = {}
    for c in cfg["colores"]:
        h = c["hex"].lstrip("#")
        rgb = tuple(int(h[i:i + 2], 16) for i in (0, 2, 4))
        indice.setdefault(rgb, inicio + len(colores))
        colores.append((c["nombre"], rgb))
    for archivo, nombre, cuadros in sprites:
        for img in cuadros:
            for r, g, b, a in img.getdata():
                if a >= 128 and (r, g, b) not in indice:
                    indice[(r, g, b)] = inicio + len(colores)
                    colores.append((f"spr_{nombre.lower()}", (r, g, b)))
    if inicio < 1 or inicio + len(colores) > 256:
        sys.exit(f"La paleta no cabe: {len(colores)} colores desde el indice {inicio}")

    # 3) PALETA.INC
    lin = ["; GENERADO por herramientas/sprites2asm.py - NO editar a mano",
           "; Cada entrada: R,G,B en escala VGA 0..63 (el DAC del VGA usa 6 bits: 8 bits >> 2)",
           f"PALETA_INICIO   EQU {inicio}",
           f"PALETA_CANT     EQU {len(colores)}"]
    for i, (nom, rgb) in enumerate(colores):
        if i < len(cfg["colores"]):
            lin.append(f"C_{nom.upper():<14} EQU {inicio + i}")
    lin.append("tablaPaleta LABEL BYTE")
    for i, (nom, (r, g, b)) in enumerate(colores):
        lin.append(f"        DB {r >> 2:2d},{g >> 2:2d},{b >> 2:2d}   ; {inicio + i:3d} {nom} {hexa((r, g, b))}")
    (RAIZ / "src" / "PALETA.INC").write_text("\r\n".join(lin) + "\r\n", encoding="ascii")

    # 4) SPRITES.INC
    out = ["; GENERADO por herramientas/sprites2asm.py - NO editar a mano",
           "; 1 byte por pixel = indice de paleta, izq->der, arriba->abajo. 0 = transparente."]
    for archivo, nombre, cuadros in sprites:
        w, h = cuadros[0].size
        out += ["", f"; ---- {archivo}: {w}x{h}, {len(cuadros)} cuadro(s) ----",
                f"{nombre}_ANCHO EQU {w}", f"{nombre}_ALTO EQU {h}",
                f"{nombre}_CUADROS EQU {len(cuadros)}"]
        for k, img in enumerate(cuadros):
            out.append(f"{nombre}_{k} LABEL BYTE")
            for y in range(h):
                fila = []
                for x in range(w):
                    r, g, b, a = img.getpixel((x, y))
                    fila.append(indice[(r, g, b)] if a >= 128 else 0)
                out.append("        DB " + ",".join(f"{v:3d}" for v in fila))
    (RAIZ / "src" / "SPRITES.INC").write_text("\r\n".join(out) + "\r\n", encoding="ascii")

    # 5) doc/hoja_sprites.png y doc/tabla_paleta.md
    (RAIZ / "doc").mkdir(exist_ok=True)
    esc, sep = 8, 10
    max_cuadros = max((len(c) for _, _, c in sprites), default=1)
    lado = max((c[0].width for _, _, c in sprites), default=16)
    alto_fila = max((c[0].height for _, _, c in sprites), default=16)
    lienzo = Image.new("RGB", (sep + max_cuadros * (lado * esc + sep),
                               sep + len(sprites) * (alto_fila * esc + sep)), (24, 24, 32))
    for f, (_, _, cuadros) in enumerate(sprites):
        for k, img in enumerate(cuadros):
            grande = img.resize((img.width * esc, img.height * esc), Image.NEAREST)
            lienzo.paste(grande, (sep + k * (lado * esc + sep), sep + f * (alto_fila * esc + sep)), grande)
    lienzo.save(RAIZ / "doc" / "hoja_sprites.png")

    md = ["# Tabla de paleta (generada por sprites2asm.py)", "",
          "| Indice | Nombre | RGB 8 bits | RGB VGA (0..63) |", "|---|---|---|---|"]
    for i, (nom, (r, g, b)) in enumerate(colores):
        md.append(f"| {inicio + i} | {nom} | {hexa((r, g, b))} | {r >> 2}, {g >> 2}, {b >> 2} |")
    (RAIZ / "doc" / "tabla_paleta.md").write_text("\n".join(md) + "\n", encoding="utf-8")

    print(f"OK: {len(sprites)} sprite(s), {len(colores)} colores (indices {inicio}..{inicio + len(colores) - 1})")
    for archivo, nombre, cuadros in sprites:
        print(f"   {archivo:28s} -> {nombre}_0..{len(cuadros) - 1}  ({cuadros[0].width}x{cuadros[0].height})")


if __name__ == "__main__":
    main()
