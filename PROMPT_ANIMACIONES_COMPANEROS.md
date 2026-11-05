```text
Estoy trabajando en un proyecto de Computación Gráfica llamado `texturizado-entrega`.
El repositorio ya está clonado en:

[RUTA_ABSOLUTA_A_TEXTURIZADO_ENTREGA]

Necesito que realices únicamente las animaciones número [NUMEROS_ASIGNADOS] de la lista incluida al final de este prompt.

Antes de modificar archivos:

1. Inspecciona el repositorio, su documentación y la carpeta `modelos-blender`.
   Considera la ruta que te proporcioné como `RAIZ_PROYECTO`. Resuelve siempre los recursos desde:
   - `RAIZ_PROYECTO/modelos-blender`: fuentes Blender portables, con imágenes empacadas.
   - `RAIZ_PROYECTO/assets/models/propios`: modelos glTF usados por OpenGL.
   - `RAIZ_PROYECTO/assets/textures`: texturas originales de ejecución.
   - `RAIZ_PROYECTO/src` y `RAIZ_PROYECTO/Shader`: referencia para la implementación OpenGL.
   No busques ni dependas de carpetas externas como `Re-Modelaje`, prácticas del curso o rutas particulares de otra computadora.
2. No integres todavía las animaciones en la escena principal y no modifiques el código, los modelos ni los recursos de `texturizado-entrega`.
3. Crea una carpeta hermana, fuera del repositorio, llamada:
   `animacion-[NOMBRE_DEL_INTEGRANTE]`
4. Dentro de ella crea una subcarpeta independiente por animación. El nombre debe indicar el número, la animación y la plataforma; por ejemplo:
   - `02-recorrido-camara-opengl`
   - `05-remolino-petalos-blender`
5. Entrega los archivos fuente editables, no solamente videos:
   - Para OpenGL: proyecto de Xcode, Visual Studio/CMake o Linux/CMake, según [PLATAFORMA_Y_SISTEMA_OPERATIVO]. Debe compilar de manera independiente y mostrar solamente lo necesario para demostrar la animación, con fondo neutro e iluminación sencilla. Reutiliza las dependencias y patrones disponibles en `RAIZ_PROYECTO`; si necesitas copiar código, copia únicamente lo mínimo a la carpeta de la animación.
   - Para Blender: parte de la fuente adecuada en `RAIZ_PROYECTO/modelos-blender`. Crea una copia dentro de la carpeta de la animación y entrega un `.blend` con materiales, cámara, luces sencillas, timeline y keyframes completos. Conserva empacados los recursos externos.
   - Para huesos: antes de comenzar, pregúntame por la fuente y la licencia del modelo humano que se va a animar. No descargues ni inventes un personaje sin mi aprobación. Conserva el modelo original y trabaja sobre una copia.
6. Cada animación debe poder iniciarse, reiniciarse o pausarse mediante input. En el clip aislado basta con una tecla documentada.
7. Incluye un `README.md` por animación con técnica, controles, dependencias, forma de abrir/compilar y forma sugerida de grabar o renderizar.
8. No generes renders ni videos finales salvo que te lo solicite expresamente. Primero entrégame los proyectos editables y pide que los revise.
9. Si encuentras que el modelo requiere separar piezas, corregir pivotes, hacer rigging, UV, bake o exportación, hazlo solamente en la carpeta nueva y documenta los cambios. No sobrescribas `modelos-blender`.
10. Verifica en proporción al trabajo: compila el proyecto OpenGL o valida que el `.blend` abra y que sus keyframes existan, pero no ejecutes procesos extensos de renderizado.

Lista de animaciones:

1. OpenGL — Mariposa monarca: máquina de estados (reposo, despegue, vuelo y aterrizaje), keyframes interpolados y jerarquía rígida para el aleteo.
2. OpenGL — Recorrido cinematográfico de cámara: entrada al panteón, camino de pétalos y llegada al altar, con keyframes e interpolación suave.
3. OpenGL — Encendido de veladoras: estados apagado/encendido/apagado, encendido progresivo por grupos y oscilación procedural de las flamas.
4. Blender — Papel picado entre faroles: seis piezas con movimiento de viento, fases y amplitudes diferentes, formando un ciclo continuo.
5. Blender — Remolino ceremonial de pétalos: los pétalos se levantan del camino, forman una espiral frente al altar y vuelven a asentarse.
6. Blender — Jarra sirviendo agua: inclinación de la jarra, aparición del chorro y aumento del nivel del vaso mediante keyframes y/o shape keys.
7. Huesos — Visitante 1: personaje humano que camina desde la entrada hasta el altar, carga una ofrenda y la deposita.
8. Huesos — Visitante 2: segundo personaje humano que sigue otra ruta, observa las tumbas y realiza un saludo o pequeño baile.

Primero respóndeme con un resumen corto de lo que entendiste, los archivos que crearás y cualquier fuente de modelo que necesites. Después realiza el trabajo con autonomía.
```

