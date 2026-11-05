
# Proyecto final de **Computación Gráfica e Interacción Humano-Computadora**  
Universidad Nacional Autónoma de México — Facultad de Ingeniería
Semestre 2027-1 
Prof. Ing. José Ramón Pérez Athié

## Cempasúchil — Ofrenda, memoria y tradición

## Descripción

**Cempasúchil** es una experiencia gráfica interactiva en 3D desarrollada con **C++17, OpenGL y GLSL**. La escena representa una noche de Día de Muertos: el visitante recorre un panteón, sigue un sendero cubierto de pétalos entre faroles, veladoras y papel picado, y llega a una ofrenda de tres niveles instalada en el portal de una casa de adobe.

El proyecto integra modelado, composición, texturizado, color, iluminación y animación. Todos los modelos, materiales, luces y animaciones se calculan en tiempo real mediante OpenGL.

## Características principales

- La escena completa esta compuesta por un panteón, camino de pelatos de cempasúchil, casas, un altar, faroles, un altar y ofrendas de todo tipo, veladoras y un detalle importente: mariposas monarca.
- Modelos propios creados en Blender y elementos CC0 de Kenney *Graveyard Kit*.
- Materiales con color base, rugosidad, metalicidad, normales, emisión, recorte alfa y soporte para vidrio y líquidos.
- Iluminación OpenGL dinámica para cirios, veladoras, faroles y un relleno cálido del altar.
- Filtrado de luces por caja envolvente, caché de materiales, instanciado y descarte por *frustum*.
- Skybox nocturno y composición completa del entorno.

## Tecnologías

| Tecnología | Uso |
|---|---|
| C++17 | Aplicación, escena, interacción y animación procedural |
| OpenGL / GLSL | Renderizado, materiales, iluminación y actualización de vértices |
| Blender 5.2 | Modelado, texturizado y animación del papel picado |
| glTF 2.0 / cgltf | Intercambio y carga de modelos y materiales |
| CMake | Configuración y compilación multiplataforma |
| GLFW, GLAD y GLM | Ventana, contexto OpenGL y matemáticas |

Las dependencias están incluidas en `external/`; la compilación no necesita descargarlas.

## Compilación de la escena completa

### macOS

Se requieren CMake y las herramientas de línea de comandos de Xcode.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/TexturizadoCempasuchil
```

También puede utilizarse el lanzador de `ejecutables/macOS/`.

### Windows con Visual Studio 2022

Instalar Visual Studio con **Desarrollo para el escritorio con C++**.

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
build\Release\TexturizadoCempasuchil.exe
```

### Linux

```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/TexturizadoCempasuchil
```

## Controles

| Tecla o acción | Función |
|---|---|
| `W A S D` | Caminar por la escena |
| `Shift` | Aumentar la velocidad de movimiento |
| Mover el ratón | Orientar la cámara cuando el control está activo |
| `TAB` | Activar o desactivar el control de cámara con el ratón |
| Rueda del ratón | Ajustar la velocidad de desplazamiento |
| `F` | Alternar entre caminata y vuelo libre |
| `Q / E` | Descender / ascender en modo vuelo |
| `F2` | Alternar entre renderizado sólido y malla de alambre |
| `R` | Regresar a la entrada del panteón mirando hacia el altar |
| `Esc` | Cerrar la aplicación |

## Opciones de línea de comandos

| Opción | Función |
|---|---|
| `--vista N` | Vista inicial, de `0` a `6` |
| `--entrada` | Entrada del panteón |
| `--altar` | Vista del altar |
| `--captura ARCHIVO` | Guardar una captura PPM |
## Estructura del repositorio

```text
├── CMakeLists.txt
├── README.md
├── readme.txt
├── src/                          código C++ de renderizado y escena
├── shaders/                      programas GLSL
├── assets/
│   ├── models/propios/           modelos glTF del equipo
│   ├── models/libreria/          modelos CC0
│   │ 
│   ├── models/propios/papel_picado_cache/
│   │                             caché de 96 frames de Blender
│   │ 
│   ├── textures/                 texturas y materiales
│   └── skybox/                   entorno panorámico
├── modelos-blender/              fuentes de modelado y animación
├── ejecutables/                  lanzadores multiplataforma
├── doc/                          reportes y documentación técnica
└── external/                     dependencias incluidas
```

## Documentación

- [Reporte técnico en español](doc/esp/Equipo-8-proyecto-final-esp.pdf)
- [Technical report in English](doc/eng/Equipo-8-final-project-eng.pdf)

## Equipo

- Basilio Illescas Andres — Product Owner
- Trejo Olvera Emmanuel — Scrum Master
- Pérez Olvera Alexis Abraham — Developer
- Cruz Macedo Samuel Santiago — Developer

## Créditos de terceros

- Modelos: [Kenney — Graveyard Kit](https://kenney.nl/assets/graveyard-kit) (CC0).
- Texturas y entorno: [Poly Haven](https://polyhaven.com) (CC0), cuando se indica en la documentación.
- Bibliotecas: GLFW, GLAD, GLM, cgltf y stb.

---

