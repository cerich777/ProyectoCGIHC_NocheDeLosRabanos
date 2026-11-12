# Comprobación del recorrido estático

## Pruebas automáticas

Ejecutar desde la raíz:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Configuration Debug -SmokeTest
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Configuration Release -CaptureDir .\build\evidencia
```

La prueba usa el mismo ejecutable, contexto OpenGL y recursos de la aplicación. No reemplaza una revisión en el equipo de entrega. Las validaciones lanzan error y devuelven código distinto de cero si fallan.

| Comprobación | Criterio |
| --- | --- |
| Importación real | Cinco archivos OBJ con sus MTL y normales válidas cargan mediante Assimp |
| Escena y luces | Ocho fichas y entre 12 y 24 puntos de luz; geometría preparada en menos de 30 lotes |
| Obstáculos | No se puede caminar dentro de una jardinera |
| Pasos grandes | Desplazamientos de 100 unidades no atraviesan límites de plaza o puente |
| Escalera | Se puede avanzar desde el pasillo al centro del kiosco, con elevación de cámara |
| Accesos a puestos | Las seis posiciones de consulta están libres y seleccionan el puesto correcto |
| Fichas | Abrir/cerrar cada puesto y contar únicamente las cinco obras |
| Distancia | No seleccionar una obra lejana fuera del alcance |
| Controles de interfaz | Enter, E, Esc, M, N, L, O y R cambian el estado previsto; Esc no sale al cerrar una ficha |
| Renderizado | Shaders compilan/enlazan, FBO de sombras completo y ausencia de errores OpenGL en las 22 vistas |
| Vistas | Entrada, general nocturna, seis puestos, ocho fichas, día, interior del kiosco, ayuda, mapa, luces apagadas y progreso completo |
| Recursos empaquetados | La copia del ejecutable y recursos a un directorio independiente pasa la misma prueba |

La escena contiene unas 204 mil caras triangulares en quince lotes. La cifra incluye arquitectura, decoración, personajes y obras; no es una medición de rendimiento en todos los equipos. La compilación y pruebas utilizan el controlador OpenGL 3.3 disponible en este equipo.

## Revisión visual

Las capturas se obtienen del framebuffer del programa, no de maquetas. Se revisan el acceso, la plaza de día/noche, las cinco obras, el taller, kiosco, mapa, ayuda y fichas para comprobar lectura de materiales, contraste y texto. Se conservan algunas evidencias en `docs/capturas`.

## Antes de presentar

1. Abrir la versión Release y caminar personalmente desde la entrada a los seis puestos y al interior del kiosco.
2. Consultar las cinco obras; comprobar contador y mapa; cerrar los paneles y volver a caminar.
3. Alternar día/noche y luces; probar minimizar, restaurar y cambiar de aplicación.
4. Ejecutar el paquete completo en el equipo de demostración con su controlador y runtime de Visual C++ x86.
5. Revisar con el equipo los textos y la correspondencia artística con la propuesta. Las obras y arquitectura son interpretaciones simplificadas; las animaciones no forman parte de esta versión.
