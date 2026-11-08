#include "scene/Animaciones.h"

#include "Registro.h"
#include "Rutas.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace {
constexpr float kPi = 3.14159265f;
constexpr float kVelocidad = 1.1f;         // m/s al caminar
constexpr float kZancada = 0.95f;          // metros que avanza un ciclo de caminata (clip de 1 s)
constexpr float kRecorrido = 18.0f;        // duración del recorrido de cámara (s)
constexpr float kVuelo = 0.8f;             // m/s de la mariposa guía

// Lugares en coordenadas de mundo (ver la distribución en Escena.h).
const glm::vec3 kMariposa{-0.55f, 0.166f, -12.75f};  // sobre el petate, frente al altar
const glm::vec3 kRemolino{0.0f, 0.0f, -11.2f};       // eje del remolino, sobre el final del camino
const glm::vec3 kJarra{0.78f, 0.162f, -12.72f};      // base del vaso, sobre el petate
const glm::vec3 kOfrenda{0.03f, 0.583f, -13.29f};    // hueco frontal del primer piso, entre las calaveritas
const glm::vec3 kBailarin{2.0f, 0.0f, 18.0f};

float suave(float x) {
    x = std::clamp(x, 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}
float giroHacia(const glm::vec3& d) { return std::atan2(d.x, d.z); }  // los modelos miran a +Z
glm::quat giroY(float a) { return glm::angleAxis(a, glm::vec3(0.0f, 1.0f, 0.0f)); }
float mezclarAngulo(float a, float b, float w) { return a + std::remainder(b - a, 2.0f * kPi) * w; }
}  // namespace

// ---------------------------------------------------------------------------
// Pistas horneadas
// ---------------------------------------------------------------------------
bool Pista::cargar(const std::string& ruta, unsigned componentesEsperados) {
    std::ifstream f(std::filesystem::u8path(ruta), std::ios::binary);
    char magia[4]{};
    unsigned cabecera[5]{};
    f.read(magia, 4);
    f.read(reinterpret_cast<char*>(cabecera), sizeof(cabecera));
    if (!f || std::memcmp(magia, "PIST", 4) != 0 || cabecera[0] != 1 || cabecera[4] != componentesEsperados) {
        registro::error("Pista no válida: %s", ruta.c_str());
        return false;
    }
    fps = cabecera[1]; cuadros = cabecera[2]; elementos = cabecera[3]; componentes = cabecera[4];
    datos.resize(size_t(cuadros) * elementos * componentes);
    f.read(reinterpret_cast<char*>(datos.data()), static_cast<std::streamsize>(datos.size() * sizeof(float)));
    if (!f || datos.empty()) {
        registro::error("Pista incompleta: %s", ruta.c_str());
        return false;
    }
    return true;
}

void Pista::evaluar(float t, unsigned elemento, float* salida) const {
    const float c = std::clamp(t * float(fps), 0.0f, float(cuadros - 1));
    const unsigned c0 = unsigned(c), c1 = std::min(c0 + 1, cuadros - 1);
    const float f = c - float(c0);
    const float* a = &datos[(size_t(c0) * elementos + elemento) * componentes];
    const float* b = &datos[(size_t(c1) * elementos + elemento) * componentes];
    for (unsigned k = 0; k < componentes; ++k) salida[k] = a[k] + (b[k] - a[k]) * f;
}

// ---------------------------------------------------------------------------
// Personajes con huesos
// ---------------------------------------------------------------------------
void Visitante::caminar(int clip, const std::vector<glm::vec3>& puntos) {
    Tramo t;
    t.clip = clip;
    t.ruta.construir(puntos);
    t.duracion = t.ruta.longitud() / kVelocidad;
    t.lugar = puntos.back();
    t.giro = giroHacia(t.ruta.tangente(t.ruta.longitud()));
    tramos.push_back(std::move(t));
}

void Visitante::quieto(int clip, float giro, float duracion, bool ciclo) {
    Tramo t;
    t.clip = clip;
    t.giro = giro;
    t.lugar = tramos.empty() ? nodo->posicion : tramos.back().lugar;
    t.duracion = duracion >= 0.0f ? duracion : modelo->clips[size_t(clip)].duracion;
    t.ciclo = ciclo;
    tramos.push_back(std::move(t));
}

void Visitante::actualizar(float dt) {
    reloj += dt;
    if (tiempo < 0.0f || tramos.empty()) {
        const float d = modelo->clips[size_t(espera)].duracion;
        modelo->posar(espera, esperaCiclo ? std::fmod(reloj, d) : 0.0f);
        return;
    }
    tiempo += dt;
    float t = tiempo;
    size_t i = 0;
    while (i + 1 < tramos.size() && t > tramos[i].duracion) t -= tramos[i++].duracion;
    const Tramo& actual = tramos[i];
    auto tiempoClip = [&](const Tramo& tr, float local) {
        const float d = modelo->clips[size_t(tr.clip)].duracion;
        // En las caminatas el ciclo avanza con la distancia: los pies no patinan.
        if (tr.ruta.longitud() > 0.0f) return std::fmod(local * kVelocidad / kZancada, 1.0f) * d;
        return tr.ciclo ? std::fmod(local, d) : std::min(local, d);
    };

    const Tramo* previo = i > 0 ? &tramos[i - 1] : nullptr;
    float rumbo = actual.giro;
    nodo->posicion = actual.lugar;
    if (actual.ruta.longitud() > 0.0f) {
        const float s = std::min(t * kVelocidad, actual.ruta.longitud());
        nodo->posicion = actual.ruta.punto(s);
        rumbo = giroHacia(actual.ruta.tangente(s));
    }
    if (previo) rumbo = mezclarAngulo(previo->giro, rumbo, suave(t / 0.5f));
    nodo->orientacion = giroY(rumbo);

    // Medio cuarto de segundo de mezcla entre clips evita saltos de pose.
    if (previo && t < 0.25f)
        modelo->posar(previo->clip, tiempoClip(*previo, previo->duracion), actual.clip, tiempoClip(actual, t), suave(t / 0.25f));
    else
        modelo->posar(actual.clip, tiempoClip(actual, t));
}

// ---------------------------------------------------------------------------
// Construcción
// ---------------------------------------------------------------------------
void Animaciones::cargar(Escena& escena) {
    escena_ = &escena;
    grupo_ = escena.raiz()->agregar("Animaciones");
    const glm::vec3 y(0.0f, 1.0f, 0.0f);

    // 1. Mariposa guía: misma jerarquía cuerpo-alas que las mariposas de la escena.
    mariposa_ = escena.instanciar("mariposa_monarca", grupo_, "Mariposa guía", kMariposa, 0.0f, 1.5f, false);
    alaIzq_ = mariposa_->buscar("giro izquierda");
    alaDer_ = mariposa_->buscar("giro derecha");
    if (alaIzq_ && alaDer_) {
        reposoIzq_ = alaIzq_->rotacion;
        reposoDer_ = alaDer_->rotacion;
    }
    mariposa_->usarOrientacion = true;
    mariposa_->orientacion = giroY(kPi);  // de frente al camino
    vuelo_.construir({kMariposa, kMariposa + glm::vec3(0.10f, 0.35f, 0.30f), {0.9f, 1.65f, -12.4f},
                      {1.7f, 1.35f, -11.0f}, {0.5f, 1.95f, -9.8f}, {-1.3f, 1.55f, -10.6f},
                      {-1.6f, 1.95f, -12.2f}, {-0.9f, 1.25f, -12.6f}, kMariposa + glm::vec3(-0.05f, 0.25f, 0.30f),
                      kMariposa});

    // 2. Recorrido: claves de posición y de mira (panteón -> portada -> camino -> altar).
    // La tercera clave cruza la portada (z = 16) por encima de la reja cerrada, que mide 2.18 m.
    const Camino& c = escena.camino;
    recorridoPos_.construir({{7.0f, 5.0f, 35.0f}, {1.2f, 2.3f, 25.5f}, c.punto(3.0f) + y * 2.9f,
                             c.punto(11.0f) + y * 1.7f + c.izquierda(11.0f) * 0.4f, c.punto(25.0f) + y * 1.6f,
                             {0.5f, 1.6f, -10.0f}, {1.0f, 1.35f, -12.0f}});
    recorridoMira_.construir({{0.0f, 1.6f, 18.0f}, {0.0f, 2.0f, 15.0f}, c.punto(9.0f) + y * 1.3f,
                              c.punto(19.0f) + y * 1.1f, {0.0f, 1.2f, -15.0f}, {0.0f, 1.15f, -15.2f},
                              {0.0f, 0.95f, -15.1f}});

    // 5. Remolino: un lote de 720 pétalos cuyas matrices se reescriben cada cuadro.
    if (remolino_.cargar(rutas::asset("animaciones/remolino_petalos.pist"), 8)) {
        petalos_ = grupo_->agregar("Remolino de pétalos");
        petalos_->posicion = kRemolino;
        lotePetalos_ = std::make_unique<Lote>();
        lotePetalos_->modelo = escena.modelo("petalo_cempasuchil");
        for (unsigned i = 0; i < remolino_.elementos; ++i) lotePetalos_->agregar(glm::mat4(1.0f));
        lotePetalos_->subir();
        petalos_->lote = lotePetalos_.get();
        colocarPetalos();
    }

    armarJarra(escena, grupo_);
    armarVisitantes(escena, grupo_);
    registro::info("ANIMACIONES: 1 mariposa, 2 recorrido, 3 veladoras, 4 papel, 5 pétalos, 6 jarra, "
                   "7 visitante 1, 8 visitante 2, 9 esqueleto, L faroles");
}

void Animaciones::armarJarra(Escena& escena, Nodo* grupo) {
    Pista interior;
    if (!jarra_.cargar(rutas::asset("animaciones/jarra_sirviendo.pist"), 13) ||
        !chorro_.cargar(rutas::asset("animaciones/jarra_chorro.pist"), 4) ||
        !interior.cargar(rutas::asset("animaciones/jarra_interior.pist"), 2))
        return;
    grupoJarra_ = grupo->agregar("Jarra sirviendo agua");
    grupoJarra_->posicion = kJarra;
    Nodo* vaso = escena.instanciar("vaso_agua", grupoJarra_, "Vaso", glm::vec3(0.0f));
    jarraNodo_ = escena.instanciar("jarra_agua", grupoJarra_, "Jarra", glm::vec3(0.0f));
    jarraNodo_->usarOrientacion = true;
    aguaVaso_ = vaso->buscar("agua");
    if (aguaVaso_) baseAguaVaso_ = aguaVaso_->modelo->mallas[size_t(aguaVaso_->malla)].primitivas[0].min.y;

    Material agua;
    for (const Material& m : escena.modelo("jarra_agua")->materiales)
        if (m.nombre.find("agua") != std::string::npos) agua = m;
    agua.dobleCara = true;  // tras el recorte se ve el interior del volumen
    if (Nodo* original = jarraNodo_->buscar("agua")) original->visible = false;

    // Volumen interior de la jarra por revolución del perfil exportado (altura, radio).
    std::vector<Vertice> vs;
    std::vector<uint32_t> idx;
    constexpr int lados = 48;
    for (unsigned a = 0; a < interior.elementos; ++a)
        for (int k = 0; k < lados; ++k) {
            const float t = 2.0f * kPi * float(k) / lados;
            const float h = interior.datos[a * 2], r = interior.datos[a * 2 + 1];
            vs.push_back({{r * std::cos(t), h, -r * std::sin(t)}, {std::cos(t), 0.0f, -std::sin(t)}, {0.0f, 0.0f}});
        }
    for (uint32_t a = 0; a + 1 < interior.elementos; ++a)
        for (uint32_t k = 0; k < lados; ++k) {
            const uint32_t i0 = a * lados + k, i1 = a * lados + (k + 1) % lados;
            idx.insert(idx.end(), {i0, i1, i1 + lados, i0, i1 + lados, i0 + lados});
        }
    for (int tapa = 0; tapa < 2; ++tapa) {
        const uint32_t anillo = tapa ? (interior.elementos - 1) * lados : 0;
        const uint32_t centro = uint32_t(vs.size());
        vs.push_back({{0.0f, interior.datos[tapa ? (interior.elementos - 1) * 2 : 0], 0.0f}, {0.0f, tapa ? 1.0f : -1.0f, 0.0f}, {0.0f, 0.0f}});
        for (uint32_t k = 0; k < lados; ++k) idx.insert(idx.end(), {centro, anillo + (k + 1) % lados, anillo + k});
    }
    modeloAguaJarra_ = Modelo::procedural("agua_interior_jarra", vs, idx, agua);
    aguaJarra_ = jarraNodo_->agregar("Agua interior");
    aguaJarra_->modelo = modeloAguaJarra_.get();
    aguaJarra_->malla = 0;
    aguaJarra_->recortar = true;

    // Chorro: tubo de anillos fijos; cada cuadro se recoloca sobre la polilínea exportada.
    vs.assign(size_t(chorro_.elementos) * 8, Vertice{});
    idx.clear();
    for (uint32_t a = 0; a + 1 < chorro_.elementos; ++a)
        for (uint32_t k = 0; k < 8; ++k) {
            const uint32_t i0 = a * 8 + k, i1 = a * 8 + (k + 1) % 8;
            idx.insert(idx.end(), {i0, i1 + 8, i1, i0, i0 + 8, i1 + 8});
        }
    modeloChorro_ = Modelo::procedural("chorro_agua", vs, idx, agua);
    Primitiva& p = modeloChorro_->mallas[0].primitivas[0];
    p.min = glm::vec3(-0.1f, -0.1f, -0.1f);  // caja fija: la curva no sale de este volumen
    p.max = glm::vec3(0.4f, 0.4f, 0.1f);
    chorroNodo_ = grupoJarra_->agregar("Chorro");
    chorroNodo_->modelo = modeloChorro_.get();
    chorroNodo_->malla = 0;
    actualizarJarra();
}

void Animaciones::armarVisitantes(Escena& escena, Nodo* grupo) {
    auto cargar = [&](Visitante& v, const char* archivo, const char* nombre, const glm::vec3& lugar, float giro) {
        v.modelo = std::make_unique<Modelo>();
        if (!v.modelo->cargar(rutas::asset(std::string("models/animados/") + archivo)) || v.modelo->clips.empty()) {
            registro::error("No se pudo cargar el personaje %s", archivo);
            v.modelo.reset();
            return false;
        }
        v.nodo = grupo->agregar(nombre);
        v.nodo->modelo = v.modelo.get();
        v.nodo->malla = 0;
        v.nodo->posicion = lugar;
        v.nodo->usarOrientacion = true;
        v.nodo->orientacion = giroY(giro);
        v.espera = v.modelo->clip("reposo");
        return true;
    };
    const Camino& c = escena.camino;

    // 7. Visitante 1: entra por la portada, sigue el camino y deja pan de muerto en el altar.
    if (cargar(visitante1_, "visitante_panteonero.glb", "Visitante 1", c.punto(3.6f), kPi)) {
        Modelo& m = *visitante1_.modelo;
        const int depositar = m.clip("depositar");
        anclaOfrenda_ = m.nodo("ancla_ofrenda");
        m.posar(depositar, m.clips[size_t(depositar)].duracion);
        // Mirando a -Z, el ancla queda en (-x, y, -z) respecto a la raíz del personaje.
        const glm::vec3 a = glm::vec3(m.globales[size_t(anclaOfrenda_)][3]);
        const glm::vec3 parada{kOfrenda.x + a.x, 0.15f, kOfrenda.z + a.z};
        std::vector<glm::vec3> ruta;
        for (float s = 3.6f; s < c.longitud() - 0.5f; s += 2.0f) ruta.push_back(c.punto(s));
        ruta.insert(ruta.end(), {{parada.x, 0.0f, -11.9f}, {parada.x, 0.15f, -12.45f}, parada});
        visitante1_.espera = m.clip("caminar_cargando");
        visitante1_.esperaCiclo = false;  // espera de pie con la ofrenda en brazos
        visitante1_.caminar(m.clip("caminar_cargando"), ruta);
        visitante1_.quieto(depositar, kPi);
        sueltaOfrenda_ = visitante1_.tramos[0].duracion + visitante1_.tramos[1].duracion;
        visitante1_.quieto(m.clip("incorporarse"), kPi);
        visitante1_.quieto(m.clip("reverencia"), kPi);
        visitante1_.quieto(m.clip("reposo"), kPi, 1e9f, true);
        ofrenda_ = escena.instanciar("pan_de_muerto", grupo, "Ofrenda del visitante", glm::vec3(0.0f), 0.0f, 1.2f);
        ofrenda_->usarOrientacion = true;
    }

    // 8. Visitante 2: recorre el panteón, observa una tumba, saluda y baila.
    if (cargar(visitante2_, "visitante_calaca.glb", "Visitante 2", {-0.5f, 0.0f, 30.5f}, kPi)) {
        Modelo& m = *visitante2_.modelo;
        const glm::vec3 tumba{-2.5f, 0.0f, 24.4f};
        visitante2_.caminar(m.clip("caminar"), {{-0.5f, 0.0f, 30.5f}, {-0.9f, 0.0f, 27.6f}, tumba});
        visitante2_.quieto(m.clip("reposo"), -kPi / 2, 0.6f);
        visitante2_.quieto(m.clip("observar"), -kPi / 2);
        const float alCamino = giroHacia(glm::vec3(0.0f, 0.0f, 16.0f) - tumba);
        visitante2_.quieto(m.clip("reposo"), alCamino, 0.6f);
        visitante2_.quieto(m.clip("saludar"), alCamino);
        visitante2_.quieto(m.clip("bailar_corto"), alCamino);
        visitante2_.caminar(m.clip("caminar"), {tumba, {-1.2f, 0.0f, 22.0f}, {0.0f, 0.0f, 20.2f}});
        visitante2_.quieto(m.clip("reposo"), kPi, 1e9f, true);
    }

    // 9. Esqueleto bailando junto a la portada, de frente al camino.
    const float alCamino = giroHacia(glm::vec3(0.0f, 0.0f, 14.0f) - kBailarin);
    if (cargar(bailarin_, "visitante_calaca.glb", "Esqueleto bailando", kBailarin, alCamino)) {
        bailarin_.quieto(bailarin_.modelo->clip("bailar"), alCamino);
        bailarin_.quieto(bailarin_.espera, alCamino, 1e9f, true);
    }
}

void Animaciones::liberar() {
    lotePetalos_.reset();
    modeloAguaJarra_.reset();
    modeloChorro_.reset();
    visitante1_.modelo.reset();
    visitante2_.modelo.reset();
    bailarin_.modelo.reset();
}

// ---------------------------------------------------------------------------
// Entrada
// ---------------------------------------------------------------------------
void Animaciones::tecla(int tecla) {
    switch (tecla) {
        case GLFW_KEY_1:
            if (tiempoMariposa_ < 0.0f) tiempoMariposa_ = 0.0f;
            estado_ = "1 Mariposa: despega, vuela y aterriza";
            break;
        case GLFW_KEY_2:
            recorrido_ = recorrido_ < 0.0f ? 0.0f : -1.0f;
            estado_ = recorrido_ >= 0.0f ? "2 Recorrido de cámara (2 para detener)" : "2 Recorrido detenido";
            break;
        case GLFW_KEY_3:
            velas_ = velas_ == 0 || velas_ == 3 ? 1 : 3;
            tiempoVelas_ = 0.0f;
            estado_ = velas_ == 1 ? "3 Veladoras: apagado gradual" : "3 Veladoras: encendido por grupos";
            break;
        case GLFW_KEY_4:
            viento_ = !viento_;
            estado_ = viento_ ? "4 Papel picado: viento" : "4 Papel picado: calma";
            break;
        case GLFW_KEY_5:
            tiempoRemolino_ = 0.0f;
            remolinoActivo_ = lotePetalos_ != nullptr;
            estado_ = "5 Remolino de pétalos";
            break;
        case GLFW_KEY_6:
            tiempoJarra_ = 0.0f;
            jarraActiva_ = grupoJarra_ != nullptr;
            estado_ = "6 Jarra sirviendo agua";
            break;
        case GLFW_KEY_7:
            visitante1_.tiempo = 0.0f;
            estado_ = "7 Visitante 1: lleva pan de muerto al altar";
            break;
        case GLFW_KEY_8:
            visitante2_.tiempo = 0.0f;
            estado_ = "8 Visitante 2: visita una tumba, saluda y baila";
            break;
        case GLFW_KEY_9:
            bailarin_.tiempo = 0.0f;
            estado_ = "9 Esqueleto bailando";
            break;
        case GLFW_KEY_L:
            farolesEncendidos_ = !farolesEncendidos_;
            estado_ = farolesEncendidos_ ? "L Faroles encendidos" : "L Faroles apagados";
            break;
        default:
            return;
    }
    registro::info("Animación: %s", estado_.c_str());
}

// ---------------------------------------------------------------------------
// Actualización por cuadro
// ---------------------------------------------------------------------------
void Animaciones::actualizarJarra() {
    float v[13];
    jarra_.evaluar(tiempoJarra_, 0, v);
    jarraNodo_->posicion = {v[0], v[1], v[2]};
    jarraNodo_->orientacion = glm::normalize(glm::quat(v[6], v[3], v[4], v[5]));
    // La superficie del agua es horizontal: plano y = base del vaso + nivel exportado.
    aguaJarra_->planoCorte = {0.0f, 1.0f, 0.0f, -(kJarra.y + v[7])};
    if (aguaVaso_) {
        aguaVaso_->visible = v[9] > 0.5f;
        aguaVaso_->escala.y = v[8];
        aguaVaso_->posicion.y = baseAguaVaso_ * (1.0f - v[8]);
    }
    chorroNodo_->visible = v[12] > 0.5f && v[11] > v[10];
    if (!chorroNodo_->visible) return;

    // Puntos de la polilínea y su longitud acumulada.
    const unsigned n = chorro_.elementos;
    std::vector<float> s(n, 0.0f);
    auto punto = [&](unsigned i) { return glm::vec3(chorro_.datos[i * 4], chorro_.datos[i * 4 + 1], chorro_.datos[i * 4 + 2]); };
    for (unsigned i = 1; i < n; ++i) s[i] = s[i - 1] + glm::length(punto(i) - punto(i - 1));
    auto muestra = [&](float u, glm::vec3& p, float& r) {
        const float d = u * s[n - 1];
        unsigned i = 1;
        while (i + 1 < n && s[i] < d) ++i;
        const float f = std::clamp((d - s[i - 1]) / std::max(s[i] - s[i - 1], 1e-6f), 0.0f, 1.0f);
        p = glm::mix(punto(i - 1), punto(i), f);
        r = glm::mix(chorro_.datos[(i - 1) * 4 + 3], chorro_.datos[i * 4 + 3], f);
    };
    Primitiva& prim = modeloChorro_->mallas[0].primitivas[0];
    std::vector<Vertice> vs(size_t(n) * 8);
    for (unsigned a = 0; a < n; ++a) {
        glm::vec3 p, q;
        float r, rq;
        const float u = v[10] + (v[11] - v[10]) * float(a) / float(n - 1);
        muestra(u, p, r);
        muestra(std::min(u + 0.01f, 1.0f), q, rq);
        const glm::vec3 t = glm::length(q - p) > 1e-6f ? glm::normalize(q - p) : glm::vec3(0.0f, -1.0f, 0.0f);
        const glm::vec3 lado = glm::normalize(glm::cross(t, glm::vec3(0.0f, 0.0f, 1.0f)));
        const glm::vec3 arriba = glm::cross(lado, t);
        for (int k = 0; k < 8; ++k) {
            const float ang = 2.0f * kPi * float(k) / 8.0f;
            const glm::vec3 nrm = std::cos(ang) * lado + std::sin(ang) * arriba;
            vs[a * 8 + k] = {p + nrm * r, nrm, {0.0f, 0.0f}};
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, prim.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(vs.size() * sizeof(Vertice)), vs.data());
}

void Animaciones::colocarPetalos() {
    float e[8];
    for (unsigned i = 0; i < remolino_.elementos; ++i) {
        remolino_.evaluar(tiempoRemolino_, i, e);
        lotePetalos_->transformaciones[i] = glm::translate(glm::mat4(1.0f), glm::vec3(e[0], e[1], e[2])) *
            glm::mat4_cast(glm::normalize(glm::quat(e[6], e[3], e[4], e[5]))) * glm::scale(glm::mat4(1.0f), glm::vec3(e[7]));
    }
    lotePetalos_->actualizarTransformaciones();
}

void Animaciones::actualizar(float dt, Camara& camara, Renderer& renderer) {
    if (!escena_) return;
    dt = std::clamp(dt, 0.0f, 0.1f);
    reloj_ += dt;

    // 1. Mariposa guía: reposo -> despegue -> vuelo -> aterrizaje -> reposo.
    if (mariposa_ && alaIzq_ && alaDer_) {
        relojAlas_ += dt;
        const float reposo = 18.0f + 16.0f * std::sin(2.0f * kPi * 0.6f * relojAlas_);
        float angulo = reposo;
        if (tiempoMariposa_ >= 0.0f) {
            tiempoMariposa_ += dt;
            const float L = vuelo_.longitud(), s = std::min(tiempoMariposa_ * kVuelo, L);
            // Despegue y aterrizaje: el aleteo crece y se apaga en los primeros y últimos 0.6 m.
            const float vigor = suave(s / 0.6f) * suave((L - s) / 0.6f);
            angulo = glm::mix(reposo, 10.0f + 55.0f * std::sin(2.0f * kPi * 4.2f * relojAlas_), vigor);
            mariposa_->posicion = vuelo_.punto(s);
            const glm::vec3 d = vuelo_.punto(s + 0.05f) - vuelo_.punto(s - 0.05f);
            if (vigor > 0.05f && glm::length(glm::vec3(d.x, 0.0f, d.z)) > 1e-4f) {
                const glm::vec3 frente = glm::normalize(d);
                const glm::vec3 lado = glm::normalize(glm::cross(frente, glm::vec3(0.0f, 1.0f, 0.0f)));
                // glTF de la mariposa: cabeza hacia -Z, vertical +Y.
                mariposa_->orientacion = glm::quat_cast(glm::mat3(lado, glm::cross(lado, frente), -frente));
            }
            if (s >= L) {
                tiempoMariposa_ = -1.0f;
                mariposa_->orientacion = giroY(kPi);
            }
        }
        alaIzq_->rotacion = reposoIzq_ + glm::vec3(0.0f, 0.0f, -angulo);
        alaDer_->rotacion = reposoDer_ + glm::vec3(0.0f, 0.0f, angulo);
    }

    // 2. Recorrido de cámara con arranque y llegada suaves.
    if (recorrido_ >= 0.0f) {
        recorrido_ += dt;
        const float f = suave(recorrido_ / kRecorrido);
        camara.establecerVuelo(true);
        camara.posicion = recorridoPos_.punto(f * recorridoPos_.longitud());
        camara.mirarHacia(recorridoMira_.punto(f * recorridoMira_.longitud()));
        if (recorrido_ >= kRecorrido) recorrido_ = -1.0f;
    }

    // 3. Veladoras: el frente de encendido viaja del panteón (z = 26) al altar (z = -19).
    tiempoVelas_ += dt;
    if (velas_ == 1) {
        nivelVelas_ = 1.0f - suave(tiempoVelas_ / 2.5f);
        if (tiempoVelas_ >= 2.5f) velas_ = 2;
    } else if (velas_ == 3) {
        nivelVelas_ = 1.0f;
        frenteVelas_ = 26.0f - 45.0f * tiempoVelas_ / 6.0f;
        if (tiempoVelas_ >= 6.5f) { velas_ = 0; frenteVelas_ = -1e4f; }
    }
    faroles_ = std::clamp(faroles_ + (farolesEncendidos_ ? dt : -dt), 0.0f, 1.0f);
    renderer.velas = {reloj_, frenteVelas_, nivelVelas_, suave(faroles_)};

    // 4. Papel picado: el viento arranca y se calma en poco más de un segundo.
    escena_->velocidadPapel = std::clamp(escena_->velocidadPapel + (viento_ ? dt : -dt) * 0.8f, 0.0f, 1.0f);

    // 5. Remolino de pétalos.
    if (remolinoActivo_) {
        tiempoRemolino_ = std::min(tiempoRemolino_ + dt, remolino_.duracion());
        colocarPetalos();
        remolinoActivo_ = tiempoRemolino_ < remolino_.duracion();
    }

    // 6. Jarra sirviendo agua.
    if (jarraActiva_) {
        tiempoJarra_ = std::min(tiempoJarra_ + dt, jarra_.duracion());
        actualizarJarra();
        jarraActiva_ = tiempoJarra_ < jarra_.duracion();
    }

    // 7, 8 y 9. Personajes.
    for (Visitante* v : {&visitante1_, &visitante2_, &bailarin_})
        if (v->modelo) v->actualizar(dt);
    if (ofrenda_ && visitante1_.modelo && visitante1_.tiempo < sueltaOfrenda_) {
        // El pan sigue al ancla del antebrazo hasta que el personaje lo suelta.
        const glm::mat4 m = glm::translate(glm::mat4(1.0f), visitante1_.nodo->posicion) *
            glm::mat4_cast(visitante1_.nodo->orientacion) * visitante1_.modelo->globales[size_t(anclaOfrenda_)];
        ofrenda_->posicion = glm::vec3(m[3]);
        ofrenda_->orientacion = glm::quat_cast(glm::mat3(glm::normalize(glm::vec3(m[0])),
                                                         glm::normalize(glm::vec3(m[1])), glm::normalize(glm::vec3(m[2]))));
    }

    grupo_->actualizar(glm::mat4(1.0f));
}
