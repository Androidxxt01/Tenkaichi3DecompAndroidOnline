# Dragon Rage 0.7.1

Una corrección de la 0.7.0 para GPU Mali con el renderizador **OpenGL ES**.

## Correcciones
- **Mali + OpenGL ES: bloques negros en la imagen y luego el juego congelado.** La 0.7.0 leía el alfa de la pantalla
  directamente del frame buffer (framebuffer fetch) en vez de copiarlo. En móviles con Mali-G52 (Moto g05, Redmi 12C)
  el driver dibujaba bloques negros, iba a 2 cuadros por segundo y dejaba de devolver imágenes tras unos segundos, en
  los menús o al empezar una pelea. Vuelve a estar **desactivado**: OpenGL ES funciona como en la 0.6.0. (Sigue en el
  motor para pruebas: ajuste `gl_fetch` = 1.)
- Ajustes → Renderizador: elegir **OpenGL ES en una GPU Mali** ahora avisa que es varias veces más lento en las peleas
  que Vulkan (en una Mali-G52: unos 4 cuadros por segundo contra 12 a 26). **Automático** (Vulkan) es la opción
  correcta en esos móviles, salvo que Vulkan no arranque.

## Notas de actualización
- Se instala sobre la 0.7.0 / 0.6.0 y conserva todo. Si cambiaste a OpenGL ES en una Mali, vuelve a Automático.
