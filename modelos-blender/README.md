# Fuentes Blender de los modelos propios

Esta carpeta reúne las fuentes `.blend` editables correspondientes a los modelos propios que carga la escena. Está separada del código y de los glTF empleados en tiempo de ejecución: la aplicación continúa leyendo `assets/models/propios`.

Los recursos de imagen utilizados están empacados dentro de cada `.blend`. Los 39 archivos fueron auditados y el resultado quedó registrado en `auditoria-recursos.json` con cero recursos faltantes. Por ello pueden abrirse desde un clon independiente de `texturizado-entrega` sin depender de `Re-Modelaje` ni de rutas existentes en la computadora original.

## Organización

- `altar`, `arco`, `flor` y `petalo`: elementos centrales de la ofrenda.
- `veladoras` y `cirios`: variantes de iluminación decorativa.
- `papel_picado`: archivo consolidado con las variantes del papel.
- `portarretratos`: archivo consolidado con sus variantes.
- `casas`: casa general y variante destinada al altar.
- `platos` y `dulces`: alimentos de la ofrenda.
- El resto de las carpetas corresponde directamente a la categoría homónima del catálogo.

Cuando existe una versión optimizada usada en la entrega actual, se conserva esa fuente y no una copia anterior de alta densidad.

## Excepciones

- `cabeza_cempasuchil` es un derivado de `flor_cempasuchil`; no necesita otra fuente Blender.
- `entrada_panteon` no tiene un `.blend` fuente dentro de `Re-Modelaje`; la escena usa directamente su glTF existente.
- Los modelos de `assets/models/libreria/kenney_graveyard` pertenecen al paquete externo Kenney Graveyard y sus fuentes disponibles son `.glb`, no `.blend`. No se duplican ni convierten artificialmente aquí.
- Las texturas originales de ejecución también permanecen en `assets/textures`, aunque las imágenes necesarias ya están empacadas en los `.blend`.

Estos archivos son fuentes de trabajo. Modificarlos no cambia automáticamente los glTF de la escena; cualquier actualización debe exportarse y revisarse de forma explícita.

Los scripts `empaquetar_recursos.py` y `empaquetar_todos_desde_blender.py` permiten repetir la auditoría si posteriormente se agregan o reemplazan modelos.
