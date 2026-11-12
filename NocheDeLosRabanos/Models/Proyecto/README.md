# Obras originales de esta versión

Cinco composiciones estilizadas propias: danza de rábanos, vida en el mercado, jaguar de la sierra, figura de totomoxtle y arreglo de flor inmortal. Son interpretaciones para el proyecto; no reproducen una pieza específica de una edición del concurso ni atribuyen una leyenda o tradición particular a una comunidad.

Los OBJ se importan realmente mediante Assimp al iniciar. Sus MTL incluyen los colores de cada parte. El archivo `Obras_NocheDeLosRabanos.blend` contiene las cinco obras en colecciones separadas, distribuidas en fila para editarlas. No contiene rig, acciones, keyframes ni animaciones.

`Escenario_NocheDeLosRabanos.blend` contiene también el escenario completo exportado del ejecutable, con sus texturas empacadas, luces y cámara de referencia. Las piezas están separadas para editarlas. La geometría del programa se mantiene en C++; editar este archivo no modifica automáticamente `Escenario.cpp`.

Convención OBJ: eje Y arriba; base y=0; altura de cada composición 1.7 unidades; triangulación, UV y normales explícitas. La escala de exhibición y posición final se aplican en `Escenario.cpp`.

Regenerar desde la raíz del repositorio, con Blender instalado:

```powershell
& 'C:\Program Files\Blender Foundation\Blender 5.2\blender.exe' --background --python .\scripts\crear_obras_blender.py
```

El script es determinista y genera tanto los OBJ/MTL como el archivo fuente. Si se edita el `.blend` manualmente, exportar la colección modificada respetando la convención anterior; no volver a ejecutar el generador sin conservar esos cambios. La aplicación solo necesita los OBJ y MTL; no necesita Blender para ejecutarse.
