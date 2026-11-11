cmake_minimum_required(VERSION 3.16...3.31)

project(TexturizadoCempasuchil
    VERSION 0.2.0
    DESCRIPTION "CEMPASÚCHIL - Proyecto final CGeIHC, FI UNAM"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilación" FORCE)
endif()

# En Windows se enlaza el runtime de MSVC de forma estática para que el .exe
# funcione en equipos sin el paquete redistribuible de Visual C++.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --------------------------------------------------------------------------
# Dependencias (incluidas en external/, no requieren conexión a internet)
# --------------------------------------------------------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# --------------------------------------------------------------------------
# Programa
# --------------------------------------------------------------------------
add_executable(TexturizadoCempasuchil
    src/main.cpp
    src/App.cpp
    src/Registro.cpp
    src/Rutas.cpp
    src/render/Shader.cpp
    src/render/ImageLoader.cpp
    src/render/Skybox.cpp
    src/render/Model.cpp
    src/render/Renderer.cpp
    src/scene/Camera.cpp
    src/scene/SceneNode.cpp
    src/scene/Camino.cpp
    src/scene/Escena.cpp
    src/scene/AltarAprobado.cpp
    src/scene/Animaciones.cpp
    src/bibliotecas.cpp)

# Generar un esquema compartido con ejecutable de Run definido.
set_target_properties(TexturizadoCempasuchil PROPERTIES
    XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_include_directories(TexturizadoCempasuchil PRIVATE
    src
    external/glm
    external/cgltf
    external/stb)

target_link_libraries(TexturizadoCempasuchil PRIVATE glfw glad)
target_compile_definitions(TexturizadoCempasuchil PRIVATE GLM_ENABLE_EXPERIMENTAL)

if(APPLE)
    find_library(IMAGEIO_FRAMEWORK ImageIO REQUIRED)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(TexturizadoCempasuchil PRIVATE
        ${IMAGEIO_FRAMEWORK} ${COREGRAPHICS_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK})
endif()

if(MSVC)
    target_compile_options(TexturizadoCempasuchil PRIVATE /W3 /utf-8)
    target_compile_definitions(TexturizadoCempasuchil PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
    # aplicación de ventana (sin consola); los mensajes se guardan en registro.txt
    set_target_properties(TexturizadoCempasuchil PROPERTIES
        OUTPUT_NAME "CEMPASUCHIL"
        WIN32_EXECUTABLE TRUE
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    target_link_options(TexturizadoCempasuchil PRIVATE /ENTRY:mainCRTStartup)
else()
    target_compile_options(TexturizadoCempasuchil PRIVATE -Wall -Wextra -Wno-unused-parameter)
    if(WIN32)
        target_link_options(TexturizadoCempasuchil PRIVATE -mwindows -static)
    endif()
endif()

# Las bibliotecas de un solo encabezado se compilan sin advertencias (código de terceros)
if(MSVC)
    set_source_files_properties(src/bibliotecas.cpp PROPERTIES COMPILE_OPTIONS "/W0")
else()
    set_source_files_properties(src/bibliotecas.cpp PROPERTIES COMPILE_OPTIONS "-w")
endif()

# Copia assets/, shaders/ y la documentación general junto al ejecutable para
# que la carpeta de salida se pueda empaquetar directamente (InstallForge).
add_custom_command(TARGET TexturizadoCempasuchil POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:TexturizadoCempasuchil>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:TexturizadoCempasuchil>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/README.md $<TARGET_FILE_DIR:TexturizadoCempasuchil>/README.md
    COMMENT "Copiando assets, shaders y README.md junto al ejecutable")

# --------------------------------------------------------------------------
# Instalador de Windows (CPack + NSIS)
# --------------------------------------------------------------------------
# Estas reglas crean una instalación autocontenida. El usuario final no
# necesita CMake, Visual Studio ni las fuentes del proyecto.
if(WIN32)
    install(TARGETS TexturizadoCempasuchil
        RUNTIME DESTINATION .)
    install(DIRECTORY assets
        DESTINATION .)
    install(DIRECTORY shaders
        DESTINATION .)
    install(FILES
        "${CMAKE_SOURCE_DIR}/README.txt"
        "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION .)

    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "CEMPASUCHIL")
    set(CPACK_PACKAGE_VENDOR "Equipo 8 - Facultad de Ingeniería, UNAM")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
        "Ofrenda, memoria y tradición - experiencia interactiva de Día de Muertos")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "CEMPASUCHIL")
    set(CPACK_PACKAGE_FILE_NAME
        "CEMPASUCHIL-Setup-${PROJECT_VERSION}-win64")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

    set(CPACK_NSIS_DISPLAY_NAME "CEMPASÚCHIL")
    set(CPACK_NSIS_PACKAGE_NAME "CEMPASÚCHIL")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "CEMPASUCHIL.exe")
    set(CPACK_PACKAGE_EXECUTABLES
        "CEMPASUCHIL" "CEMPASÚCHIL")
    set(CPACK_CREATE_DESKTOP_LINKS "CEMPASUCHIL")
    set(CPACK_NSIS_MENU_LINKS
        "README.txt" "Instrucciones de uso")

    include(CPack)
endif()
