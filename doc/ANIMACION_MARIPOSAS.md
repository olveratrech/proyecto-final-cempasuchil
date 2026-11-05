# Animación de mariposas por proximidad

Se reutilizan las 12 instancias existentes (3 del portal y 9 del camino), sus modelos glTF, materiales, posiciones y escalas. No cambia la iluminación, los shaders, la cámara ni el altar.

- Activación: cámara a 5 metros o menos del emplazamiento original, en coordenadas de mundo.
- Desactivación: cámara a 10 metros o más. Entre 5 y 10 metros conserva el estado anterior para evitar oscilaciones en el límite.
- Transición: aproximadamente 0.67 segundos, con interpolación suave hacia/desde la postura original. Permanecen visibles cuando están detenidas; su reloj se pausa.
- Vuelo: recorrido local Catmull-Rom, orientación según trayectoria, inclinación al girar, aleteo variable y planeos breves. Cada ejemplar tiene un desfase independiente.
- Articulación: se usan `giro izquierda` y `giro derecha` del glTF, con rotación sobre Z. El modelo glTF apunta hacia -Z, a diferencia del OBJ de la demostración.

Los radios se ajustan con `encenderMetros` y `apagarMetros` en `src/scene/AnimacionMariposas.h`. `Escena` registra solo las instancias de mariposa del escenario; `App` actualiza sus relojes después de mover la cámara y antes de dibujar.

## Verificación

Compilación macOS correcta. Pruebas de activación, histéresis, parada del reloj, reentrada, distancia mundial y conservación de nodos ajenos correctas. Prueba gráfica aislada con los 12 modelos originales y el renderizador del proyecto: 228 llamadas de dibujo, sin errores de OpenGL.

La comprobación completa en macOS pasó: 1488 nodos, 71 modelos y 10521 instancias; error OpenGL = 0. Se retiró el bloqueo basado en un total fijo de 3057 piezas del altar, que había quedado desactualizado. Se conservan las comprobaciones por grupo, apoyos, separaciones y límites del portal, así como la composición. Las carpetas de compilación se eliminaron a petición del usuario; el lanzador las regenera.

Respaldo previo de los tres archivos modificados: `../animacion-emmanuel/respaldos/integracion_mariposas_antes.zip` (ruta relativa a la carpeta del proyecto).
