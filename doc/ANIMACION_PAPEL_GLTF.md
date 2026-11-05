# Animación de papel picado y faroles

La escena optimizada conserva los faroles y su iluminación en OpenGL. No se hornean luces ni se reemplazan los materiales de la escena.

## Archivos Blender exportados

Los seis papeles de `papel_picado_viento.blend` fueron exportados como GLB animados en `assets/models/propios/papel_picado_animado/`.

Cada archivo conserva las 13 shape keys y el ciclo de viento de 96 fotogramas. Se usa GLB/glTF porque el cargador actual del proyecto está basado en `cgltf`; FBX requeriría añadir otro importador.

## Integración utilizada

La animación se horneó directamente desde Blender como una caché de vértices de 96 frames a 24 FPS. Los archivos `.pvc` están en `assets/models/propios/papel_picado_cache/`.

Cada frame contiene las posiciones evaluadas por Blender después de aplicar las 13 shape keys. La caché se normaliza contra la malla estática usada por OpenGL, por lo que conserva el pivote superior, escala, materiales e instancias de la escena. `Modelo::actualizarAnimacion` reproduce esos frames y actualiza únicamente los VBO de los seis papeles.

La escena carga dos recursos por variante: `papel_1` a `papel_6` permanecen estáticos para los bordes y pisos del altar; `papel_animado_1` a `papel_animado_6` se usan exclusivamente en las tiras suspendidas del portal y los faroles. Así, actualizar el VBO animado no modifica las instancias decorativas apoyadas en el altar.

Los faroles, materiales, luces y lógica de proximidad permanecen en OpenGL sin cambios. La onda matemática aproximada fue eliminada.
