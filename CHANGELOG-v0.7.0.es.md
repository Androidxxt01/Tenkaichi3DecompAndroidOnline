# Dragon Rage 0.7.0

**El online vuelve a funcionar** (versión 0.1.36 del port de PC), el launcher muestra **el estado del servicio online**
nada más abrirse, la página **Dragon Net Battle** se rehízo para pantallas táctiles, **los packs de texturas se guardan
en ASTC** para las GPU de móvil, las peleas en Mali con OpenGL ES ya no copian la pantalla en cada efecto, y las
imágenes de disco que cerrarían el juego se detectan al importarlas.

## Novedades

### Online
- **Versión 0.1.36 del port de PC.** El servicio de partidas ahora rechaza versiones anteriores a la 0.1.36, y la 0.6.0
  era la 0.1.35: ese era el mensaje *"Update available — you have 0.1.35; 0.1.36 or newer is needed"*. Esta versión
  vuelve a ser aceptada. También de la 0.1.36: los objetos se quitan en las sesiones online, para que ambos lados
  peleen con las mismas reglas.
- **Aviso del estado del online** arriba del launcher, en español, inglés y portugués. En cada inicio consulta el
  servicio de partidas y dice, en letras grandes, una de estas:
  - *El online funciona* (con cuántos jugadores buscan partida ahora);
  - *El online necesita una actualización* (la versión que pide el servicio y la que tienes);
  - *Partida rápida: solo versiones oficiales*, si el servicio pasa a aceptar solo las versiones firmadas del port de
    PC — la conexión directa (dirección y puerto) sigue funcionando en ese caso;
  - *No se pudo contactar el servicio online* (sin internet, o el servicio no respondió).
  Un botón de actualizar vuelve a consultar; *Cómo funciona el online* explica el resto.
- **Dragon Net Battle táctil**: la página online dentro del juego ahora es una pantalla completa con el estilo del
  launcher, botones grandes y desplazamiento táctil: tu nombre, **Partida rápida** (las colas y cuántos esperan),
  **Crear** (código de sala o dirección, puerto, rollback, retraso de entrada, tipo de batalla, tiempo, velocidad) y
  **Unirse** (código de sala con botón *Pegar*, o dirección y puerto). Hace exactamente lo mismo que la página del PC
  (mismas opciones, mismo matchmaking, mismos mensajes) y avisa cuando hace falta actualizar.

### Más rápido en Mali (y otras GPU de móvil)
- **OpenGL ES: framebuffer fetch.** Los efectos que leen el alfa de la pantalla (la prueba de *alfa de destino* del
  juego) copiaban la imagen y rompían los tiles de la GPU cada vez — hasta 14 copias y 50 render passes por cuadro en
  una Mali-G52. Donde la GPU tiene `GL_EXT_shader_framebuffer_fetch` (Mali, Adreno, PowerVR) el shader ahora lo lee
  directamente, sin copia. Vulkan no cambia.
- **Packs de texturas en ASTC.** Las GPU de móvil leen ASTC pero no el DXT de los `.dds` de los packs, así que se
  descomprimían a píxeles planos (cuatro veces la memoria). Ahora cada reemplazo se convierte una vez, en segundo
  plano, en un hilo de baja prioridad, a una copia ASTC 4×4 en `textures_astc/` (8 bits por píxel), y se carga desde
  ahí en adelante. El pack no se modifica; la carpeta se puede borrar cuando quieras. *Ajustes → Packs de texturas en
  ASTC* lo desactiva.

### Importación de la ISO, y por qué se cerró el juego
- **Las imágenes de disco modificadas se detectan al importar.** Algunos móviles se cerraban al iniciar el juego
  después de importar la ISO: la imagen estaba modificada (un mod o un dump reempaquetado) con un `DBZP.BIN` demasiado
  pequeño para la lista de archivos del juego. La importación ahora lo comprueba y lo dice, en vez de un juego que se
  cierra.
- **Las copias incompletas se detectan**: cada archivo se comprueba después de copiarlo (almacenamiento lleno, archivo
  ilegible).
- **"El juego se cerró: …"**: cuando la última ejecución terminó mal, el launcher ahora dice por qué, a partir del log
  del juego — un archivo del juego que falta, dañado o modificado, memoria insuficiente, un motor que no arrancó, o un
  cierre inesperado — con qué hacer y un botón *Exportar log*.

## Notas de actualización
- Se instala sobre la 0.6.0 y conserva datos del juego, ajustes, partidas guardadas, packs de texturas y la
  distribución del control.
- Las primeras peleas con un pack de texturas convierten sus imágenes en segundo plano; desde la siguiente carga vienen
  de las copias ASTC.
- La app sigue necesitando tu propia imagen de disco de la versión estadounidense (SLUS-21678); no distribuye datos del
  juego.
