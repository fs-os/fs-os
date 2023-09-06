# Todo
Stuff that needs to be/has been done.

Feel free to make a [Pull Request](https://github.com/fs-os/fs-os/blob/main/CONTRIBUTING.md)
if you have any improvements to the OS.

### Pending
- [X] Paging
    - [X] 4GiB identity map.
    - [ ] Improve (Add map functions, allocate pages, etc.).
- [X] Multitasking.
    - [X] Non-preemptive with no priority.
    - [ ] Improve. Add priority, etc.
- [ ] Port [tinylisp](https://github.com/Robert-van-Engelen/tinylisp)
      interpreter.
- [ ] Consistent arrow key support (`getchar`, `curses.h`, etc.).
- [ ] Global framebuffer cursor.
    - [ ] Editable `getchar` line buffer with arrows.
- [ ] Scrollable help (like the `less` command) if it gets too long. Use curses.
- [ ] Devices (Framebuffer, disks, etc.).
    - [ ] `stdin`, `stdout`, `stderr`.
        - [ ] Per process.
- [ ] Filesystems.
    - [ ] FAT.
    - [ ] Ext2.
- [ ] Load executables from disk.

### Done
- [X] Newline support for VGA terminal.
- [X] Scrolling support for VGA terminal (for excess lines).
- [X] Sysroot and similar file structure to [this](https://wiki.osdev.org/Meaty_Skeleton#libc_and_libk_Design).
- [X] Add allocation functions and heap.
- [X] Framebuffer support.
    - [X] Add framebuffer console array with char and color info.
    - [X] Replace vga functions (`vga.c`) with framebuffer console.
- [X] Sleep functions ([Link](https://wiki.osdev.org/Programmable_Interval_Timer)).
- [X] GDT and IDT.
- [X] Keyboard.
    - [X] `getchar` system.
    - [X] Control backspace in `getchar`.
    - [X] Control backspace so you can only delete what you wrote.
- [X] Port ncurses lib to framebuffer console.
    - [X] Port [minesweeper](https://github.com/8dcc/minesweeper).
    - [X] Port [5x5](https://github.com/8dcc/5x5).
