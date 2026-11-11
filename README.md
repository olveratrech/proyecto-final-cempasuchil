# CEMPASÚCHIL: OFRENDA, MEMORIA Y TRADICIÓN

[Descargar Release v1.0.0 para Windows](https://github.com/olveratrech/proyecto-final-cempasuchil/releases/latest) ·
[Documentación en español](doc/esp/Equipo-8-proyecto-final-esp.pdf) ·
[Documentación en inglés](doc/eng/Equipo-8-final-project-eng.pdf) ·
[Video demostrativo](https://youtu.be/2iYnnTLFxNo)

Proyecto final de **Computación Gráfica e Interacción Humano-Computadora**  
Universidad Nacional Autónoma de México — Facultad de Ingeniería  
Semestre 2027-1 · Grupo 04  
Profesor: **Ing. José Ramón Pérez Athié**

## Equipo 8

| Integrante | Rol |
|---|---|
| Basilio Illescas Andrés | Product Owner |
| Trejo Olvera Emmanuel | Scrum Master |
| Pérez Olvera Alexis Abraham | Developer |
| Cruz Macedo Samuel Santiago | Developer |

## Descripción

**CEMPASÚCHIL** es una experiencia gráfica interactiva en 3D desarrollada con C++17, OpenGL, GLSL y Blender. La escena representa una noche de Día de Muertos: un visitante entra a un panteón, recorre un sendero de pétalos entre tumbas, veladoras, faroles, mariposas monarca y papel picado, y finalmente llega a una ofrenda de tres niveles instalada en el portal de una casa.

La ofrenda está dedicada a Janis Joplin, Kurt Cobain, Valentín Elizalde, Amy Winehouse, Jim Morrison, Jimi Hendrix y Kristen Pfaff, figuras de la música que murieron a los 27 años. El altar funciona como un espacio de memoria sobre el talento, la fragilidad, las vidas marcadas por la presión, los excesos y la violencia, y, sobre todo, la permanencia de la obra artística. La intención no es romantizar el sufrimiento, sino recordar a las personas que existieron detrás de las figuras públicas.

El proyecto integra el pipeline gráfico completo: conceptualización, modelado geométrico y jerárquico, transformaciones, texturizado, color, iluminación, materiales, animación, interacción y renderizado en tiempo real.

## Características principales

- Escena recorrible formada por un panteón, un sendero de pétalos, una casa de adobe y una ofrenda de tres niveles.
- 2,134 nodos, 81 modelos cargados y 10,549 instancias en la versión final.
- Modelos propios creados en Blender y modelos CC0 de apoyo.
- Grafo de escena jerárquico e instanciado para miles de elementos repetidos.
- Materiales con color base, rugosidad, metalicidad, normales, emisión, transparencia y recorte alfa.
- Texturas procedurales creadas y horneadas en Blender.
- Iluminación OpenGL dinámica con Phong, luces puntuales y aproximación de Fresnel para vidrio y agua.
- Skybox mediante mapa cúbico para ambientar la escena nocturna.
- Navegación en primera persona con balanceo suave de caminata y vuelo libre opcional.
- Filtrado de luces por caja envolvente, descarte por frustum, caché de materiales e instanciado.
- Nueve animaciones interactivas: tres por código, tres creadas en Blender y tres mediante huesos.

## Tecnologías

| Tecnología | Uso |
|---|---|
| C++17 | Aplicación, escena, interacción y animación procedural |
| OpenGL / GLSL | Renderizado, materiales, iluminación y actualización de vértices |
| Blender 5.2 | Modelado, texturizado, horneado, animación y esqueletos |
| glTF 2.0 / cgltf | Intercambio y carga de mallas, materiales, nodos, huesos y clips |
| GLFW, GLAD y GLM | Ventana, entrada, funciones OpenGL y matemáticas |
| stb_image / ImageIO | Decodificación de imágenes multiplataforma |
| CMake | Configuración y compilación multiplataforma |

Las dependencias necesarias para compilar están incluidas en `external/`; CMake no necesita descargarlas.

## Descarga e instalación en Windows

La versión empaquetada se encuentra en [GitHub Releases](https://github.com/olveratrech/proyecto-final-cempasuchil/releases/latest). No es necesario descargar el código fuente ni instalar CMake, Visual Studio o Blender.

1. Abrir la [versión más reciente](https://github.com/olveratrech/proyecto-final-cempasuchil/releases/latest).
2. Descargar `CEMPASUCHIL-Setup-0.2.0-win64.exe` desde **Assets**.
3. Ejecutar el instalador y seguir las indicaciones del asistente.
4. Abrir **CEMPASÚCHIL** desde el menú Inicio o desde el acceso directo creado por el instalador.

El instalador incorpora `CEMPASUCHIL.exe`, los modelos, texturas, animaciones, shaders, licencia, instrucciones de uso y desinstalador. Al ser un proyecto académico sin firma digital, Windows puede mostrar una advertencia antes de abrirlo.

### Requisitos de ejecución

- Windows 10 u 11 de 64 bits.
- GPU y controlador compatibles con OpenGL 3.3 Core o superior.
- 4 GB de memoria RAM como mínimo.
- Aproximadamente 1 GB de espacio disponible.
- Teclado y ratón.

Una máquina virtual puede instalar o compilar correctamente el programa y aun así no ejecutar la escena si su controlador virtual no expone OpenGL 3.3.

## Requisitos para compilar el código fuente

- GPU y controlador compatibles con OpenGL 3.3 Core o superior.
- CMake 3.16 o superior.
- Compilador compatible con C++17.
- En macOS: herramientas de línea de comandos de Xcode.
- En Windows: Visual Studio 2022 o posterior con *Desarrollo para el escritorio con C++*.
- En Linux: compilador C++, OpenGL y bibliotecas de desarrollo X11 requeridas por GLFW.

## Compilación y ejecución

### macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/TexturizadoCempasuchil
```

### Windows con Visual Studio

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
build\Release\CEMPASUCHIL.exe
```

También puede abrirse `ejecutables/windows/TexturizadoCempasuchil.vbs`.

### Linux

En Debian o Ubuntu:

```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev \
  libxinerama-dev libxcursor-dev libxi-dev libgl-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/TexturizadoCempasuchil
```

También puede utilizarse `ejecutables/linux/iniciar_linux.sh` o el acceso `.desktop`.

Los lanzadores y sus requisitos particulares se explican en [ejecutables/README.md](ejecutables/README.md).

## Generación automática del instalador

El workflow [Generar instalador de Windows](.github/workflows/windows-installer.yml) prepara la entrega de Windows mediante GitHub Actions:

1. Utiliza un runner con Windows Server 2022.
2. Configura CMake con Visual Studio 2022 y arquitectura x64.
3. Compila `CEMPASUCHIL.exe` en modo `Release`.
4. Verifica el ejecutable, `assets/`, `shaders/` y los archivos de instrucciones.
5. Empaqueta la aplicación con CPack y NSIS.
6. Publica el instalador y su suma SHA-256 como artefactos.

El workflow puede iniciarse manualmente desde GitHub Actions y también se ejecuta al publicar una etiqueta cuyo nombre comienza con `v`. La entrega permanente se conserva en [GitHub Releases](https://github.com/olveratrech/proyecto-final-cempasuchil/releases/latest), ya que los artefactos de Actions tienen vigencia limitada.

## Controles

| Tecla o acción | Función |
|---|---|
| `W A S D` | Caminar por la escena |
| `Shift` | Aumentar temporalmente la velocidad |
| `TAB` | Capturar o liberar el ratón para orientar la cámara |
| Movimiento del ratón | Orientar la vista cuando el control está activo |
| Rueda del ratón | Ajustar la velocidad de desplazamiento |
| `F` | Alternar entre caminata y vuelo libre |
| `Q / E` | Descender o ascender en modo vuelo |
| `F2` | Alternar entre renderizado sólido y malla de alambre |
| `R` | Regresar a la entrada del panteón mirando hacia el altar |
| `1` a `9` | Activar o controlar las animaciones |
| `L` | Apagar o encender gradualmente los faroles |
| `Esc` | Cerrar la aplicación |

La cámara inicia en la entrada del panteón, a una altura aproximada de 1.3 m, orientada hacia el altar. En modo caminata permanece a nivel del suelo y aplica un balanceo sutil para simular pasos; `Q` y `E` solo funcionan durante el vuelo.

## Animaciones

| Tecla | Animación | Medio | Técnica principal |
|---|---|---|---|
| `1` | Mariposa monarca | Código | Máquina de estados, Catmull-Rom y aleteo jerárquico |
| `2` | Recorrido de cámara | Código | Keyframes de posición y mirada con interpolación suave |
| `3` | Veladoras | Código | Estados, encendido progresivo y parpadeo procedural |
| `4` | Papel picado | Blender | Caché de vértices de 96 cuadros con deformación por viento |
| `5` | Remolino de pétalos | Blender | Keyframes horneados de 720 pétalos |
| `6` | Jarra sirviendo agua | Blender | Pistas de jarra, agua interior, vaso y chorro |
| `7` | Visitante con ofrenda | Huesos | Caminata, depósito del pan y reverencia |
| `8` | Visitante entre tumbas | Huesos | Recorrido, observación, saludo y baile |
| `9` | Esqueleto bailando | Huesos | Clip de baile junto a la entrada del panteón |

Las tres animaciones externas fueron creadas en Blender y exportadas a datos que OpenGL reproduce directamente. Los personajes se almacenan como glTF con `skin`, huesos y clips; el motor interpola sus pistas y deforma las mallas durante la ejecución.

## Opciones de línea de comandos

Las opciones se escriben al ejecutar el programa desde una terminal:

| Opción | Función |
|---|---|
| `--vista N` | Seleccionar una vista inicial de `0` a `6` |
| `--entrada` | Iniciar en la entrada del panteón |
| `--altar` | Iniciar con una vista frontal del altar |
| `--captura ARCHIVO` | Renderizar y guardar una captura en formato PPM |

Ejemplo:

```bash
./build/TexturizadoCempasuchil --altar --captura altar.ppm
```

## Estructura del proyecto

```text
.
├── CMakeLists.txt               Configuración de compilación
├── README.md                    Guía general
├── README.txt                   Instrucciones incluidas en el instalador
├── LICENSE
├── .github/workflows/           Automatización del instalador de Windows
├── src/                         Aplicación, renderizador y escena
│   ├── render/                  Modelos, shaders, skybox e imágenes
│   └── scene/                   Cámara, grafo, composición y animaciones
├── shaders/                     Programas GLSL
├── assets/
│   ├── models/propios/          Modelos creados por el equipo
│   ├── models/libreria/         Modelos CC0 de apoyo
│   ├── models/animados/         Personajes con esqueletos y clips
│   ├── animaciones/             Pistas horneadas desde Blender
│   ├── textures/                Texturas y fotografías procesadas
│   └── skybox/                  Mapa cúbico del entorno
├── modelos-blender/             Fuentes de modelado y animación
├── ejecutables/                 Lanzadores para macOS, Windows y Linux
├── doc/                         Reportes en español e inglés
└── external/                    Dependencias incluidas
```

Las carpetas `build/` y `build-*` son productos locales regenerables y no forman parte del código fuente.

## Documentación

- [Documentación técnica y de usuario en español](doc/esp/Equipo-8-proyecto-final-esp.pdf)
- [Technical and user documentation in English](doc/eng/Equipo-8-final-project-eng.pdf)
- [Instrucciones del instalador para Windows](README.txt)
- [Video demostrativo en YouTube](https://youtu.be/2iYnnTLFxNo)
- [Instalador y suma SHA-256](https://github.com/olveratrech/proyecto-final-cempasuchil/releases/latest)

El reporte completo documenta la planeación, metodología ágil, evolución por entregas, modelado, texturizado, iluminación, animaciones, optimización, compatibilidad multiplataforma, resultados, evidencias visuales y distribución del ejecutable.

## Video demostrativo

El video presenta el recorrido completo, los modelos principales, la iluminación dinámica, los controles de navegación, las tres animaciones por código, las tres creadas en Blender y las tres animaciones por huesos.

[Ver el video demostrativo de CEMPASÚCHIL](https://youtu.be/2iYnnTLFxNo)

## Créditos y licencias

- Modelos de apoyo: [Kenney Graveyard Kit](https://kenney.nl/assets/graveyard-kit), licencia CC0.
- Recursos de entorno y texturas CC0 de [Poly Haven](https://polyhaven.com), cuando se indica en la documentación.
- Bibliotecas: GLFW, GLAD, GLM, cgltf y stb.
- El código y los recursos propios se distribuyen conforme a [LICENSE](LICENSE).
