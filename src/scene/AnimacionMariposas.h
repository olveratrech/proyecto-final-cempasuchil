#pragma once
#include "scene/SceneNode.h"
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <array>
#include <cmath>

// Solo modifica un nodo de vuelo y las articulaciones de esta instancia.
// El nodo de colocación original conserva posición, rotación, escala y selección.
struct AnimacionMariposa {
    static constexpr float encenderMetros = 5.0f;
    static constexpr float apagarMetros = 10.0f;
    Nodo* ancla = nullptr;
    Nodo* vuelo = nullptr;
    Nodo* izquierda = nullptr;
    Nodo* derecha = nullptr;
    glm::vec3 reposoIzq{0}, reposoDer{0};
    double tiempo = 0;
    float desfase = 0;
    float intensidad = 0;
    bool activa = false;

    static glm::vec3 ruta(double tiempo) {
        static const std::array<glm::vec3,8> puntos{{
            {-1.65f,.05f,.15f},{-1.05f,.50f,-.85f},{.25f,.30f,-1.10f},
            {1.55f,0,-.45f},{1.60f,.40f,.65f},{.40f,.65f,1.05f},
            {-.70f,.10f,.75f},{-1.45f,-.30f,.85f}}};
        double fase=std::fmod(tiempo/1.3,8.0);
        if(fase<0) fase+=8;
        int i=static_cast<int>(fase);
        float t=static_cast<float>(fase-i);
        const auto& a=puntos[(i+7)%8]; const auto& b=puntos[i];
        const auto& c=puntos[(i+1)%8]; const auto& d=puntos[(i+2)%8];
        return .125f*((2.f*b)+(-a+c)*t+(2.f*a-5.f*b+4.f*c-d)*t*t+(-a+3.f*b-3.f*c+d)*t*t*t);
    }

    void actualizar(float dt, const glm::vec3& ojo) {
        // Distancia al emplazamiento fijo en coordenadas de mundo, no al punto
        // oscilante: el propio vuelo no puede disparar encendidos/apagados.
        const glm::mat4 padre=ancla->padre ? ancla->padre->mundo : glm::mat4(1);
        const glm::mat4 mundo=padre*ancla->matrizLocal();
        const float distancia=glm::length(glm::vec3(mundo[3])-ojo);
        if(distancia<=encenderMetros) activa=true;
        else if(distancia>=apagarMetros) activa=false;
        dt=std::clamp(dt,0.f,.1f);
        intensidad=std::clamp(intensidad+(activa?dt:-dt)*1.5f,0.f,1.f);
        if(intensidad==0 && !activa) {
            vuelo->posicion=glm::vec3(0); vuelo->rotacion=glm::vec3(0);
            izquierda->rotacion=reposoIzq; derecha->rotacion=reposoDer;
            ancla->actualizar(padre);
            return; // El reloj de esta mariposa queda detenido.
        }
        const float peso=intensidad*intensidad*(3-2*intensidad);
        tiempo+=dt*peso;
        const double t=tiempo+desfase;
        // Ruta pequeña alrededor del emplazamiento; no invade todo el escenario.
        vuelo->posicion=peso*(ruta(t)-ruta(desfase));
        vuelo->posicion.y+=peso*float(.018*std::sin(t*2.3));
        const glm::vec3 frente=glm::normalize(ruta(t+.01)-ruta(t-.01));
        const glm::vec3 lado=glm::normalize(glm::cross(frente,glm::vec3(0,1,0)));
        const glm::vec3 arriba=glm::cross(lado,frente);
        // glTF: cabeza -Z, vertical +Y (distinto del OBJ de la demostración).
        glm::quat orientacion=glm::quat_cast(glm::mat3(lado,arriba,-frente));
        float giro=glm::cross(glm::normalize(ruta(t)-ruta(t-.2)),
                              glm::normalize(ruta(t+.2)-ruta(t))).y;
        orientacion=orientacion*glm::angleAxis(glm::radians(std::clamp(giro*45.f,-23.f,23.f)),glm::vec3(0,0,1));
        vuelo->rotacion=glm::degrees(glm::eulerAngles(glm::slerp(glm::quat(1,0,0,0),orientacion,peso)));
        float planeo=std::clamp(float((std::sin(t*1.15+.4)-.72)/.28),0.f,1.f);
        planeo=planeo*planeo*(3-2*planeo);
        const float fase=float(std::fmod(6.28318530718*(4.2*t+.13*std::sin(t*1.7)+.06*std::sin(t*3.1)),6.28318530718));
        const float amplitud=(55.f+4.f*float(std::sin(t*.8)))*(1-.88f*planeo);
        const float angulo=peso*(10+amplitud*(std::sin(fase)+.12f*std::sin(2*fase)));
        izquierda->rotacion=reposoIzq+glm::vec3(0,0,-angulo);
        derecha->rotacion=reposoDer+glm::vec3(0,0,angulo);
        ancla->actualizar(padre);
    }
};
