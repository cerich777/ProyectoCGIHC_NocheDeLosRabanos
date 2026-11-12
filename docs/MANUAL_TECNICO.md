# Manual técnico de la versión estática

## Inicio y dependencias

Visual Studio 2022, v143, C++17, Win32, OpenGL 3.3. Se conservan las dependencias del laboratorio: GLFW (ventana/entrada), GLEW (funciones OpenGL), GLM (matemáticas), Assimp (OBJ/MTL) y SOIL2 (PNG). Texto e interfaz utilizan GDI de Windows, ya incluido en el sistema; no se instaló una biblioteca externa de interfaz, fuentes ni animación.

El proceso establece el directorio de trabajo junto al ejecutable. MSBuild copia los shaders, texturas y OBJ/MTL al compilar. El `.blend` se conserva en fuentes, pero no se copia como recurso de ejecución.

## Arquitectura

`Main.cpp` mantiene la ventana, entrada, cámara y estados de ayuda/mapa/ficha/vista general. `Escenario` construye piezas con matrices de transformación y mantiene luces, obstáculos y fichas. La geometría común se reutiliza para elementos arquitectónicos. Los puestos utilizan una matriz padre para transformar tanto piezas como luces y puntos de interés.

`Render` crea primitivas con normales/UV, carga texturas con SOIL2 e importa las cinco obras con Assimp. Convierte las piezas estáticas a posiciones/normales globales y agrupa por textura. Los colores de material se conservan por vértice. La agrupación reduce llamadas de dibujo sin cambiar la separación lógica de objetos. Al introducir animaciones habrá que dibujar las partes móviles con transformaciones independientes, fuera de esos lotes estáticos.

Los recursos GPU se destruyen mientras el contexto está activo. `Render` no se puede copiar. Los archivos `Camera.h`, `Model.h` y `Mesh.h` del laboratorio permanecen como referencia; no administran los objetos del nuevo recorrido. `Shader.h` se utiliza como compilador de programas, cuyos identificadores libera su propietario.

## Iluminación y materiales

El shader de recorrido combina albedo, color de material, ambiente, difusa direccional, especular y luces puntuales con atenuación. Las caras posteriores se iluminan para conservar la lectura de hojas y superficies delgadas. Se aplica mapeo tonal y gamma al final. Los materiales incluyen adoquín, piedra, madera, tela, muros, vegetación y cubierta de cobre.

La escena tiene quince luces puntuales: seis puestos, ocho luminarias y una del kiosco. La sombra direccional se prepara en un mapa de profundidad de 2048 × 2048, con comparación PCF 3 × 3. Se calcula al cargar porque toda la geometría es estática. El modo de día cambia ambiente y luz general; L permite comparar el aporte puntual. Los elementos emisivos representan las bombillas.

Las luces puntuales no proyectan mapas de sombra propios: el alcance utiliza sombras direccionales y luces de exhibición atenuadas. El cielo es un gradiente con estrellas y luna estáticas, sin fotografías ni ciclo automático de tiempo.

## Navegación y colisiones

La cámara conserva una altura de 1.7 unidades sobre el piso y el desplazamiento se normaliza para evitar velocidad diagonal extra. El visitante se aproxima por un círculo de radio 0.28 en XZ. Los obstáculos son cajas contra las que se resuelve cada eje; el desplazamiento se subdivide en pasos de como máximo 0.09 para evitar atravesar obstáculos con un cuadro largo.

Las jardineras, mesas, postes, personajes y pretil del kiosco bloquean el movimiento. La escalera tiene una altura de navegación progresiva y el piso interior eleva la cámara una unidad. Se rechazan subidas bruscas para impedir entrar por los costados de la plataforma. No hay salto, gravedad ni física dinámica porque no los requiere el recorrido estático.

## Interacción e interfaz

Se selecciona un punto a menos de 5.2 unidades y situado en la dirección de la cámara (o muy cerca). E activa o cierra una sola ficha. El contador de visita se actualiza solo para las cinco obras; el acceso, taller y kiosco son puntos complementarios. Los paneles detienen el desplazamiento y liberan el cursor.

GDI dibuja texto Unicode en un DIB que se transfiere a una textura RGBA. Un shader la compone sobre la escena. La referencia es 1280 × 720 y escala con la ventana. El mapa representa puestos, jardines, kiosco y posición/dirección del visitante. Al perder foco se libera el cursor para que no quede atrapado fuera de la aplicación.

## Recursos originales y edición

`scripts/crear_obras_blender.py` genera cinco colecciones en Blender, OBJ triangulados con UV/normales y MTL de colores. Los OBJ usan Y arriba, origen centrado en la base y altura 1.7 antes de aplicar la transformación del puesto. La fuente `.blend` presenta las colecciones en fila para su edición. No contiene rigging ni animación.

`scripts/crear_texturas.py` genera patrones repetibles y carteles propios con Pillow. Las fuentes usadas para rasterizar carteles son fuentes del sistema de Windows; no se redistribuyen archivos de tipografía. Los recursos ya generados están incluidos; estos scripts son opcionales para ejecutar el programa.

Los archivos en `Models/Proyecto` e `images/Proyecto` se incluyen expresamente en `.gitignore`, incluso los `.obj` de modelo (que no deben confundirse con objetos del compilador). Otros recursos locales siguen ignorados. Los resultados de compilación quedan fuera de Git.

Además de las cinco obras, `Models/Proyecto/Escenario_NocheDeLosRabanos.blend` conserva toda la arquitectura y las piezas de la escena como objetos editables, con sus texturas empacadas. Se obtiene del mismo ejecutable mediante `--export-scene ruta.obj`, seguido de `scripts/crear_escenario_blender.py`. La exportación OBJ intermedia va en `build` y no se versiona. El `.blend` completo sirve como fuente de trabajo artístico; editarlo no cambia automáticamente la geometría que actualmente construye `Escenario.cpp`.

## Exportación y entrega

Compilar Release y distribuir el contenido completo de `build/Win32/Release`: ejecutable, tres DLL de dependencias, carpetas `Models`, `images` y `Shader`. El otro equipo necesita Windows, controlador con OpenGL 3.3 y el runtime de Visual C++ x86 correspondiente. No distribuir únicamente el `.exe`.

La documentación de pruebas describe lo comprobado en este equipo. Debe repetirse una compilación/ejecución en el equipo de demostración antes de la entrega académica.
