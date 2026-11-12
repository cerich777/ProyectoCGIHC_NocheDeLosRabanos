# Dependencias del laboratorio

Origen local: `CompuGrafica/CompuGraficaLab/ConfiguracionInicial` y su carpeta hermana `External Libraries`.
Se copiaron únicamente componentes necesarios, sin modelos, imágenes, cachés ni proyectos de otras prácticas.

| Componente | Archivos utilizados |
| --- | --- |
| GLEW | include, glew32.lib y glew32.dll de Release/Win32, LICENSE.txt |
| GLFW | include, glfw3dll.lib y glfw3.dll de lib-vc2015 (x86), COPYING.txt |
| GLM | árbol de encabezados glm |
| Assimp | include, assimp-vc140-mt.lib y DLL x86 |
| SOIL2 | fuentes C y encabezados de ConfiguracionInicial/SOIL2; avisos de autoría dentro de las fuentes |

Se conservan los avisos disponibles en los archivos originales. No se encontró un archivo separado de licencia de Assimp/GLM en el paquete local; sus encabezados mantienen los avisos originales. Las dependencias corresponden al material existente del laboratorio, sin actualizar versiones.

Camera.h, Model.h y Mesh.h proceden del código del laboratorio y tienen adaptaciones identificadas al inicio. Shader.h y la escena base son nuevos. Los shaders modelLoading se conservan como referencia de integración.
