# fs-os
**Free &amp; Simple Operating System**

### Description
The main purpose of this project is to learn, so expect some mistakes. If you want to contribute to the project, feel free to make a [pull request](https://github.com/fs-os/fs-os/pulls).

An important part of this project, as the name implies, is to keep it as simple as possible.
This is very important in order to understand how everything in the project works (*which is the main goal!*).
Also keep in mind that when we say *free*, we are talking about [free as in freedom](https://www.gnu.org/philosophy/free-sw.html).

Some interesting resources that influenced in the project's philosophy:
- [GNU philosophy](https://www.gnu.org/philosophy).
- [Suckless philosophy](https://suckless.org/philosophy).
- [cat-v.org](http://cat-v.org) and their [harmful stuff](http://harmful.cat-v.org).

### Building from source
#### Requisites
- i686-elf cross compiler from [here](https://github.com/fs-os/cross-compiler).
- [nasm](https://nasm.us) for compiling the assembly used in the project.
- [grub](https://www.gnu.org/software/grub) and [xorriso](https://www.gnu.org/software/xorriso) for creating the bootable image.
- (Optional) [qemu](https://www.qemu.org) for testing the ISO on a VM.

#### Building
Once you have all the requisites, simply run:
```console
$ git clone https://github.com/fs-os/fs-os
$ cd fs-os
$ make
...
```
Or for testing it directly with qemu:
```console
$ make qemu
...
```

### Todo
See [todo.md](TODO.md).

### Credits
- [OSDev wiki](https://wiki.osdev.org)
