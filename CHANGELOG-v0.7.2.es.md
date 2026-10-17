# Dragon Rage 0.7.2

Las peleas vuelven a usar el mismo código de motor que la 0.6.0, en todas las GPU.

## Cambios
- **Los packs de texturas en ASTC quedan desactivados por defecto** (Ajustes → Packs de texturas en ASTC, ahora
  marcado como experimental). Con un pack de texturas, la 0.7.0 convertía cada reemplazo nuevo a ASTC con la CPU
  mientras el juego corría; en móviles rápidos eso costaba cuadros a 60 y 120 por segundo. Desactivado, los packs de
  texturas cargan exactamente como en la 0.6.0.
- Con el framebuffer fetch de la 0.7.1 ya desactivado, nada de lo que añadió la 0.7.x corre en una pelea salvo que lo
  actives. Se mantienen: la 0.1.36 del port de PC (online), el aviso del estado del online, la página Dragon Net
  Battle, la comprobación de la importación y el aviso de por qué se cerró el juego.

## Notas de actualización
- Se instala sobre la 0.7.x / 0.6.0 y conserva todo. La carpeta `textures_astc`, si se creó, se puede borrar.
