
# Todo
Stuff that needs to be/has been done.

### Features
- [X] Newline support for VGA terminal.
- [X] Scrolling support for VGA terminal (for excess lines).
- [X] Sysroot and similar file structure to [this](https://wiki.osdev.org/Meaty_Skeleton#libc_and_libk_Design).
- [X] Add allocation functions and heap.
- [X] Framebuffer support.
    - [X] Add framebuffer console array with char and color info.
    - [X] Replace vga functions (`vga.c`) with framebuffer console.
- [ ] Sleep functions ([Link](https://wiki.osdev.org/Programmable_Interval_Timer)).
- [ ] GDT and IDT.
- [ ] PCI devices.
- [ ] Keyboard.
- [ ] Ext2.
- [ ] Paging?

### Misc
- [X] Repace sysroot rules (not necesarily targets) with files, so it doesn't compile everytime.

