// Camera.h - Cámara en primera persona con modo de caminata y vuelo.
#pragma once

#include <glm/glm.hpp>

class Camara {
public:
    // --- primera persona ------------------------------------------------------
    glm::vec3 posicion{0.0f, 1.3f, 10.0f};
    float guinada = -90.0f;  // yaw (grados): -90 mira hacia -Z
    float cabeceo = 0.0f;    // pitch (grados)
    float velocidad = 4.0f;  // m/s
    float sensibilidad = 0.12f;
    float alturaOjos = 1.30f;
    bool vuelo = false;

    // --- proyección -------------------------------------------------------------
    float fov = 60.0f;
    float cerca = 0.05f;
    float lejos = 400.0f;


    glm::vec3 frente() const;
    glm::vec3 derecha() const;
    glm::vec3 ojo() const;  // posición de la cámara
    glm::mat4 vista() const;
    glm::mat4 proyeccion(float aspecto) const;

    void mirarHacia(const glm::vec3& punto);
    void girar(float dx, float dy);
    void desplazar(const glm::vec3& direccionLocal, float dt, float multiplicador);
    void actualizarPaso(bool caminando, float dt, float multiplicador);
    void establecerVuelo(bool activo);
    void acercar(float pasos);

private:
    float fasePaso_ = 0.0f;
    float balanceo_ = 0.0f;
};
