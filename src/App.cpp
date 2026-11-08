#include "App.h"

#include "Registro.h"
#include "Rutas.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <cmath>
#include <functional>
#include <vector>


static App* g_app = nullptr;

int App::ejecutar(const Opciones& opciones) {
    op_ = opciones;
    if (!iniciar()) {
        terminar();
        return 1;
    }

    if (op_.diagnosticarRendimiento) {
        const int codigo = ejecutarDiagnostico();
        terminar();
        return codigo;
    }

    int cuadros=0;
    double anterior = glfwGetTime();
    while (!glfwWindowShouldClose(ventana_)) {
        double ahora = glfwGetTime();
        float dt = static_cast<float>(std::min(ahora - anterior, 0.1));
        anterior = ahora;
        glfwPollEvents();
        glfwGetFramebufferSize(ventana_, &anchoFb_, &altoFb_);
        if (anchoFb_ == 0 || altoFb_ == 0) {  // ventana minimizada
            glfwWaitEvents();
            continue;
        }
        procesarEntrada(dt);
        escena_.actualizarMariposas(dt, camara_.ojo());
        animaciones_.actualizar(dt, camara_, renderer_);

        renderer_.dibujar(escena_.raiz(), camara_, anchoFb_, altoFb_, &escena_.cielo);

        glfwSwapBuffers(ventana_);
        if(op_.comprobar && ++cuadros==5) {
            const GLenum error=glGetError();
            registro::info("COMPROBACION GL: vista=%d, llamadas=%d, triangulos=%d, error=%u",op_.vistaInicial,renderer_.stats.llamadas,renderer_.stats.triangulos,error);
            terminar();return error==GL_NO_ERROR?0:2;
        }
    }
    terminar();
    return 0;
}

int App::ejecutarDiagnostico() {
    constexpr int calentamiento = 12;
    constexpr int muestras = 60;
    auto medir = [&](const char* nombre, bool luces) {
        renderer_.lucesPuntuales = luces;
        double totalMs = 0.0;
        for (int i = -calentamiento; i < muestras; ++i) {
            const auto inicio = std::chrono::steady_clock::now();
            renderer_.dibujar(escena_.raiz(), camara_, anchoFb_, altoFb_, &escena_.cielo);
            glFinish();
            const auto fin = std::chrono::steady_clock::now();
            if (i >= 0)
                totalMs += std::chrono::duration<double, std::milli>(fin - inicio).count();
        }
        const double promedio = totalMs / muestras;
        registro::info("DIAGNOSTICO %s: %.3f ms/cuadro (%.1f FPS), luces=%d, llamadas=%d, triangulos=%d, instancias=%d",
                       nombre, promedio, 1000.0 / promedio, renderer_.stats.luces,
                       renderer_.stats.llamadas, renderer_.stats.triangulos,
                       renderer_.stats.instancias);
        return promedio;
    };

    glfwGetFramebufferSize(ventana_, &anchoFb_, &altoFb_);
    registro::info("DIAGNOSTICO CONFIG: framebuffer=%dx%d, filtro=%d, realce=%d", anchoFb_, altoFb_, renderer_.filtrarLuces, renderer_.realzarAltar);
    struct CostoLote { std::string nombre; int instancias; long long triangulos; };
    std::vector<CostoLote> costos;
    std::function<void(const Nodo*)> recorrer = [&](const Nodo* nodo) {
        if (nodo->visible && nodo->lote && nodo->lote->modelo) {
            const int instancias = nodo->lote->instancias();
            costos.push_back({nodo->nombre, instancias,
                              static_cast<long long>(nodo->lote->modelo->triangulos) * instancias});
        }
        if (nodo->visible)
            for (const auto& hijo : nodo->hijos) recorrer(hijo.get());
    };
    recorrer(escena_.raiz());
    std::sort(costos.begin(), costos.end(),
              [](const CostoLote& a, const CostoLote& b) { return a.triangulos > b.triangulos; });
    for (size_t i = 0; i < std::min<size_t>(costos.size(), 10); ++i)
        registro::info("DIAGNOSTICO LOTE #%zu: %s, instancias=%d, triangulos_potenciales=%lld",
                       i + 1, costos[i].nombre.c_str(), costos[i].instancias, costos[i].triangulos);
    const double conLuces = medir("CON_LUCES_PUNTUALES", true);
    registro::info("DIAGNOSTICO LISTAS: promedio %.2f luces por dibujo", float(renderer_.stats.lucesPorDibujo)/std::max(renderer_.stats.llamadas,1));
    if (!op_.captura.empty()) {
        std::vector<unsigned char> pixeles(static_cast<size_t>(anchoFb_)*altoFb_*3);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadBuffer(GL_BACK);
        glReadPixels(0,0,anchoFb_,altoFb_,GL_RGB,GL_UNSIGNED_BYTE,pixeles.data());
        std::ofstream salida(op_.captura,std::ios::binary);
        salida << "P6\n" << anchoFb_ << " " << altoFb_ << "\n255\n";
        for (int y=altoFb_-1;y>=0;--y)
            salida.write(reinterpret_cast<const char*>(pixeles.data()+static_cast<size_t>(y)*anchoFb_*3),anchoFb_*3);
    }
    const double sinLuces = medir("SIN_LUCES_PUNTUALES", false);
    glDisable(GL_MULTISAMPLE);
    const double sinMsaa = medir("CON_LUCES_SIN_MSAA", true);
    if (op_.msaa > 0) glEnable(GL_MULTISAMPLE);
    const double costo = conLuces - sinLuces;
    registro::info("DIAGNOSTICO RESULTADO: costo_luces=%.3f ms (%.1f%% del cuadro con luces)",
                   costo, conLuces > 0.0 ? 100.0 * costo / conLuces : 0.0);
    registro::info("DIAGNOSTICO RESULTADO: costo_msaa=%.3f ms (%.1f%% del cuadro con MSAA)",
                   conLuces - sinMsaa, conLuces > 0.0 ? 100.0 * (conLuces - sinMsaa) / conLuces : 0.0);
    return glGetError() == GL_NO_ERROR ? 0 : 2;
}

bool App::iniciar() {
    g_app = this;
    glfwSetErrorCallback([](int codigo, const char* desc) { registro::error("GLFW %d: %s", codigo, desc); });
    if (!glfwInit()) {
        registro::fatal("No se pudo iniciar GLFW.");
        return false;
    }
    if(op_.comprobar || op_.diagnosticarRendimiento)glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    if (op_.msaa > 0) glfwWindowHint(GLFW_SAMPLES, op_.msaa);
    if (op_.ancho <= 0 || op_.alto <= 0) {
        // tamaño inicial: 80 % del área de trabajo del monitor principal
        int x = 0, y = 0, w = 1280, h = 720;
        if (GLFWmonitor* mon = glfwGetPrimaryMonitor()) glfwGetMonitorWorkarea(mon, &x, &y, &w, &h);
        op_.ancho = std::max(960, w * 4 / 5);
        op_.alto = std::max(540, h * 4 / 5);
    }
    ventana_ = glfwCreateWindow(op_.ancho, op_.alto, "Cempasúchil | Proyecto final",
                               nullptr, nullptr);
    if (!ventana_) {
        registro::fatal("No se pudo crear la ventana. Se requiere una tarjeta gráfica compatible con OpenGL 3.3.");
        return false;
    }
    glfwMakeContextCurrent(ventana_);
    glfwSwapInterval(op_.diagnosticarRendimiento ? 0 : 1);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        registro::fatal("No se pudieron cargar las funciones de OpenGL.");
        return false;
    }
    registro::info("OpenGL %s | GLSL %s | %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
                   glGetString(GL_RENDERER));
    if (op_.msaa > 0) glEnable(GL_MULTISAMPLE);

    renderer_.filtrarLuces = op_.filtrarLuces;
    renderer_.realzarAltar = op_.realzarAltar;
    if (!renderer_.iniciar()) {
        registro::fatal("No se pudieron compilar los shaders (carpeta shaders/).");
        return false;
    }
    if (!escena_.cargar()) {
        registro::fatal("No se pudieron cargar los modelos (carpeta assets/).");
        return false;
    }
    animaciones_.cargar(escena_);
    glfwSetKeyCallback(ventana_, alTeclear);
    glfwSetScrollCallback(ventana_, alDesplazar);
    aplicarVista(op_.vistaInicial);
    return true;
}

void App::terminar() {
    animaciones_.liberar();
    escena_.liberar();
    renderer_.liberar();
    if (ventana_) glfwDestroyWindow(ventana_);
    ventana_ = nullptr;
    glfwTerminate();
}

// ---------------------------------------------------------------------------
// Entrada
// ---------------------------------------------------------------------------
// Navegación FPS: caminar sobre el suelo o vuelo libre activado con F.
void App::procesarEntrada(float dt) {
    double x, y;
    glfwGetCursorPos(ventana_, &x, &y);
    float dx = float(x - ultimoX_), dy = float(y - ultimoY_);
    ultimoX_ = x; ultimoY_ = y;
    if (animaciones_.recorridoActivo()) return;  // el recorrido conduce la cámara
    if (ratonActivo_) camara_.girar(dx, dy);
    auto tecla = [&](int k) { return glfwGetKey(ventana_, k) == GLFW_PRESS; };
    glm::vec3 d(0.0f);
    if (tecla(GLFW_KEY_W)) d.z += 1;
    if (tecla(GLFW_KEY_S)) d.z -= 1;
    if (tecla(GLFW_KEY_D)) d.x += 1;
    if (tecla(GLFW_KEY_A)) d.x -= 1;
    if (camara_.vuelo && tecla(GLFW_KEY_E)) d.y += 1;
    if (camara_.vuelo && tecla(GLFW_KEY_Q)) d.y -= 1;
    const bool caminando = d.x != 0.0f || d.z != 0.0f;
    const float multiplicador = tecla(GLFW_KEY_LEFT_SHIFT) ? 3.0f : 1.0f;
    if (d != glm::vec3(0)) camara_.desplazar(glm::normalize(d), dt, multiplicador);
    camara_.actualizarPaso(caminando, dt, multiplicador);
}

void App::alTeclear(GLFWwindow* v, int tecla, int codigo, int accion, int mods) {
    if (accion != GLFW_PRESS) return;
    App& a = *g_app;
    switch (tecla) {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(v, GLFW_TRUE); break;
        case GLFW_KEY_TAB:
            a.ratonActivo_ = !a.ratonActivo_;
            glfwSetInputMode(v, GLFW_CURSOR, a.ratonActivo_ ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(v, &a.ultimoX_, &a.ultimoY_);
            registro::info("Control de cámara con ratón: %s", a.ratonActivo_ ? "activo" : "inactivo");
            break;
        case GLFW_KEY_F2: a.renderer_.alambre = !a.renderer_.alambre; break;
        case GLFW_KEY_F:
            a.camara_.establecerVuelo(!a.camara_.vuelo);
            registro::info("Modo de navegación: %s", a.camara_.vuelo ? "vuelo" : "caminata");
            break;
        case GLFW_KEY_R: a.aplicarVista(1); break;
        default:
            a.animaciones_.tecla(tecla);
            if (!a.animaciones_.estado().empty())
                glfwSetWindowTitle(v, ("Cempasúchil | " + a.animaciones_.estado()).c_str());
            break;
    }
}

void App::alDesplazar(GLFWwindow* v, double dx, double dy) {
    g_app->camara_.acercar(float(dy));
}

void App::aplicarVista(int indice) {
    const auto& vistas = escena_.vistas();
    if (indice < 0 || indice >= static_cast<int>(vistas.size())) return;
    camara_.posicion = vistas[static_cast<size_t>(indice)].posicion;
    camara_.mirarHacia(vistas[static_cast<size_t>(indice)].objetivo);
    camara_.establecerVuelo(indice != 1 && camara_.posicion.y > 2.5f);
}
