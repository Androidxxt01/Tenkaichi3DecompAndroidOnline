# Dragon Rage 0.7.0

**O online volta a funcionar** (versão 0.1.36 do port de PC), o launcher mostra **a situação do serviço online** logo
ao abrir, a página **Dragon Net Battle** foi refeita para toque, **os pacotes de textura são guardados em ASTC** para as
GPUs de celular, as lutas em Mali no OpenGL ES não copiam mais a tela a cada efeito, e imagens de disco que fechariam o
jogo são detectadas na importação.

## Destaques

### Online
- **Versão 0.1.36 do port de PC.** O serviço de partidas agora recusa versões anteriores à 0.1.36, e a 0.6.0 era a
  0.1.35: era essa a mensagem *"Update available — you have 0.1.35; 0.1.36 or newer is needed"*. Esta versão volta a
  ser aceita. Também da 0.1.36: os itens são zerados nas sessões online, para os dois lados lutarem com as mesmas regras.
- **Aviso da situação do online** no topo do launcher, em português, inglês e espanhol. A cada abertura ele consulta o
  serviço de partidas e diz, em letras grandes, uma destas:
  - *O online está funcionando* (com quantos jogadores estão procurando partida agora);
  - *O online precisa de atualização* (a versão que o serviço pede e a que você tem);
  - *Partida rápida: só versões oficiais*, se o serviço passar a aceitar só as versões assinadas do port de PC — a
    conexão direta (endereço e porta) continua funcionando nesse caso;
  - *Não foi possível falar com o serviço online* (sem internet, ou o serviço não respondeu).
  Um botão de atualizar consulta de novo; *Como funciona o online* explica o resto.
- **Dragon Net Battle feito para toque**: a página online dentro do jogo agora é uma tela inteira no estilo do
  launcher, com botões grandes e rolagem por toque: seu nome, **Partida rápida** (as filas e quantos estão esperando),
  **Criar** (código de sala ou endereço, porta, rollback, atraso de entrada, tipo de batalha, tempo, velocidade) e
  **Entrar** (código de sala com botão *Colar*, ou endereço e porta). Faz exatamente o que a página do PC faz (mesmas
  opções, mesmo matchmaking, mesmas mensagens) e avisa quando é preciso atualizar.

### Mais rápido em Mali (e outras GPUs de celular)
- **OpenGL ES: framebuffer fetch.** Os efeitos que leem o alfa da tela (o teste de *alfa de destino* do jogo) copiavam a
  imagem e quebravam os tiles da GPU a cada vez — até 14 cópias e 50 render passes por quadro numa Mali-G52. Onde a
  GPU tem `GL_EXT_shader_framebuffer_fetch` (Mali, Adreno, PowerVR) o shader agora lê direto, sem cópia. O Vulkan não
  mudou.
- **Pacotes de textura em ASTC.** As GPUs de celular leem ASTC mas não o DXT dos `.dds` dos pacotes, então eles eram
  descompactados para pixels puros (quatro vezes a memória). Agora cada substituição é convertida uma vez, em segundo
  plano, numa thread de baixa prioridade, para uma cópia ASTC 4×4 em `textures_astc/` (8 bits por pixel), e carregada
  dali em diante. O pacote em si não é alterado; a pasta pode ser apagada quando quiser. *Configurações → Pacotes de
  textura em ASTC* desliga.

### Importação da ISO, e por que o jogo fechou
- **Imagens de disco modificadas são detectadas na importação.** Alguns celulares fechavam logo ao iniciar o jogo
  depois de importar a ISO: a imagem era modificada (um mod ou um dump reempacotado) com um `DBZP.BIN` pequeno demais
  para a lista de arquivos do jogo. A importação agora verifica isso e avisa, em vez de um jogo que fecha.
- **Cópias incompletas são detectadas**: cada arquivo é conferido depois de copiado (armazenamento cheio, arquivo
  ilegível).
- **"O jogo fechou: …"**: quando a última execução terminou mal o launcher agora diz por quê, a partir do log do jogo —
  arquivo do jogo faltando, danificado ou modificado, memória insuficiente, motor que não iniciou, ou um travamento —
  com o que fazer e um botão *Exportar log*.

## Notas de atualização
- Instala por cima da 0.6.0 e mantém dados do jogo, configurações, saves, pacotes de textura e o layout do controle.
- As primeiras lutas com um pacote de textura convertem as imagens em segundo plano; a partir do próximo carregamento
  elas vêm das cópias ASTC.
- O app continua precisando da sua própria imagem de disco da versão americana (SLUS-21678); ele não distribui dados
  do jogo.
