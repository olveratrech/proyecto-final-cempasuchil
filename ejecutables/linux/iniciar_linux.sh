#!/bin/sh
set -u

DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$DIR/../.." && pwd)
LOG_DIR="$ROOT/logs"
LOG="$LOG_DIR/lanzador-linux.log"
BUILD="$ROOT/build-linux"
mkdir -p "$LOG_DIR"
printf '\n===== %s Linux =====\n' "$(date '+%Y-%m-%d %H:%M:%S')" >>"$LOG"

if ! command -v cmake >/dev/null 2>&1; then
  echo "ERROR: No se encontró CMake." >>"$LOG"
  command -v notify-send >/dev/null 2>&1 && notify-send "Texturizado Cempasúchil" "No se encontró CMake. Revisa la carpeta logs."
  exit 3
fi

# CMake guarda rutas absolutas dentro de CMakeCache.txt. Si el repositorio se
# mueve o se copia junto con build-linux, esa caché deja de ser reutilizable.
# build-linux sólo contiene resultados regenerables, por lo que se reconstruye
# automáticamente cuando pertenece a otra ubicación del proyecto.
CACHE="$BUILD/CMakeCache.txt"
if [ -f "$CACHE" ]; then
  CACHE_SOURCE=$(sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$CACHE" | tail -n 1)
  CACHE_BUILD=$(sed -n 's/^CMAKE_CACHEFILE_DIR:INTERNAL=//p' "$CACHE" | tail -n 1)
  if [ "$CACHE_SOURCE" != "$ROOT" ] ||
     { [ -n "$CACHE_BUILD" ] && [ "$CACHE_BUILD" != "$BUILD" ]; }; then
    printf 'INFO: La caché de CMake corresponde a otra ruta. Regenerando %s.\n' "$BUILD" >>"$LOG"
    if [ "$BUILD" != "$ROOT/build-linux" ] ||
       ! cmake -E remove_directory "$BUILD" >>"$LOG" 2>&1; then
      echo "ERROR: No se pudo limpiar la caché trasladada de CMake." >>"$LOG"
      command -v notify-send >/dev/null 2>&1 && notify-send "Texturizado Cempasúchil" "No se pudo regenerar build-linux. Revisa la carpeta logs."
      exit 4
    fi
  fi
fi

if ! cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release >>"$LOG" 2>&1 ||
   ! cmake --build "$BUILD" --parallel 4 >>"$LOG" 2>&1; then
  command -v notify-send >/dev/null 2>&1 && notify-send "Texturizado Cempasúchil" "Falló la compilación. Revisa la carpeta logs."
  exit 4
fi
cmake -E copy_directory "$ROOT/assets" "$BUILD/assets" >>"$LOG" 2>&1
cmake -E copy_directory "$ROOT/shaders" "$BUILD/shaders" >>"$LOG" 2>&1
cd "$BUILD" || exit 5
nohup "$BUILD/TexturizadoCempasuchil" >>"$LOG" 2>&1 &
