# Noche de los Rábanos · Manual de uso

## Iniciar

En Windows, extraer el paquete completo y abrir `NocheDeLosRabanos.exe`. Conservar junto al programa las DLL y carpetas `Models`, `images` y `Shader`. El equipo debe tener controlador compatible con OpenGL 3.3 y runtime de Visual C++ x86. Si se trabaja con el código fuente, abrir la solución de Visual Studio y compilar **Release | Win32**.

La pantalla inicial explica los controles. Presionar Enter para empezar en el acceso de la plaza. Esta es una interpretación estilizada del Zócalo y sus exhibiciones, sin animaciones.

## Explorar

Caminar con W, A, S y D, y orientar la vista con el ratón. Shift aumenta la velocidad. La rueda ajusta el campo de visión. Tab libera el cursor para trabajar con otras ventanas; volver a pulsar Tab para capturarlo. Al cambiar de aplicación el cursor se libera automáticamente.

Los puestos se encuentran a ambos lados y al fondo de la plaza. El kiosco está en el centro y se puede subir por la escalera que mira hacia la entrada. Los muebles, jardineras, personajes y barandales impiden atravesar objetos sólidos.

## Consultar una obra

Acercarse a un puesto y mirar hacia él. Aparece una indicación con la tecla E y el título de la pieza. Presionar E para abrir su ficha; E o Esc la cierran. Se puede consultar un solo punto a la vez. El contador registra las cinco obras: danza, mercado, jaguar, totomoxtle y flor. El taller, acceso y kiosco ofrecen información adicional sin aumentar ese contador.

El mapa pequeño muestra los puntos de interés y la dirección de la cámara. M abre o cierra el mapa grande. Los puntos de las obras consultadas se muestran en verde. No hay un orden obligatorio.

## Vistas y ayuda

- N alterna día/noche.
- L permite comparar el aporte de las luces de puestos y luminarias.
- O abre una vista general de la plaza; volver a pulsarla devuelve la cámara al recorrido.
- R regresa al acceso y conserva las obras visitadas.
- H muestra la ayuda.
- P guarda una captura BMP en la carpeta `capturas` junto al ejecutable.
- Esc cierra el panel abierto; si no hay ninguno, termina el programa.

Si un error de recursos impide iniciar, verificar que se extrajo todo el paquete y no solo el `.exe`. Para ver el diagnóstico desde Visual Studio usar Ctrl+F5; la consola indica qué archivo falta. El proyecto no necesita conexión a Internet para ejecutarse.

## Contenido

Cinco composiciones originales simplificadas y una demostración de trabajo artesanal en pose fija. Las fichas se apoyan en la propuesta académica del equipo y sus referencias de SIC y Gobierno de Oaxaca. No son reproducciones de obras premiadas ni un levantamiento exacto de la plaza. Las acciones de tallado, montaje y caminata se incorporarán en la etapa de animación.
