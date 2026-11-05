PROYECTO FINAL DE COMPUTACIÓN GRÁFICA E INTERACCIÓN HUMANO-COMPUTADORA
CEMPASÚCHIL - Ofrenda, memoria y tradición

Universidad Nacional Autónoma de México - Facultad de Ingeniería
Semestre 2027-1
Prof. Ing. José Ramón Pérez Athié

---------------------------------------------------------------------
DESCRIPCIÓN
---------------------------------------------------------------------
  Cempasúchil es una experiencia gráfica interactiva en 3D desarrollada
  con C++17, OpenGL y GLSL. La escena representa una noche de Día de
  Muertos: el visitante recorre un panteón, sigue un sendero cubierto de
  pétalos entre faroles, veladoras y papel picado, y llega a una ofrenda
  de tres niveles instalada en el portal de una casa de adobe.

  El proyecto integra modelado, composición, texturizado, color,
  iluminación y animación. Todos los modelos, materiales, luces y
  animaciones se calculan en tiempo real mediante OpenGL.

---------------------------------------------------------------------
CARACTERÍSTICAS PRINCIPALES
---------------------------------------------------------------------
  - Escena compuesta por un panteón, sendero de pétalos de cempasúchil,
    casas, altar, faroles, veladoras, ofrendas y mariposas monarca.
  - Modelos propios creados en Blender y elementos CC0 de Kenney
    Graveyard Kit.
  - Materiales con color base, rugosidad, metalicidad, normales,
    emisión, recorte alfa y soporte para vidrio y líquidos.
  - Iluminación OpenGL dinámica para cirios, veladoras, faroles y un
    relleno cálido del altar.
  - Filtrado de luces por caja envolvente, caché de materiales,
    instanciado y descarte por frustum.
  - Skybox nocturno y composición completa del entorno.

---------------------------------------------------------------------
TECNOLOGÍAS
---------------------------------------------------------------------
  C++17 .............. aplicación, escena, interacción y animación
  OpenGL / GLSL ...... renderizado, materiales e iluminación
  Blender 5.2 ........ modelado, texturizado y papel picado animado
  glTF 2.0 / cgltf ... intercambio y carga de modelos y materiales
  CMake .............. configuración y compilación multiplataforma
  GLFW, GLAD y GLM ... ventana, contexto OpenGL y matemáticas

  Las dependencias están incluidas en external/; la compilación no
  necesita descargarlas.

---------------------------------------------------------------------
COMPILACIÓN DE LA ESCENA COMPLETA
---------------------------------------------------------------------
  macOS
    Se requieren CMake y las herramientas de línea de comandos de Xcode.

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j
    ./build/TexturizadoCempasuchil

    También puede utilizarse el lanzador de ejecutables/macOS/.

  Windows con Visual Studio 2022
    Instalar Visual Studio con la carga de trabajo Desarrollo para el
    escritorio con C++.

    cmake -S . -B build -G "Visual Studio 17 2022" -A x64
    cmake --build build --config Release
    build\Release\TexturizadoCempasuchil.exe

  Linux
    Instalar CMake, un compilador C++ y las bibliotecas de desarrollo de
    X11 y OpenGL requeridas por GLFW.

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j
    ./build/TexturizadoCempasuchil

---------------------------------------------------------------------
CONTROLES
---------------------------------------------------------------------
  W A S D ............ caminar por la escena
  Shift .............. aumentar la velocidad de movimiento
  Mover el ratón ..... orientar la cámara cuando el control está activo
  TAB ................. activar / desactivar el control con el ratón
  Rueda del ratón .... ajustar la velocidad de desplazamiento
  F .................. alternar entre caminata y vuelo libre
  Q / E .............. descender / ascender en modo vuelo
  F2 ................. alternar sólido / malla de alambre
  R .................. regresar a la entrada mirando hacia el altar
  Esc ................ cerrar la aplicación

---------------------------------------------------------------------
OPCIONES DE LÍNEA DE COMANDOS
---------------------------------------------------------------------
  Estas opciones se escriben en una terminal después del ejecutable.

  --vista N .......... vista inicial, de 0 a 6
  --entrada .......... entrada del panteón
  --altar ............ vista del altar
  --captura ARCHIVO .. guardar una captura PPM

  Ejemplo:
    ./build/TexturizadoCempasuchil --vista 3

---------------------------------------------------------------------
ESTRUCTURA DEL REPOSITORIO
---------------------------------------------------------------------
  CMakeLists.txt ................ configuración de CMake
  README.md ..................... documentación principal
  readme.txt .................... instrucciones en texto plano
  src/ .......................... código C++ de renderizado y escena
  shaders/ ...................... programas GLSL
  assets/models/propios/ ........ modelos glTF del equipo
  assets/models/libreria/ ....... modelos CC0
  assets/models/propios/
    papel_picado_cache/ ......... caché de 96 frames de Blender
  assets/textures/ .............. texturas y materiales
  assets/skybox/ ................ entorno panorámico
  modelos-blender/ .............. fuentes de modelado y animación
  ejecutables/ .................. lanzadores multiplataforma
  doc/ .......................... reportes y documentación técnica
  external/ ..................... dependencias incluidas

---------------------------------------------------------------------
DOCUMENTACIÓN
---------------------------------------------------------------------
  Español:
    doc/esp/Equipo-8-proyecto-final-esp.pdf

  Inglés:
    doc/eng/Equipo-8-proyecto-final-eng.pdf

---------------------------------------------------------------------
EQUIPO
---------------------------------------------------------------------
  Basilio Illescas Andres ......... Product Owner
  Trejo Olvera Emmanuel ........... Scrum Master
  Pérez Olvera Alexis Abraham ..... Developer
  Cruz Macedo Samuel Santiago ..... Developer

---------------------------------------------------------------------
CRÉDITOS DE TERCEROS
---------------------------------------------------------------------
  Modelos: Kenney, Graveyard Kit (CC0) - kenney.nl
  Texturas y entorno: Poly Haven (CC0), cuando se indica en la
  documentación.
  Bibliotecas: GLFW, GLAD, GLM, cgltf y stb.
