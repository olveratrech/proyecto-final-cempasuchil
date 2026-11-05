"""Reconecta y empaca imágenes de un .blend usando solamente assets del repositorio.

Uso:
  blender --background archivo.blend --python empaquetar_recursos.py -- /ruta/al/repositorio
"""

import bpy
import json
import os
import sys
from pathlib import Path

ALIAS = {"tallo_color.png": "flor_verdes_color.png"}


def argumentos():
    args = sys.argv
    if "--" not in args:
        raise RuntimeError("Falta la ruta raíz del repositorio después de --")
    extra = args[args.index("--") + 1 :]
    if len(extra) != 1:
        raise RuntimeError("Se esperaba exactamente una ruta raíz")
    return Path(extra[0]).resolve()


def indice_imagenes(raiz):
    extensiones = {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".webp", ".exr", ".hdr"}
    resultado = {}
    for base in (raiz / "assets" / "textures", raiz / "assets" / "models"):
        if not base.exists():
            continue
        for ruta in base.rglob("*"):
            if ruta.is_file() and ruta.suffix.lower() in extensiones:
                resultado.setdefault(ruta.name.lower(), []).append(ruta)
    return resultado


def existe_imagen(imagen):
    if imagen.source in {"GENERATED", "VIEWER"}:
        return True
    if imagen.packed_file:
        return True
    if not imagen.filepath:
        return False
    return Path(bpy.path.abspath(imagen.filepath)).is_file()


def elegir(candidatas, imagen):
    if len(candidatas) == 1:
        return candidatas[0]
    anterior = Path(imagen.filepath).as_posix().lower()
    partes = [p for p in Path(anterior).parts if len(p) > 2]
    puntuadas = []
    for candidata in candidatas:
        texto = candidata.as_posix().lower()
        puntuadas.append((sum(1 for p in partes if p in texto), len(texto), candidata))
    return max(puntuadas, key=lambda x: (x[0], -x[1]))[2]


def main():
    raiz = argumentos()
    archivo = Path(bpy.data.filepath)
    indice = indice_imagenes(raiz)
    reporte = {"archivo": str(archivo), "reconectadas": [], "ya_disponibles": [], "faltantes": []}

    for imagen in bpy.data.images:
        if imagen.name == "Render Result" or imagen.source == "VIEWER":
            continue
        if existe_imagen(imagen):
            reporte["ya_disponibles"].append(imagen.name)
            continue
        nombre = Path(imagen.filepath or imagen.name).name.lower()
        nombre = ALIAS.get(nombre, nombre)
        candidatas = indice.get(nombre, [])
        if not candidatas:
            reporte["faltantes"].append({"imagen": imagen.name, "ruta": imagen.filepath})
            continue
        nueva = elegir(candidatas, imagen)
        imagen.filepath = str(nueva)
        try:
            imagen.reload()
            reporte["reconectadas"].append({"imagen": imagen.name, "ruta": str(nueva)})
        except Exception as exc:
            reporte["faltantes"].append({"imagen": imagen.name, "ruta": str(nueva), "error": str(exc)})

    bpy.ops.file.pack_all()
    bpy.ops.wm.save_as_mainfile(filepath=str(archivo), compress=True)
    print("AUDITORIA_RECURSOS=" + json.dumps(reporte, ensure_ascii=False))
    if reporte["faltantes"]:
        raise RuntimeError(f"Quedaron {len(reporte['faltantes'])} imágenes sin resolver")


if __name__ == "__main__":
    main()
