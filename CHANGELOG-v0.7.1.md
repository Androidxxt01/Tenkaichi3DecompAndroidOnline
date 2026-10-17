# Dragon Rage 0.7.1

A fix for 0.7.0 on Mali GPUs with the **OpenGL ES** renderer.

## Fixes
- **Mali + OpenGL ES: black blocks over the picture, then a frozen game.** 0.7.0 read the screen's alpha straight
  from the frame buffer (framebuffer fetch) instead of copying it. On Mali-G52 phones (Moto g05, Redmi 12C) the
  driver drew black blocks, ran at 2 pictures a second and stopped giving pictures back after a few seconds, in menus
  or as a fight began. It is **off** again: OpenGL ES works as in 0.6.0. (It stays in the engine for testing:
  setting `gl_fetch` = 1.)
- Settings → Renderer: choosing **OpenGL ES on a Mali GPU** now says that it is several times slower in fights than
  Vulkan (a Mali-G52: about 4 pictures a second against 12 to 26). **Automatic** (Vulkan) is the right choice there
  unless Vulkan does not start.

## Update notes
- Installs over 0.7.0 / 0.6.0 and keeps everything. If you switched to OpenGL ES on a Mali, switch back to Automatic.
