# Dragon Rage 0.7.1

Uma correção da 0.7.0 para GPUs Mali com o renderizador **OpenGL ES**.

## Correções
- **Mali + OpenGL ES: blocos pretos na imagem e depois o jogo travado.** A 0.7.0 lia o alfa da tela direto do frame
  buffer (framebuffer fetch) em vez de copiá-lo. Em celulares com Mali-G52 (Moto g05, Redmi 12C) o driver desenhava
  blocos pretos, rodava a 2 quadros por segundo e parava de devolver imagens depois de alguns segundos, nos menus ou
  no começo da luta. Ficou **desligado** de novo: o OpenGL ES funciona como na 0.6.0. (Continua no motor para testes:
  configuração `gl_fetch` = 1.)
- Configurações → Renderizador: escolher **OpenGL ES numa GPU Mali** agora avisa que ele é várias vezes mais lento nas
  lutas que o Vulkan (numa Mali-G52: cerca de 4 quadros por segundo contra 12 a 26). **Automático** (Vulkan) é a
  escolha certa nesses celulares, a não ser que o Vulkan não abra.

## Notas de atualização
- Instala por cima da 0.7.0 / 0.6.0 e mantém tudo. Se você mudou para OpenGL ES numa Mali, volte para Automático.
