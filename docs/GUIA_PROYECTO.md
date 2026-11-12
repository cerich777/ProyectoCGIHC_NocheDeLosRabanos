# Alcance implementado según la propuesta

Referencia de trabajo: propuesta del equipo 4, secciones 2, 4, 6 y 8. El PDF se utilizó como referencia de requisitos, sin modificarlo. Esta entrega implementa la parte estática del recorrido; las cinco animaciones A1–A5 quedan fuera de esta etapa por petición expresa.

| Requisito de la propuesta | Implementación |
| --- | --- |
| Zócalo simplificado, kiosco central y exhibiciones alrededor | Plaza rectangular con kiosco octagonal, pasillos y seis puestos distribuidos en los laterales y fondo |
| Entrada delimitada, recorridos libres y vista general | Puente y barandales en el acceso; navegación libre; mapa y vista general con O |
| Kiosco, piso, jardineras y seis puestos modulares | Geometría propia en `Escenario.cpp`, escalera transitable, cuatro jardineras, bancas y árboles |
| Tres obras de rábano, una de totomoxtle y una de flor | Cinco modelos originales estáticos, creados y guardados en Blender; OBJ/MTL importados con Assimp |
| Escala, UV, materiales y texturas | Escala de 1 unidad aproximada a 1 metro; exportación Y arriba; UV y normales; texturas propias del entorno y colores de material de las obras |
| Ambiente tenue, luces de puestos/luminarias y fuente general | Luz ambiental y direccional, quince puntos de luz, mapa de sombras direccional y modos noche/día |
| Colisiones y límites de navegación | Volumen circular de visitante contra cajas, movimiento subdividido y deslizamiento; límites de plaza/puente y altura del kiosco |
| Interacción y fichas culturales | Detección por distancia/dirección, una ficha activa a la vez, ocho puntos de interés y registro de cinco obras visitadas |
| Presencia social | Trece visitantes/personajes de plaza y un artesano en poses fijas; no hay movimiento automático |
| Organización e integración | Una solución, un ejecutable y módulos separados; recursos propios versionables y dependencias relativas |
| Animaciones de tallado, montaje y visitantes | Pendientes de las prácticas posteriores; no se agregaron bibliotecas nuevas para animarlas |

## Distribución

La tabla del PDF y el cronograma utilizan agrupaciones distintas para las zonas. Esta implementación identifica seis estaciones de exhibición: danza, mercado, jaguar, totomoxtle, flor y taller. Se añaden dos puntos de información: acceso e historia, y kiosco/plaza. Las fichas de las cinco primeras estaciones completan el contador de obras. El taller se mantiene como demostración estática.

El kiosco está en el centro; dos exhibiciones están a la izquierda, dos a la derecha y dos al fondo. Las jardineras dejan pasillos centrales y laterales. El acceso al kiosco se realiza por la escalera orientada hacia la entrada.

## Criterio artístico y cultural

La propuesta solicita un entorno representativo, no la reproducción exacta de una edición. La arquitectura, las esculturas, los personajes y la decoración son versiones estilizadas originales. El jaguar es una composición de fauna; no se inventa una leyenda ni se atribuye un significado indígena concreto. Las figuras no se anuncian como réplicas de obras participantes del concurso.

Las fichas resumen el contexto y materiales documentados en la propuesta: mercado de la Vigilia, 23 de diciembre, primer concurso formal en 1897, rábano, hoja de maíz y flor inmortal. Las referencias de consulta son las entradas de SIC y Gobierno de Oaxaca incluidas en el PDF. Se indica que el diseño expositivo es una interpretación del equipo.

## Qué queda para la siguiente etapa

Exclusivamente los sistemas y contenido animado A1–A5: tallado, montaje de totomoxtle, recorrido del visitante, composición de flor y colocación de escultura. Los personajes actuales no tienen esqueletos ni pistas de animación. Los objetos y módulos están separados para poder incorporar después el sistema que se enseñe en clase.

Antes de la entrega académica, el equipo debe revisar y personalizar textos, nombres y decisiones artísticas, además de registrar sus evidencias y participación. La aplicación no fabrica capturas de tableros, revisiones de compañeros ni documentos de gestión que no hayan ocurrido.
