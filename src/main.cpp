// Entrega independiente de texturizado.
#include "App.h"
#include "Registro.h"
#include "Rutas.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>

int main(int argc, char** argv) {
    Opciones op;
    for(int i=1;i<argc;++i) {
        const std::string argumento=argv[i];
        if(argumento=="--captura" && i+1<argc)op.captura=argv[++i];
        if(argumento=="--vista" && i+1<argc)op.vistaInicial=std::atoi(argv[++i]);
        if(argumento=="--altar")op.vistaInicial=3;
        if(argumento=="--entrada")op.vistaInicial=1;
    }
    if (!rutas::inicializar(argc > 0 ? argv[0] : nullptr)) {
        registro::fatal("No se encontraron las carpetas assets/ y shaders/ junto al programa.");
        return 1;
    }
    registro::abrir((std::filesystem::u8path(rutas::base()) / "registro.txt").u8string());
    registro::info("Carpeta base: %s", rutas::base().c_str());

    App app;
    int codigo = app.ejecutar(op);
    registro::cerrar();
    return codigo;
}
