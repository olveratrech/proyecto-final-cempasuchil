# Exporta la animación "jarra sirviendo agua" (jarra_sirviendo_agua.blend) a
# tres pistas en assets/animaciones/:
#
#   jarra_sirviendo.pist  (240 cuadros x 1 elemento x 13 componentes)
#       0-2   posición de la jarra
#       3-6   rotación de la jarra (x, y, z, w)
#       7     altura de la superficie del agua dentro de la jarra (caja de nivel)
#       8     factor de llenado del vaso (equivale a la shape key "vaso_vacio")
#       9     agua del vaso visible (0/1)
#       10-11 bevel_factor_start / bevel_factor_end del chorro
#       12    chorro visible (0/1)
#   jarra_chorro.pist     (1 cuadro x N puntos x 4: posición y radio del chorro)
#   jarra_interior.pist   (1 cuadro x M anillos x 2: altura y radio del volumen
#                          de agua que se recorta dentro de la jarra)
#
# Todas las posiciones se expresan respecto al centro de la base del vaso.
import os
import sys

import bpy

AQUI = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, AQUI)
import pistas  # noqa: E402

REPO = os.path.normpath(os.path.join(AQUI, "..", ".."))
FUENTE = os.path.join(AQUI, "jarra_sirviendo_agua.blend")
CARPETA = os.path.join(REPO, "assets", "animaciones")

if os.path.normpath(bpy.data.filepath or "") != os.path.normpath(FUENTE):
    bpy.ops.wm.open_mainfile(filepath=FUENTE)

escena = bpy.context.scene
obj = bpy.data.objects
jarra, vaso, nivel = obj["jarra_agua"], obj["vaso"], obj["nivel_agua_jarra"]
agua_vaso, chorro, interior = obj["agua_vaso"], obj["chorro"], obj["agua_jarra"]
llave = agua_vaso.data.shape_keys.key_blocks["vaso_vacio"]
curva = chorro.data

escena.frame_set(escena.frame_start)
base = vaso.matrix_world.translation.copy()

cuadros = []
for f in range(escena.frame_start, escena.frame_end + 1):
    escena.frame_set(f)
    loc, rot, _ = jarra.matrix_world.decompose()
    # La caja de nivel se desplaza 0.5 m hacia abajo en su malla: su cara
    # superior coincide con la ubicación del objeto.
    superficie = nivel.matrix_world.translation.z - base.z
    llenado = 1.0 - 0.97 * llave.value
    cuadros.append([(*pistas.a_gl(loc - base), *pistas.quat_a_gl(rot), superficie, llenado,
                     0.0 if agua_vaso.hide_render else 1.0,
                     curva.bevel_factor_start, curva.bevel_factor_end,
                     0.0 if chorro.hide_render else 1.0)])

os.makedirs(CARPETA, exist_ok=True)
pistas.escribir(os.path.join(CARPETA, "jarra_sirviendo.pist"), escena.render.fps, cuadros)

# Trayectoria del chorro (polilínea de la curva) y su radio por punto.
puntos = []
for p in curva.splines[0].points:
    mundo = chorro.matrix_world @ pistas.Vector(p.co[:3])
    puntos.append((*pistas.a_gl(mundo - base), p.radius * curva.bevel_depth))
pistas.escribir(os.path.join(CARPETA, "jarra_chorro.pist"), escena.render.fps, [puntos])

# Perfil del volumen interior: anillos (altura, radio) en coordenadas de la jarra.
anillos = {}
for v in interior.data.vertices:
    radio = (v.co.x ** 2 + v.co.y ** 2) ** 0.5
    if radio > 1e-6:
        anillos[round(v.co.z, 5)] = radio
perfil = [(z, anillos[z]) for z in sorted(anillos)]
pistas.escribir(os.path.join(CARPETA, "jarra_interior.pist"), escena.render.fps, [perfil])
