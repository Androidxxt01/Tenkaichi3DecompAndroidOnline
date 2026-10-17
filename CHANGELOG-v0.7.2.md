# Dragon Rage 0.7.2

The fights run the same engine code as in 0.6.0 again, on every GPU.

## Changes
- **ASTC texture packs are off by default** (Settings → ASTC texture packs, now marked experimental). With a texture
  pack, 0.7.0 converted each new replacement to ASTC on the CPU while the game was running; on fast phones that
  cost frames at 60 and 120 pictures a second. Off, texture packs load exactly as in 0.6.0.
- With 0.7.1's framebuffer fetch already off, nothing that 0.7.x added runs in a fight any more unless you turn it
  on. What stays: the PC port's 0.1.36 (online), the online status banner, the Dragon Net Battle page, the import
  checks and the "why the game closed" dialog.

## Update notes
- Installs over 0.7.x / 0.6.0 and keeps everything. The `textures_astc` folder, if one was made, can be deleted.
