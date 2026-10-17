# Dragon Rage 0.7.2

As lutas voltam a rodar o mesmo código de motor da 0.6.0, em todas as GPUs.

## Mudanças
- **Pacotes de textura em ASTC ficam desligados por padrão** (Configurações → Pacotes de textura em ASTC, agora
  marcado como experimental). Com um pacote de textura, a 0.7.0 convertia cada substituição nova para ASTC usando a CPU
  enquanto o jogo rodava; em celulares rápidos isso custava quadros a 60 e 120 por segundo. Desligado, os pacotes de
  textura carregam exatamente como na 0.6.0.
- Com o framebuffer fetch da 0.7.1 já desligado, nada do que a 0.7.x acrescentou roda numa luta, a não ser que você
  ligue. Continuam: a 0.1.36 do port de PC (online), o aviso da situação do online, a página Dragon Net Battle, a
  verificação da importação e o aviso de por que o jogo fechou.

## Notas de atualização
- Instala por cima da 0.7.x / 0.6.0 e mantém tudo. A pasta `textures_astc`, se foi criada, pode ser apagada.
