# Dragon Rage 0.7.0

**Online works again** (the PC port's release 0.1.36), the launcher shows **the online service's status** as soon as it
opens, the **Dragon Net Battle** page is rebuilt for touch, **texture packs are kept as ASTC** for phone GPUs, Mali
fights on OpenGL ES no longer copy the screen for every effect, and disc images that would close the game are caught
when they are imported.

## Highlights

### Online
- **The PC port's release 0.1.36.** The matchmaking service now refuses releases older than 0.1.36, and 0.6.0 was
  0.1.35: that was the *"Update available — you have 0.1.35; 0.1.36 or newer is needed"* message. This release is
  accepted again. Also from 0.1.36: items are cleared in online sessions, so both sides fight with the same rules.
- **Online status banner** at the top of the launcher, in English, Portuguese and Spanish. On every start it asks the
  matchmaking service and says, in big letters, one of:
  - *Online is working* (with how many players are searching right now);
  - *Online needs an update* (the release the service asks for and the one you have);
  - *Quick match: official builds only*, if the service starts accepting only the PC port's signed builds — direct
    connections (address and port) still work then;
  - *Could not reach the online service* (no internet, or the service did not answer).
  A refresh button checks again; *How online works* explains the rest.
- **Dragon Net Battle, made for touch**: the in-game online page is now a full screen in the launcher's style, with
  big buttons and touch scrolling: your name, **Quick match** (the queues and how many are waiting), **Host** (room
  code or address, port, rollback, input delay, battle type, time, speed) and **Join** (room code with a *Paste*
  button, or address and port). It does exactly what the PC page does (same options, same matchmaking, same
  messages) and tells you when an update is needed.

### Faster on Mali (and other phone GPUs)
- **OpenGL ES: framebuffer fetch.** Effects that read the screen's alpha (the game's *destination alpha* test) used to
  copy the picture and break the GPU's tiles each time — up to 14 copies and 50 render passes a frame on a Mali-G52.
  Where the GPU has `GL_EXT_shader_framebuffer_fetch` (Mali, Adreno, PowerVR) the shader now reads it directly, with
  no copy. Vulkan is unchanged.
- **ASTC texture packs.** Phone GPUs read ASTC but not the DXT of the packs' `.dds` files, so those were unpacked to
  plain pixels (four times the memory). Now each replacement is converted once, in the background, on one
  low-priority thread, to an ASTC 4×4 copy in `textures_astc/` (8 bits a pixel), and loaded from there from then on.
  The pack itself is not touched; the folder can be deleted at any time. *Settings → ASTC texture packs* turns it off.

### Importing the ISO, and why the game closed
- **Modified disc images are caught at import.** Some phones closed as soon as the game started after importing an
  ISO: the disc image was a modified one (a mod or a re-packed dump) whose `DBZP.BIN` is too small for the game's file
  list. The import now checks it and says so, instead of a game that closes.
- **Incomplete copies are caught**: every file is checked after it is copied (storage full, unreadable file).
- **"The game closed: …"**: when the last run ended badly the launcher now says why, from the game's log — a missing,
  damaged or modified game file, not enough memory, an engine that could not start, or a crash — with what to do and
  an *Export log* button.

## Update notes
- Installs over 0.6.0 and keeps game data, settings, saves, texture packs and the controller layout.
- The first fights with a texture pack convert its images in the background; from the next load they come from the
  ASTC copies.
- The app still needs your own disc image of the USA release (SLUS-21678); it distributes no game data.
