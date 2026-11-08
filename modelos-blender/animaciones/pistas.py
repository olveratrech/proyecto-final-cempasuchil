# Escritura de pistas horneadas (.pist) que lee src/anim/Pista.cpp.
#
# Formato binario, little endian:
#   char[4]  "PIST"
#   uint32   version      (1)
#   uint32   fps
#   uint32   cuadros
#   uint32   elementos     (objetos por cuadro, p. ej. 720 pétalos)
#   uint32   componentes   (flotantes por elemento)
#   float32  datos[cuadros][elementos][componentes]
#
# Las coordenadas se convierten de Blender (Z arriba, -Y al frente) a OpenGL/glTF
# (Y arriba, +Z al frente) con la misma regla que usa el exportador glTF:
#   (x, y, z) -> (x, z, -y)
import struct

from mathutils import Quaternion, Vector


def a_gl(v):
    """Punto o vector de Blender a coordenadas de OpenGL."""
    return (v[0], v[2], -v[1])


def quat_a_gl(q: Quaternion):
    """Cuaternión de Blender a OpenGL como (x, y, z, w), orden de glm::quat."""
    return (q.x, q.z, -q.y, q.w)


def escribir(ruta, fps, cuadros):
    """cuadros: lista de cuadros; cada cuadro es una lista de elementos y cada
    elemento una secuencia de flotantes de la misma longitud."""
    elementos = len(cuadros[0])
    componentes = len(cuadros[0][0])
    with open(ruta, "wb") as f:
        f.write(b"PIST")
        f.write(struct.pack("<5I", 1, fps, len(cuadros), elementos, componentes))
        for cuadro in cuadros:
            assert len(cuadro) == elementos
            for elemento in cuadro:
                assert len(elemento) == componentes
                f.write(struct.pack(f"<{componentes}f", *elemento))
    print(f"PISTA {ruta}: {len(cuadros)} cuadros x {elementos} elementos x {componentes} componentes")


__all__ = ["a_gl", "quat_a_gl", "escribir", "Vector"]
