# Noche de los Rábanos · Zócalo de Oaxaca

Recorrido tridimensional estático en C++17 y OpenGL 3.3, basado en la propuesta del equipo. Arquitectura y obras originales estilizadas, con fuentes editables. Las animaciones se reservan para las prácticas posteriores.

## Ejecutar en Visual Studio

1. Visual Studio 2022 con **Desarrollo para el escritorio con C++**, MSVC v143 y Windows SDK.
2. Abrir `NocheDeLosRabanos.sln` desde la raíz del repositorio.
3. Elegir **Release | Win32** para recorrer la escena, o **Debug | Win32** para depurar.
4. Compilar y presionar **F5** o **Ctrl+F5**. Enter cierra la bienvenida.

Los binarios locales de Assimp y GLFW son x86; no se ofrece configuración x64. Las DLL, OBJ/MTL, materiales y shaders se copian automáticamente al directorio del ejecutable. Blender no es necesario para jugar el recorrido.

## Lo que incluye

- Plaza con pavimento texturizado, kiosco octagonal transitable, escalera, columnas, cubierta y barandales.
- Cuatro jardineras con árboles, bancas, fachadas y portales simplificados, luminarias y acceso con puente/barandales.
- Seis puestos de exhibición, carteles propios y decoración de papel picado.
- Cinco obras originales importadas de OBJ/MTL con Assimp: danza de rábanos, mercado, jaguar, figura de totomoxtle y arreglo de flor inmortal.
- Taller con artesano y materiales en pose fija; visitantes estáticos en la plaza.
- Iluminación nocturna con luz ambiental, direccional y quince luces puntuales, sombras y materiales texturizados. Modo de día para revisar el escenario.
- Cámara a altura de visitante, colisiones y límites transitables, acceso al kiosco, mapa y vista general.
- Ocho fichas informativas, indicación de cercanía y progreso de las cinco obras.
- Pruebas de carga, colisiones, escalera, selección, progreso y renderizado; generación de capturas.

La arquitectura es una interpretación reconocible y de escala reducida, no un levantamiento exacto del Zócalo. Las obras se diseñaron para esta versión y no se presentan como réplicas de piezas reales. No hay rigging, fotogramas clave, caminatas ni cambios animados de objetos.

## Controles

| Tecla | Acción |
| --- | --- |
| Enter | Cerrar bienvenida |
| WASD + ratón | Caminar y mirar |
| Shift | Caminar más rápido |
| Rueda | Acercar/alejar la vista |
| E | Abrir/cerrar la ficha de una pieza cercana |
| M | Mostrar/cerrar mapa grande |
| H | Ayuda |
| N | Alternar día y noche |
| L | Encender/apagar el aporte de las luces puntuales |
| O | Vista general de la plaza / volver a caminar |
| R | Regresar al acceso |
| Tab | Liberar/capturar el cursor |
| P | Guardar captura BMP en `capturas/` junto al ejecutable |
| Esc | Cerrar ficha/ayuda/mapa; salir si no hay panel abierto |

## Organización como las prácticas

```text
NocheDeLosRabanos.sln
External Libraries/              GLEW, GLFW, GLM, Assimp y SOIL2
NocheDeLosRabanos/
  Main.cpp                       Ventana, controles y ciclo principal
  Escenario.cpp/.h               Plaza, puestos, personajes y puntos de interés
  Render.cpp/.h                  Geometría, importación, materiales, sombras y dibujo
  Colisiones.cpp/.h              Obstáculos, límites y altura de la escalera
  Interaccion.cpp/.h             Selección, ficha activa y progreso
  Interfaz.cpp/.h                Texto, paneles, mapa y ayuda
  Camera.h, Shader.h, Model.h, Mesh.h   Ayudantes conservados del laboratorio
  Shader/                        Shaders de recorrido, sombras, cielo e interfaz
  Models/Proyecto/               Cinco OBJ/MTL y fuente editable de Blender
  images/Proyecto/               Texturas y carteles originales
scripts/                         Compilación y generación de recursos
docs/                            Manual técnico, alcance y pruebas
```

Los filtros de Visual Studio conservan **Archivos de encabezado**, **Archivos de origen > Shader** y **Archivos de recursos**. El recorrido usa `Render` para administrar sus recursos con liberación antes de cerrar el contexto; los ayudantes antiguos se conservan como referencia, sin duplicar su carga de modelos.

## Compilar y comprobar

Desde la raíz, en PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Configuration Release -SmokeTest
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Configuration Debug -CaptureDir .\build\evidencia
```

La prueba ejecuta validaciones de colisiones e interacción y dibuja vistas del acceso, la plaza, los seis puestos, una ficha, el modo de día y el kiosco. La ventana de prueba permanece oculta y el proceso termina automáticamente.

## Recursos y Git

Los modelos e imágenes propios de `Models/Proyecto` e `images/Proyecto` se incluyen para que el equipo pueda clonar y ejecutar la escena completa. No se copiaron los modelos o texturas de las prácticas. Otros recursos locales permanecen excluidos por las reglas originales. `.vs`, `build`, capturas de ejecución y archivos personales de Visual Studio no se agregan.

Las obras pueden editarse en `Models/Proyecto/Obras_NocheDeLosRabanos.blend`; ver su README. Los scripts `crear_obras_blender.py` y `crear_texturas.py` permiten regenerar los recursos originales. No ejecutar los generadores sobre ediciones manuales sin conservarlas primero.

También se incluye `Models/Proyecto/Escenario_NocheDeLosRabanos.blend` con el escenario completo editable y texturas empacadas. Para generar un paquete ejecutable verificado: `powershell -ExecutionPolicy Bypass -File .\scripts\empaquetar.ps1`; el ZIP queda en `entrega/`, fuera de Git.

No se realiza commit ni push automáticamente:

```powershell
git status
git add .
git diff --cached --stat
git commit -m "Implementa recorrido estatico de la Noche de los Rabanos"
```

Consultar `docs/GUIA_PROYECTO.md`, `docs/MANUAL_TECNICO.md` y `docs/PRUEBAS.md`.
