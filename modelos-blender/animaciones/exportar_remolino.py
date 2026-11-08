# Exporta la animación del remolino de pétalos (remolino_petalos.blend) a
# assets/animaciones/remolino_petalos.pist.
#
# Uso:  blender -b remolino_petalos.blend -P exportar_remolino.py
#       (o python3 exportar_remolino.py con el módulo bpy instalado)
#
# Cada cuadro guarda, por pétalo: posición (3), rotación como cuaternión (4) y
# escala uniforme (1), en el marco local del remolino: el origen es el eje del
# remolino al nivel del piso, +Z apunta del altar hacia el camino.
import os
import sys

import bpy

AQUI = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, AQUI)
import pistas  # noqa: E402

REPO = os.path.normpath(os.path.join(AQUI, "..", ".."))
FUENTE = os.path.join(AQUI, "remolino_petalos.blend")
SALIDA = os.path.join(REPO, "assets", "animaciones", "remolino_petalos.pist")
EJE = pistas.Vector((0.0, -2.05, 0.0))  # eje del remolino en la escena de Blender

if os.path.normpath(bpy.data.filepath or "") != os.path.normpath(FUENTE):
    bpy.ops.wm.open_mainfile(filepath=FUENTE)

escena = bpy.context.scene
petalos = sorted((o for o in escena.objects if o.name.startswith("petalo_") and o.type == "MESH"),
                 key=lambda o: o.name)
cuadros = []
for f in range(escena.frame_start, escena.frame_end + 1):
    escena.frame_set(f)
    cuadro = []
    for p in petalos:
        loc, rot, esc = p.matrix_world.decompose()
        cuadro.append((*pistas.a_gl(loc - EJE), *pistas.quat_a_gl(rot), esc.x))
    cuadros.append(cuadro)

os.makedirs(os.path.dirname(SALIDA), exist_ok=True)
pistas.escribir(SALIDA, escena.render.fps, cuadros)
