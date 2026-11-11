CEMPASÚCHIL: OFRENDA, MEMORIA Y TRADICIÓN
Proyecto final de Computación Gráfica e Interacción Humano-Computadora
Facultad de Ingeniería, UNAM
Equipo 8 - Grupo 04

======================================================================
1. DESCRIPCIÓN
======================================================================

CEMPASÚCHIL es una experiencia gráfica interactiva en 3D desarrollada
con C++17, OpenGL y GLSL. La aplicación permite recorrer un panteón,
seguir un sendero de pétalos iluminado por veladoras y faroles, y llegar
a una ofrenda de Día de Muertos.

La escena incluye materiales texturizados, iluminación dinámica,
fotografías, papel picado, mariposas monarca y nueve animaciones
interactivas creadas mediante código, Blender y animación por huesos.

======================================================================
2. REQUISITOS MÍNIMOS
======================================================================

- Windows 10 u 11 de 64 bits.
- Tarjeta gráfica y controlador compatibles con OpenGL 3.3 Core o
  superior.
- Procesador de 64 bits.
- 4 GB de memoria RAM como mínimo.
- 1 GB de espacio libre en disco para la instalación.
- Teclado y ratón.

Se recomienda instalar los controladores más recientes proporcionados
por el fabricante de la tarjeta gráfica. Algunas máquinas virtuales no
ofrecen OpenGL 3.3, aunque el equipo anfitrión sí sea compatible.

No es necesario instalar CMake, Visual Studio, Blender ni las
dependencias de desarrollo para utilizar la versión empaquetada.

======================================================================
3. INSTALACIÓN EN WINDOWS
======================================================================

1. Descargue el archivo CEMPASUCHIL-Setup.exe.
2. Haga doble clic en el instalador.
3. Si Windows muestra una advertencia por tratarse de una aplicación
   académica sin firma digital, revise que el archivo provenga del
   repositorio oficial del proyecto antes de continuar.
4. Siga los pasos del asistente y seleccione la carpeta de instalación.
5. Al finalizar, abra CEMPASÚCHIL desde el menú Inicio o desde el acceso
   directo creado por el instalador.

El ejecutable, las texturas, los modelos, las animaciones y los shaders
se instalan juntos. No se deben mover o eliminar individualmente las
carpetas assets y shaders.

Para desinstalar la aplicación, utilice Configuración > Aplicaciones >
Aplicaciones instaladas > CEMPASÚCHIL > Desinstalar, o ejecute el
desinstalador incluido en la carpeta del programa.

======================================================================
4. INICIO Y NAVEGACIÓN
======================================================================

La cámara inicia en la entrada del panteón, aproximadamente a 1.3 m del
suelo y orientada hacia el altar. En el modo de caminata, la cámara se
mantiene a nivel del terreno y presenta un balanceo sutil que simula los
pasos del visitante.

Para orientar la cámara, presione TAB para capturar el ratón y después
muévalo. Presione TAB nuevamente para liberar el cursor.

CONTROLES GENERALES

  W, A, S, D            Caminar por la escena.
  Shift                 Aumentar temporalmente la velocidad.
  TAB                   Capturar o liberar el ratón.
  Movimiento del ratón  Orientar la cámara cuando el ratón está capturado.
  Rueda del ratón       Ajustar la velocidad de desplazamiento.
  F                     Alternar entre caminata y vuelo libre.
  Q / E                 Descender o ascender en modo vuelo.
  F2                    Alternar entre renderizado sólido y malla.
  R                     Regresar a la entrada mirando hacia el altar.
  L                     Apagar o encender gradualmente los faroles.
  Esc                   Cerrar la aplicación.

Las teclas Q y E solamente funcionan cuando el vuelo está activado.

======================================================================
5. ANIMACIONES
======================================================================

  1  Mariposa monarca: despegue, vuelo y aterrizaje.
  2  Recorrido automático de cámara desde el panteón hasta el altar.
  3  Encendido y apagado progresivo de las veladoras.
  4  Movimiento del papel picado producido por el viento.
  5  Remolino de pétalos frente a la ofrenda.
  6  Jarra sirviendo agua en un vaso.
  7  Visitante que lleva una ofrenda y realiza una reverencia.
  8  Visitante que recorre las tumbas, observa, saluda y baila.
  9  Esqueleto bailando junto a la entrada del panteón.

Presione nuevamente la tecla correspondiente cuando la interacción
admita detenerse, reanudarse o cambiar de estado.

======================================================================
6. OPCIONES DE LÍNEA DE COMANDOS
======================================================================

Las opciones siguientes pueden utilizarse desde PowerShell o Símbolo
del sistema:

  --vista N           Iniciar en una vista numerada de 0 a 6.
  --entrada           Iniciar en la entrada del panteón.
  --altar             Iniciar con una vista frontal del altar.
  --captura ARCHIVO   Guardar una captura en formato PPM.

Ejemplo desde PowerShell:

  & "C:\Program Files\CEMPASUCHIL\CEMPASUCHIL.exe" --altar

Ejemplo para guardar una captura en la carpeta Imágenes del usuario:

  & "C:\Program Files\CEMPASUCHIL\CEMPASUCHIL.exe" --altar `
    --captura "$env:USERPROFILE\Pictures\altar.ppm"

La ubicación real del ejecutable puede variar si se eligió otra carpeta
durante la instalación.

======================================================================
7. SOLUCIÓN DE PROBLEMAS
======================================================================

LA VENTANA NO ABRE O INDICA QUE OPENGL NO ES COMPATIBLE

- Actualice el controlador de la tarjeta gráfica desde el sitio del
  fabricante.
- Verifique que la GPU sea compatible con OpenGL 3.3 Core o superior.
- Evite ejecutar la aplicación mediante Escritorio remoto o dentro de
  una máquina virtual sin aceleración 3D.

FALTAN MODELOS, TEXTURAS O SHADERS

- Reinstale la aplicación con el instalador completo.
- No ejecute una copia aislada de CEMPASUCHIL.exe.
- No cambie la ubicación de las carpetas assets y shaders dentro de la
  instalación.

EL RENDIMIENTO ES BAJO

- Cierre aplicaciones que utilicen intensivamente la tarjeta gráfica.
- Actualice el controlador gráfico.
- Compruebe que Windows esté utilizando la GPU dedicada para ejecutar
  CEMPASUCHIL.exe, si el equipo tiene más de una GPU.

======================================================================
8. CRÉDITOS
======================================================================

Universidad Nacional Autónoma de México
Facultad de Ingeniería
Computación Gráfica e Interacción Humano-Computadora
Semestre 2027-1

Profesor:
Ing. José Ramón Pérez Athié

Equipo 8: "Los Jugosos"
- Basilio Illescas Andrés
- Cruz Macedo Samuel Santiago
- Pérez Olvera Alexis Abraham
- Trejo Olvera Emmanuel
