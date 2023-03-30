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
- [ ] Port ncurses lib to framebuffer console.
    - [ ] Port [minesweeper](https://github.com/8dcc/minesweeper).
- [ ] Devices (Framebuffer, disks, etc.).
    - [ ] `stdin`, `stdout`, `stderr`.
        - [ ] Per process.
- [ ] Filesystems.
    - [ ] FAT.
    - [ ] Ext2.
- [ ] Userspace. Ring 3. Load executables from disk.

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
    - [X] Getchar system.
    - [X] Control backspace in getchar.
    - [X] Control backspace so you can only delete what you wrote.

