# Rig de los personajes de Kenney Graveyard Kit para exportar_personajes.py:
# importa el personaje jerárquico, le crea un armature de siete huesos, une sus
# piezas en una sola malla pesada a los huesos y ofrece poses por hueso.
import math
import os

import bpy
from mathutils import Matrix, Quaternion, Vector

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
KENNEY = f"{REPO}/assets/models/libreria/kenney_graveyard"

PIEZA_A_HUESO = {
    "leg-right": "pierna.R", "leg-left": "pierna.L", "torso": "torso",
    "head": "cabeza", "arm-right": "brazo.R", "arm-left": "brazo.L",
}


def importar(path):
    """Importa un glTF/GLB y devuelve los objetos nuevos (sin animaciones importadas)."""
    antes = set(bpy.data.objects)
    acts = set(bpy.data.actions)
    bpy.ops.import_scene.gltf(filepath=path)
    nuevos = [o for o in bpy.data.objects if o not in antes]
    for a in list(bpy.data.actions):
        if a not in acts:
            bpy.data.actions.remove(a)
    for o in nuevos:
        o.animation_data_clear()
    return nuevos


def unir(objs, nombre):
    objs = [o for o in objs if o.type == "MESH"]
    with bpy.context.temp_override(active_object=objs[0], object=objs[0],
                                   selected_objects=objs, selected_editable_objects=objs):
        bpy.ops.object.join()
    objs[0].name = nombre
    return objs[0]


def hornear_transformaciones(objs):
    """Deja cada malla con su transformación aplicada y sin padre (raíz en el origen)."""
    bpy.context.view_layer.update()
    mats = {o.name: o.matrix_world.copy() for o in objs}
    for o in objs:
        if o.type != "MESH":
            continue
        M = mats[o.name]
        o.parent = None
        o.data = o.data.copy()
        o.data.transform(M)
        o.matrix_world = Matrix.Identity(4)


def crear_personaje(glb, nombre, escala):
    nuevos = importar(glb)
    mallas = {o.name.split(".")[0]: o for o in nuevos if o.type == "MESH"}
    piv = {k: o.matrix_world.translation.copy() for k, o in mallas.items()}
    hornear_transformaciones(list(mallas.values()))
    for o in [o for o in nuevos if o.type != "MESH"]:
        bpy.data.objects.remove(o)

    datos = bpy.data.armatures.new(f"{nombre}_esqueleto")
    rig = bpy.data.objects.new(f"{nombre}_rig", datos)
    bpy.context.scene.collection.objects.link(rig)
    datos.display_type = "STICK"
    rig.show_in_front = True
    bpy.context.view_layer.objects.active = rig
    with bpy.context.temp_override(active_object=rig, object=rig):
        bpy.ops.object.mode_set(mode="EDIT")
    eb = datos.edit_bones

    def hueso(n, h, t, padre=None):
        b = eb.new(n)
        b.head, b.tail, b.roll = h, t, 0.0
        if padre:
            b.parent = eb[padre]
        return b

    cad = piv["torso"]
    hueso("cadera", cad, cad + Vector((0, 0, 0.08)))
    hueso("torso", cad, piv["head"], "cadera")
    hueso("cabeza", piv["head"], piv["head"] + Vector((0, 0, 0.3)), "torso")
    hueso("brazo.R", piv["arm-right"], piv["arm-right"] + Vector((-0.25, 0, 0)), "torso")
    hueso("brazo.L", piv["arm-left"], piv["arm-left"] + Vector((0.25, 0, 0)), "torso")
    hueso("pierna.R", piv["leg-right"], piv["leg-right"] + Vector((0, 0, -0.2)), "cadera")
    hueso("pierna.L", piv["leg-left"], piv["leg-left"] + Vector((0, 0, -0.2)), "cadera")
    with bpy.context.temp_override(active_object=rig, object=rig):
        bpy.ops.object.mode_set(mode="OBJECT")

    # pesos rígidos: cada pieza queda 100 % ligada a su hueso
    for k, o in mallas.items():
        vg = o.vertex_groups.new(name=PIEZA_A_HUESO[k])
        vg.add(range(len(o.data.vertices)), 1.0, "REPLACE")
    cuerpo = unir(list(mallas.values()), nombre)
    cuerpo.parent = rig
    mod = cuerpo.modifiers.new("Armature", "ARMATURE")
    mod.object = rig
    rig.scale = (escala,) * 3
    for pb in rig.pose.bones:
        pb.rotation_mode = "QUATERNION"
    rig.rotation_mode = "XYZ"
    return rig, cuerpo


# ---------- poses (rotaciones en ejes del esqueleto, relativas al padre) ----------
def Q(eje, grados):
    return Quaternion(Vector(eje), math.radians(grados))

X, Y, Z = (1, 0, 0), (0, 1, 0), (0, 0, 1)
I = Quaternion()

# El personaje mira hacia -Y del esqueleto; su derecha es -X.
BRAZOS_ABAJO = {"brazo.R": Q(Y, -80), "brazo.L": Q(Y, 80)}
BRAZOS_CARGA = {"brazo.R": Q(X, 14) @ Q(Z, 98), "brazo.L": Q(X, 14) @ Q(Z, -98)}


def mezclar(a, b, w):
    out = {}
    for k in set(a) | set(b):
        qa, qb = a.get(k, I), b.get(k, I)
        out[k] = qa.slerp(qb, w)
    return out


def suave(t):
    t = max(0.0, min(1.0, t))
    return t * t * (3 - 2 * t)


class Animador:
    """Inserta llaves por hueso cuidando la continuidad de los cuaterniones."""

    def __init__(self, rig):
        self.rig = rig
        self.prev = {}

    def pose(self, f, rots, dz=0.0):
        for pb in self.rig.pose.bones:
            R = pb.bone.matrix_local.to_quaternion()
            q = R.inverted() @ rots.get(pb.name, I) @ R
            p = self.prev.get(pb.name)
            if p is not None and p.dot(q) < 0:
                q.negate()
            self.prev[pb.name] = q.copy()
            pb.rotation_quaternion = q
            pb.keyframe_insert("rotation_quaternion", frame=f, group=pb.name)
        cad = self.rig.pose.bones["cadera"]
        cad.location = (0, dz, 0)
        cad.keyframe_insert("location", frame=f, group="cadera")

    def raiz(self, f, loc, yaw):
        self.rig.location = loc
        self.rig.rotation_euler = (0, 0, yaw)
        self.rig.keyframe_insert("location", frame=f)
        self.rig.keyframe_insert("rotation_euler", frame=f)


def caminata(fase, brazos=True, amp=34):
    """Pose de caminata en la fase dada (radianes). Devuelve (rotaciones, desplazamiento de cadera)."""
    s = math.sin(fase)
    r = {
        "pierna.R": Q(X, -amp * s),
        "pierna.L": Q(X, amp * s),
        "torso": Q(Z, 5 * s) @ Q(X, 4),
        "cabeza": Q(Z, -3 * s),
    }
    if brazos:
        r["brazo.R"] = Q(X, 26 * s) @ Q(Y, -80)
        r["brazo.L"] = Q(X, -26 * s) @ Q(Y, 80)
    dz = 0.018 * (1 - s * s) - 0.009
    return r, dz
