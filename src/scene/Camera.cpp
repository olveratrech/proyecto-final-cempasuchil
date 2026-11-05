#include "scene/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

glm::vec3 Camara::frente() const {
    float y = glm::radians(guinada), p = glm::radians(cabeceo);
    return glm::normalize(glm::vec3(std::cos(y) * std::cos(p), std::sin(p), std::sin(y) * std::cos(p)));
}

glm::vec3 Camara::derecha() const { return glm::normalize(glm::cross(frente(), glm::vec3(0.0f, 1.0f, 0.0f))); }

glm::vec3 Camara::ojo() const {
    return posicion + glm::vec3(0.0f, vuelo ? 0.0f : balanceo_, 0.0f);
}

glm::mat4 Camara::vista() const {
    glm::vec3 o = ojo();
    return glm::lookAt(o, o + frente(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camara::proyeccion(float aspecto) const { return glm::perspective(glm::radians(fov), aspecto, cerca, lejos); }

void Camara::mirarHacia(const glm::vec3& punto) {
    glm::vec3 d = glm::normalize(punto - posicion);
    cabeceo = glm::degrees(std::asin(std::clamp(d.y, -1.0f, 1.0f)));
    guinada = glm::degrees(std::atan2(d.z, d.x));
}

void Camara::girar(float dx, float dy) {
    guinada += dx * sensibilidad;
    cabeceo = std::clamp(cabeceo - dy * sensibilidad, -89.0f, 89.0f);
}

void Camara::desplazar(const glm::vec3& d, float dt, float multiplicador) {
    float v = velocidad * multiplicador * dt;
    glm::vec3 f = frente();
    if (!vuelo) {
        f.y = 0.0f;
        if (glm::dot(f, f) > 0.0f) f = glm::normalize(f);
    }
    glm::vec3 r = derecha();
    posicion += r * d.x * v + f * d.z * v;
    if (vuelo) posicion += glm::vec3(0.0f, 1.0f, 0.0f) * d.y * v;
    else posicion.y = alturaOjos;
    posicion.y = std::max(posicion.y, 0.25f);
}

void Camara::actualizarPaso(bool caminando, float dt, float multiplicador) {
    if (vuelo) {
        balanceo_ = 0.0f;
        return;
    }
    if (caminando) {
        fasePaso_ += dt * 5.2f * std::sqrt(multiplicador);
        const float objetivo = std::sin(fasePaso_ * 2.0f) * 0.017f +
                               std::sin(fasePaso_) * 0.004f;
        balanceo_ += (objetivo - balanceo_) * std::min(1.0f, dt * 10.0f);
    } else {
        balanceo_ += (0.0f - balanceo_) * std::min(1.0f, dt * 8.0f);
    }
}

void Camara::establecerVuelo(bool activo) {
    vuelo = activo;
    balanceo_ = 0.0f;
    if (!vuelo) posicion.y = alturaOjos;
}

void Camara::acercar(float pasos) {
    velocidad = std::clamp(velocidad * std::pow(1.15f, pasos), 0.5f, 40.0f);
}
