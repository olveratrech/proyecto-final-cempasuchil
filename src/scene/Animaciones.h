// Animaciones.h - Las nueve animaciones activables con el teclado.
//
//   1  Mariposa monarca      código: máquina de estados, ruta Catmull-Rom y aleteo jerárquico
//   2  Recorrido de cámara   código: keyframes de posición y mira interpolados
//   3  Veladoras             código: estados de encendido y parpadeo procedural (GLSL)
//   4  Papel picado          Blender: caché de vértices (pausa / reanuda el viento)
//   5  Remolino de pétalos   Blender: pistas horneadas de 720 pétalos
//   6  Jarra sirviendo agua  Blender: pistas de la jarra, el nivel del agua y el chorro
//   7  Visitante 1           huesos: panteonero que deja pan de muerto en el altar
//   8  Visitante 2           huesos: calaca que visita una tumba, saluda y baila
//   9  Esqueleto bailando    huesos: baile de 12 s
//   L  Faroles               interacción de luz: apagar / encender
#pragma once

#include "render/Model.h"
#include "render/Renderer.h"
#include "scene/Camera.h"
#include "scene/Camino.h"
#include "scene/Escena.h"

#include <memory>
#include <string>
#include <vector>

// Muestras por cuadro exportadas desde Blender (formato en modelos-blender/animaciones/pistas.py).
struct Pista {
    unsigned fps = 24, cuadros = 0, elementos = 0, componentes = 0;
    std::vector<float> datos;
    bool cargar(const std::string& ruta, unsigned componentesEsperados);
    float duracion() const { return cuadros > 1 ? float(cuadros - 1) / float(fps) : 0.0f; }
    // Interpolación lineal entre los dos cuadros que rodean t (segundos).
    void evaluar(float t, unsigned elemento, float* salida) const;
};

// Personaje con esqueleto que recorre una secuencia de tramos (clip + trayectoria).
struct Visitante {
    struct Tramo {
        int clip = -1;
        float duracion = 0.0f;        // segundos; en tramos de caminata se deriva de la distancia
        bool ciclo = false;
        Camino ruta;                  // vacía si el personaje no se desplaza
        glm::vec3 lugar{0.0f};        // posición al terminar el tramo
        float giro = 0.0f;            // orientación al terminar el tramo (rad, 0 = +Z)
    };
    std::unique_ptr<Modelo> modelo;
    Nodo* nodo = nullptr;
    std::vector<Tramo> tramos;
    float tiempo = -1.0f;             // < 0: espera a que se presione su tecla
    float reloj = 0.0f;
    int espera = -1;                  // clip mientras espera
    bool esperaCiclo = true;

    void actualizar(float dt);
    void caminar(int clip, const std::vector<glm::vec3>& puntos);
    void quieto(int clip, float giro, float duracion = -1.0f, bool ciclo = false);
};

class Animaciones {
public:
    void cargar(Escena& escena);
    void liberar();
    void tecla(int tecla);
    void actualizar(float dt, Camara& camara, Renderer& renderer);
    bool recorridoActivo() const { return recorrido_ >= 0.0f; }
    const std::string& estado() const { return estado_; }

private:
    void colocarPetalos();
    void armarJarra(Escena& escena, Nodo* grupo);
    void actualizarJarra();
    void armarVisitantes(Escena& escena, Nodo* grupo);

    Escena* escena_ = nullptr;
    Nodo* grupo_ = nullptr;
    std::string estado_;

    // 1. Mariposa guía
    Nodo *mariposa_ = nullptr, *alaIzq_ = nullptr, *alaDer_ = nullptr;
    glm::vec3 reposoIzq_{0.0f}, reposoDer_{0.0f};
    Camino vuelo_;
    float tiempoMariposa_ = -1.0f, relojAlas_ = 0.0f;

    // 2. Recorrido de cámara
    Camino recorridoPos_, recorridoMira_;
    float recorrido_ = -1.0f;

    // 3. Veladoras y L. Faroles
    int velas_ = 0;                   // 0 encendidas, 1 apagando, 2 apagadas, 3 encendiendo
    float tiempoVelas_ = 0.0f, reloj_ = 0.0f, frenteVelas_ = -1e4f, nivelVelas_ = 1.0f;
    bool farolesEncendidos_ = true;
    float faroles_ = 1.0f;

    // 4. Papel picado
    bool viento_ = true;

    // 5. Remolino de pétalos
    Pista remolino_;
    Nodo* petalos_ = nullptr;
    std::unique_ptr<Lote> lotePetalos_;
    float tiempoRemolino_ = 0.0f;
    bool remolinoActivo_ = false;

    // 6. Jarra sirviendo agua
    Pista jarra_, chorro_;
    Nodo *grupoJarra_ = nullptr, *jarraNodo_ = nullptr, *aguaVaso_ = nullptr, *aguaJarra_ = nullptr, *chorroNodo_ = nullptr;
    std::unique_ptr<Modelo> modeloAguaJarra_, modeloChorro_;
    float tiempoJarra_ = 0.0f, baseAguaVaso_ = 0.0f;
    bool jarraActiva_ = false;

    // 7, 8 y 9. Personajes con huesos
    Visitante visitante1_, visitante2_, bailarin_;
    Nodo* ofrenda_ = nullptr;         // pan de muerto que carga el visitante 1
    int anclaOfrenda_ = -1;
    float sueltaOfrenda_ = 0.0f;      // instante en que el pan queda en el altar
};
