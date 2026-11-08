// Aplicación independiente para la entrega de texturizado.
#pragma once
#include "render/Renderer.h"
#include "scene/Camera.h"
#include "scene/Animaciones.h"
#include "scene/Escena.h"
#include <string>
struct GLFWwindow;
struct Opciones {
    int ancho=0, alto=0, msaa=4;
    bool comprobar=false;
    bool diagnosticarRendimiento=false;
    int vistaInicial=1;
    bool filtrarLuces=true, realzarAltar=true;
    std::string captura;
};
class App {
public:
    int ejecutar(const Opciones& opciones);
private:
    bool iniciar();
    int ejecutarDiagnostico();
    void terminar();
    void procesarEntrada(float dt);
    void aplicarVista(int indice);
    static void alTeclear(GLFWwindow*,int,int,int,int);
    static void alDesplazar(GLFWwindow*,double,double);
    GLFWwindow* ventana_=nullptr;
    Opciones op_;
    Escena escena_;
    Animaciones animaciones_;
    Renderer renderer_;
    Camara camara_;
    bool ratonActivo_=false;
    double ultimoX_=0, ultimoY_=0;
    int anchoFb_=1, altoFb_=1;
};
