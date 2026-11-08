# Genera los dos personajes con esqueleto y sus clips de animación, y los exporta
# como glTF binario con skin para el motor OpenGL:
#
#   assets/models/animados/visitante_panteonero.glb   (animación 7)
#   assets/models/animados/visitante_calaca.glb       (animaciones 8 y 9)
#   modelos-blender/animaciones/personajes.blend      (fuente editable)
#
# Cada personaje parte de una pieza jerárquica de Kenney Graveyard Kit (CC0).
# rig_kenney.crear_personaje() le agrega un armature de siete huesos (cadera,
# torso, cabeza, brazo.R/L, pierna.R/L) y liga cada pieza a su hueso con peso 1.
#
# Los clips solo contienen rotaciones de hueso y el desplazamiento vertical de
# la cadera. La trayectoria sobre la escena (posición y orientación de la raíz)
# la calcula el motor, de modo que el mismo ciclo de caminata sirve para
# cualquier ruta. El clip "bailar" es la excepción: incluye su propio
# desplazamiento lateral y el giro completo en el hueso de la cadera.
#
# Cada clip se guarda como acción en su propia pista NLA y el exportador glTF
# escribe una animación por pista.
#
# Uso:  python3 exportar_personajes.py   (con el módulo bpy)  o
#       blender -b -P exportar_personajes.py
import math
import os
import sys

import bpy
from mathutils import Vector

AQUI = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, AQUI)
import rig_kenney as rk  # noqa: E402
from rig_kenney import Q, X, Y, Z, I  # noqa: E402

REPO = rk.REPO
SALIDA = os.path.join(REPO, "assets", "models", "animados")
FUENTE = os.path.join(AQUI, "personajes.blend")
ESC = 1.9            # escala del personaje (la misma de los clips del video)
FPS = 24
ALTURA_DEPOSITO = 0.42  # altura del primer escalón del altar sobre el piso del portal (m)

ABAJO = dict(rk.BRAZOS_ABAJO)
CARGA = dict(rk.BRAZOS_CARGA)
MANOS_JUNTAS = {"brazo.R": Q(X, 28) @ Q(Z, 60) @ Q(Y, -55), "brazo.L": Q(X, 28) @ Q(Z, -60) @ Q(Y, 55)}


# ---------------------------------------------------------------------------
# Utilidades de clips
# ---------------------------------------------------------------------------
def nuevo_clip(rig, nombre, cuadros, pose, raiz=None):
    """Crea una acción con llaves en cada cuadro y la guarda en una pista NLA.

    pose(f) -> (rotaciones, dz). raiz(f) -> (desplazamiento_x, giro_y) opcional,
    se hornea en el hueso de la cadera (solo para el baile).
    """
    rig.animation_data_create()
    accion = bpy.data.actions.new(nombre)
    rig.animation_data.action = accion
    animador = rk.Animador(rig)
    for f in range(1, cuadros + 1):
        rots, dz = pose(f)
        if raiz:
            dx, giro = raiz(f)
            rots = dict(rots)
            rots["cadera"] = Q(Z, math.degrees(giro)) @ rots.get("cadera", I)
        animador.pose(f, rots, dz)
        if raiz:
            cad = rig.pose.bones["cadera"]
            cad.location = (dx / ESC, dz, 0.0)
            cad.keyframe_insert("location", frame=f, group="cadera")
    pista = rig.animation_data.nla_tracks.new()
    pista.name = nombre
    tira = pista.strips.new(nombre, 1, accion)
    tira.action_frame_end = cuadros
    rig.animation_data.action = None
    pista.mute = True
    return accion


def pose_neutra(rig):
    for pb in rig.pose.bones:
        pb.rotation_quaternion = (1, 0, 0, 0)
        pb.location = (0, 0, 0)


def exportar(rig, cuerpo, extra, archivo):
    bpy.ops.object.select_all(action="DESELECT")
    for o in (rig, cuerpo, *extra):
        o.select_set(True)
    bpy.context.view_layer.objects.active = rig
    for pista in rig.animation_data.nla_tracks:
        pista.mute = False
    bpy.ops.export_scene.gltf(
        filepath=archivo, export_format="GLB", use_selection=True,
        export_animations=True, export_animation_mode="NLA_TRACKS",
        export_force_sampling=True, export_frame_step=1, export_skins=True,
        export_def_bones=False, export_morph=False, export_yup=True,
        export_apply=False, export_materials="EXPORT", export_cameras=False,
        export_lights=False)
    print("EXPORTADO", archivo)


# ---------------------------------------------------------------------------
# Escena
# ---------------------------------------------------------------------------
bpy.ops.wm.read_factory_settings(use_empty=True)
escena = bpy.context.scene
escena.render.fps = FPS

# ---------------------------------------------------------------------------
# Visitante 1: panteonero que lleva pan de muerto al altar
# ---------------------------------------------------------------------------
rig1, cuerpo1 = rk.crear_personaje(f"{rk.KENNEY}/character-keeper.glb", "visitante_panteonero", ESC)
rig1.name = "visitante_panteonero"


def pose_deposito(a, inclinacion=24):
    return {"torso": Q(X, inclinacion), "cabeza": Q(X, -inclinacion + 6),
            "brazo.R": Q(X, a) @ Q(Z, 98), "brazo.L": Q(X, a) @ Q(Z, -98),
            "pierna.R": Q(X, -6), "pierna.L": Q(X, -6)}


def aplicar(rig, rots, dz=0.0):
    for pb in rig.pose.bones:
        R = pb.bone.matrix_local.to_quaternion()
        pb.rotation_quaternion = R.inverted() @ rots.get(pb.name, I) @ R
    rig.pose.bones["cadera"].location = (0, dz, 0)
    bpy.context.view_layer.update()


def punto_antebrazos(rig):
    """Punto medio sobre los antebrazos donde descansa la ofrenda (mundo)."""
    pts = []
    for n, sx in (("brazo.R", -1), ("brazo.L", 1)):
        pb = rig.pose.bones[n]
        p_rest = pb.bone.head_local + Vector((sx * 0.2, -0.01, 0.084))
        pts.append(rig.matrix_world @ (pb.matrix @ pb.bone.matrix_local.inverted() @ p_rest))
    return (pts[0] + pts[1]) / 2


# Ancla de la ofrenda: vacío hijo del antebrazo derecho; el motor la sigue
# mientras el personaje carga el pan y la usa para soltarlo en el altar.
aplicar(rig1, CARGA)
ancla = bpy.data.objects.new("ancla_ofrenda", None)
escena.collection.objects.link(ancla)
ancla.empty_display_size = 0.05
ancla.parent = rig1
ancla.parent_type = "BONE"
ancla.parent_bone = "brazo.R"
ancla.matrix_world.translation = punto_antebrazos(rig1)

# Inclinación de brazos que deja la ofrenda a la altura del primer escalón.
bajo, alto = -40.0, 70.0
for _ in range(30):
    medio = (bajo + alto) / 2
    aplicar(rig1, pose_deposito(medio), -0.02)
    if punto_antebrazos(rig1).z > ALTURA_DEPOSITO:
        bajo = medio
    else:
        alto = medio
A_DEP = (bajo + alto) / 2
print("Brazos al depositar:", round(A_DEP, 2), "grados")
pose_neutra(rig1)

nuevo_clip(rig1, "reposo", 48, lambda f: (
    dict(ABAJO, cabeza=Q(Z, 6 * math.sin(2 * math.pi * f / 48))), 0.004 * math.sin(2 * math.pi * f / 48)))


def caminar_cargando(f):
    r, dz = rk.caminata(2 * math.pi * (f - 1) / 24, brazos=False)
    r.update(CARGA)
    return r, dz


nuevo_clip(rig1, "caminar_cargando", 25, caminar_cargando)
nuevo_clip(rig1, "depositar", 32, lambda f: (
    rk.mezclar(CARGA, pose_deposito(A_DEP), rk.suave((f - 1) / 27)), -0.02 * rk.suave((f - 1) / 27)))
nuevo_clip(rig1, "incorporarse", 26, lambda f: (
    rk.mezclar(pose_deposito(A_DEP), ABAJO, rk.suave((f - 1) / 25)), -0.02 * (1 - rk.suave((f - 1) / 25))))


def reverencia(f):
    w = math.sin(math.pi * (f - 1) / 29)
    return dict(ABAJO, torso=Q(X, 16 * w), cabeza=Q(X, 18 * w)), -0.01 * w


nuevo_clip(rig1, "reverencia", 30, reverencia)


def caminar(f):
    return rk.caminata(2 * math.pi * (f - 1) / 24)


nuevo_clip(rig1, "caminar", 25, caminar)

# ---------------------------------------------------------------------------
# Visitante 2 y esqueleto bailarín: calaca
# ---------------------------------------------------------------------------
rig2, cuerpo2 = rk.crear_personaje(f"{rk.KENNEY}/character-skeleton.glb", "visitante_calaca", ESC)
rig2.name = "visitante_calaca"
rig2.location.x = 2.0  # solo para separarlos en el .blend fuente

nuevo_clip(rig2, "reposo", 48, lambda f: (
    dict(ABAJO, cabeza=Q(Z, 6 * math.sin(2 * math.pi * f / 48))), 0.004 * math.sin(2 * math.pi * f / 48)))
nuevo_clip(rig2, "caminar", 25, caminar)


def observar(f, cuadros=96):
    """Manos juntas frente a la tumba; la cabeza baja y mira a ambos lados."""
    t = (f - 1) / (cuadros - 1)
    w = min(rk.suave(t / 0.2), rk.suave((1 - t) / 0.15))
    gira = 32 * math.sin(2 * math.pi * (t - 0.35) / 0.55) if 0.35 < t < 0.9 else 0.0
    r = dict(rk.mezclar(ABAJO, MANOS_JUNTAS, w))
    r["cabeza"] = Q(Z, gira * w) @ Q(X, 22 * w * (1 - 0.6 * abs(gira) / 32))
    r["torso"] = Q(X, 9 * w)
    return r, -0.01 * w


nuevo_clip(rig2, "observar", 96, observar)


def saludar(f, cuadros=48):
    t = (f - 1) / (cuadros - 1)
    w = min(rk.suave(t / 0.2), rk.suave((1 - t) / 0.2))
    arriba = Q(X, -10) @ Q(Y, 62 + 22 * math.sin(2 * math.pi * 2.5 * t))
    return {"brazo.L": Q(Y, 80), "brazo.R": Q(Y, -80).slerp(arriba, w),
            "cabeza": Q(Y, -8 * w) @ Q(X, -6 * w), "torso": Q(Y, 4 * w)}, 0.0


nuevo_clip(rig2, "saludar", 48, saludar)


def baile_corto(f, cuadros=96):
    t = (f - 1) / (cuadros - 1)
    w = min(rk.suave(t / 0.12), rk.suave((1 - t) / 0.12))
    ph = 2 * math.pi * 2.0 * t
    sw = math.sin(ph)
    r = {"cadera": Q(Z, 14 * sw * w) @ Q(Y, 9 * sw * w), "torso": Q(Y, -15 * sw * w),
         "cabeza": Q(X, 9 * math.sin(2 * ph) * w) @ Q(Y, 10 * sw * w),
         "brazo.R": Q(Y, -80).slerp(Q(X, -15) @ Q(Y, 35 + 35 * sw), w),
         "brazo.L": Q(Y, 80).slerp(Q(X, -15) @ Q(Y, -35 + 35 * sw), w),
         "pierna.R": Q(Y, -6 * sw * w), "pierna.L": Q(Y, -6 * sw * w)}
    return r, 0.03 * abs(math.sin(ph)) * w


nuevo_clip(rig2, "bailar_corto", 96, baile_corto)

# Baile completo del esqueleto: cinco pasos a 120 bpm (un tiempo = 12 cuadros)
# mezclados con transiciones de medio segundo.
TIEMPO = 12


def onda(f, tiempos=2.0, fase=0.0):
    return math.sin(2 * math.pi * (f / (TIEMPO * tiempos)) + fase)


def paso_reposo(f):
    return dict(ABAJO, cabeza=Q(X, 5 * onda(f, 1))), 0.012 * abs(onda(f, 2)), 0.0, 0.0


def paso_balanceo(f):
    s = onda(f)
    return ({"cadera": Q(Y, 8 * s), "torso": Q(Y, -12 * s) @ Q(Z, 10 * s),
             "cabeza": Q(Y, 9 * s) @ Q(X, 7 * onda(f, 1)),
             "brazo.R": Q(Z, 35) @ Q(Y, -35 + 55 * s), "brazo.L": Q(Z, -35) @ Q(Y, 35 + 55 * s),
             "pierna.R": Q(X, -20 * max(0.0, s)) @ Q(Y, 6 * max(0.0, s)),
             "pierna.L": Q(X, -20 * max(0.0, -s)) @ Q(Y, -6 * max(0.0, -s))},
            0.035 * abs(onda(f, 1)), 0.20 * s, math.radians(10) * s)


def paso_patadas(f):
    k = onda(f)
    kp, kn = max(0.0, k), max(0.0, -k)
    return ({"cadera": Q(Z, -10 * k), "torso": Q(Z, 16 * k) @ Q(X, 6 * (kp + kn)),
             "cabeza": Q(Z, -10 * k) @ Q(X, 8 * onda(f, 1)),
             "pierna.R": Q(X, -48 * kp), "pierna.L": Q(X, -48 * kn),
             "brazo.R": Q(Y, -80).slerp(Q(X, -10) @ Q(Z, 85), kn),
             "brazo.L": Q(Y, 80).slerp(Q(X, -10) @ Q(Z, -85), kp)},
            0.045 * abs(k), 0.0, math.radians(14) * k)


def paso_brazos_arriba(f):
    s = onda(f)
    return ({"cadera": Q(Y, 11 * s), "torso": Q(Y, -9 * s), "cabeza": Q(Y, 10 * s) @ Q(X, -6),
             "brazo.R": Q(X, -12) @ Q(Y, 62 + 22 * s), "brazo.L": Q(X, -12) @ Q(Y, -62 + 22 * s),
             "pierna.R": Q(Y, 8 * max(0.0, s)), "pierna.L": Q(Y, -8 * max(0.0, -s))},
            0.03 * abs(onda(f, 1)), 0.0, 0.0)


def paso_disco(f):
    d = rk.suave(0.5 + 0.9 * onda(f))
    arriba = {"brazo.R": Q(Z, 20) @ Q(Y, 62), "brazo.L": Q(Z, 35) @ Q(Y, 60), "torso": Q(Y, -10),
              "cadera": Q(Y, 6), "cabeza": Q(Z, -18) @ Q(Y, 8), "pierna.R": Q(Y, 10), "pierna.L": I}
    abajo = {"brazo.R": Q(Z, -35) @ Q(Y, -60), "brazo.L": Q(Z, -20) @ Q(Y, -62), "torso": Q(Y, 10),
             "cadera": Q(Y, -6), "cabeza": Q(Z, 18) @ Q(Y, -8), "pierna.R": I, "pierna.L": Q(Y, -10)}
    return (rk.mezclar(abajo, arriba, d), 0.03 * abs(onda(f, 1)),
            0.12 * (2 * d - 1), math.radians(-12) * (2 * d - 1))


def paso_final(f):
    return ({"brazo.R": Q(X, -8) @ Q(Y, 38), "brazo.L": Q(X, -8) @ Q(Y, -38),
             "cabeza": Q(X, -8), "torso": Q(X, -4)}, 0.0, 0.0, 0.0)


PASOS = [(paso_balanceo, 14), (paso_patadas, 100), (paso_brazos_arriba, 172), (paso_disco, 236), (paso_final, 274)]


def baile(f):
    r, dz, x, giro = paso_reposo(f)
    for paso, inicio in PASOS:
        w = rk.suave((f - inicio) / 12)
        if w <= 0:
            continue
        r2, dz2, x2, giro2 = paso(f)
        r = rk.mezclar(r, r2, w)
        dz += (dz2 - dz) * w
        x += (x2 - x) * w
        giro += (giro2 - giro) * w
    return r, dz, x, giro + 2 * math.pi * rk.suave((f - 184) / 44)


nuevo_clip(rig2, "bailar", 288, lambda f: baile(f)[:2], raiz=lambda f: baile(f)[2:])

# ---------------------------------------------------------------------------
# Exportación
# ---------------------------------------------------------------------------
pose_neutra(rig1)
pose_neutra(rig2)
os.makedirs(SALIDA, exist_ok=True)
rig2.location.x = 0.0
exportar(rig1, cuerpo1, [ancla], os.path.join(SALIDA, "visitante_panteonero.glb"))
exportar(rig2, cuerpo2, [], os.path.join(SALIDA, "visitante_calaca.glb"))
rig2.location.x = 2.0
for o in bpy.data.objects:
    if o.type == "ARMATURE":
        for pista in o.animation_data.nla_tracks:
            pista.mute = True
bpy.ops.file.pack_all()
bpy.ops.wm.save_as_mainfile(filepath=FUENTE, compress=True)
print("GUARDADO", FUENTE)
