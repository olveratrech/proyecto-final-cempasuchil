"""Ejecutar desde la consola o el editor de texto de Blender.

Procesa todos los .blend de modelos-blender desde una sola sesión gráfica.
"""

import bpy
import json
from pathlib import Path


RAIZ = Path(__file__).resolve().parents[1]
CARPETA = RAIZ / "modelos-blender"
EXTENSIONES = {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".webp", ".exr", ".hdr"}
ALIAS = {
    # Nombre histórico de la textura del tallo en la fuente Blender.
    "tallo_color.png": "flor_verdes_color.png",
}


def construir_indice():
    indice = {}
    for base in (RAIZ / "assets" / "textures", RAIZ / "assets" / "models"):
        for ruta in base.rglob("*"):
            if ruta.is_file() and ruta.suffix.lower() in EXTENSIONES:
                indice.setdefault(ruta.name.lower(), []).append(ruta)
    return indice


def disponible(imagen):
    if imagen.source in {"GENERATED", "VIEWER"} or imagen.packed_file:
        return True
    return bool(imagen.filepath and Path(bpy.path.abspath(imagen.filepath)).is_file())


def procesar(archivo, indice):
    bpy.ops.wm.open_mainfile(filepath=str(archivo), load_ui=False)
    faltantes = []
    reconectadas = []
    for imagen in bpy.data.images:
        if imagen.name == "Render Result" or imagen.source == "VIEWER" or disponible(imagen):
            continue
        nombre = Path(imagen.filepath or imagen.name).name.lower()
        nombre = ALIAS.get(nombre, nombre)
        candidatas = indice.get(nombre, [])
        if not candidatas:
            faltantes.append({"imagen": imagen.name, "ruta": imagen.filepath})
            continue
        imagen.filepath = str(candidatas[0])
        imagen.reload()
        reconectadas.append(imagen.name)
    if not faltantes:
        bpy.ops.file.pack_all()
        bpy.ops.wm.save_as_mainfile(filepath=str(archivo), compress=True)
    return {"archivo": str(archivo.relative_to(RAIZ)), "reconectadas": reconectadas, "faltantes": faltantes}


indice = construir_indice()
archivos = sorted(p for p in CARPETA.rglob("*.blend") if p.is_file())
reporte = []
for numero, archivo in enumerate(archivos, 1):
    print(f"EMPAQUETANDO {numero}/{len(archivos)}: {archivo.name}")
    try:
        reporte.append(procesar(archivo, indice))
    except Exception as exc:
        reporte.append({"archivo": str(archivo.relative_to(RAIZ)), "error": str(exc), "faltantes": []})

salida = CARPETA / "auditoria-recursos.json"
salida.write_text(json.dumps(reporte, ensure_ascii=False, indent=2), encoding="utf-8")
fallos = [r for r in reporte if r.get("error") or r.get("faltantes")]
print(f"EMPAQUETADO_TERMINADO archivos={len(reporte)} fallos={len(fallos)} reporte={salida}")
