# fs-os
**Free &amp; Simple Operating System**

### Description
The main purpose of this project is to learn, so expect some mistakes. If you want to
contribute to the project, feel free to make a [pull request](https://github.com/fs-os/fs-os/pulls).

This project has a wiki explaining how the OS works from the start, so feel free to
check it out [here](https://github.com/fs-os/fs-os/wiki) and open an issue if you
find wrong or incomplete information.

The goal of this project is not to create a user-friendly and secure system. The
goal is to create a playground for tinkering, where you have a lot of power and
a lot of things that can go wrong. As the project evolves, I would like that
tinkering to be more accessible at runtime, instead of having to edit the code
directly and re-compile the OS. Being able to edit code and assembly at runtime
(not necessarily the operating system's code) would be amazing.

An important part of this project, as the name implies, is to keep it very
simple and bloat-free. This is very important to understand how everything in
the project works, which is one of the main goals.

Some interesting resources that influenced the project's philosophy:
- [GNU philosophy](https://www.gnu.org/philosophy) (See [free](https://www.gnu.org/philosophy/free-sw.html)).
- [Suckless philosophy](https://suckless.org/philosophy).
- [cat-v.org](http://cat-v.org) and their [list of harmful stuff](http://harmful.cat-v.org).
- [TempleOS](https://templeos.org/) and Terry A. Davis.

### Building from source
#### Requirements
- i686-elf cross compiler from [here](https://github.com/fs-os/cross-compiler).
- [nasm](https://nasm.us) for compiling the assembly used in the project.
- The [limine](https://github.com/limine-bootloader/limine) dependencies,
  including [xorriso](https://www.gnu.org/software/xorriso) for creating the
  bootable image (Limine sources are already in the repo and only need to be
  compiled once).
- (Optional) [qemu](https://www.qemu.org) for testing the ISO on a VM.

> **Note**  
> Depending on the distro, you might also need the `qemu-ui-gtk` and
> `qemu-audio-pa` packages for the virtual machine.

#### Building
Once you have all the requirements, simply run:
```console
$ git clone https://github.com/fs-os/fs-os
$ cd fs-os
$ make
...
```

Which would be the same as the following:
```console
$ make limine   # Clone and build the bootloader inside ./limine/, you only need to do this once
...

$ make sysroot
...

$ make fs-os.iso
...
```

> **Note**  
> For more information about the building process and target dependencies, see the
> [compiling](https://github.com/fs-os/fs-os/wiki/Compiling#makefile-structure)
> section of the [wiki](https://github.com/fs-os/fs-os/wiki).

Or for testing it directly with qemu:
```console
$ make qemu
...
```

### Documentation
This project uses the [doxygen](https://github.com/doxygen/doxygen) tool to
generate its documentation.

#### Requirements
Package name (gentoo)            | Description
---------------------------------|----------------------------------------------
`app-doc/doxygen`                | Doxygen for building the documentation
`app-text/texlive`               | For building the documentation in LaTeX format (for pdf)
`media-gfx/graphviz`             | (Optional\*) For the graphs
`dev-texlive/texlive-latexextra` | (Optional) Only needed for building the documentation in pdf format

\* Needed if `HAVE_DOT=YES` in [Doxyfile](./Doxyfile) (The default).

#### Building the documentation
Simply run:
```console
$ doxygen
...

$ firefox doc/html/index.html
...
```

Generate PDF from LaTeX (Optional):
```console
$ cd doc/latex
$ make pdf
...

$ firefox refman.pdf
...
```

### Todo
See [todo.md](TODO.md) or the todo list of the doxygen documentation.

### Screenshots
> **Note**  
> Some screenshots might be a bit outdated.

![fs-os0](https://user-images.githubusercontent.com/29655971/269358077-593eb986-078f-46f3-b69b-cc8d3bd6a212.png)
![fs-os1](https://user-images.githubusercontent.com/29655971/269358675-487565a2-523c-49a4-816b-7c834a1121f3.png)

### Credits
- [OSDev wiki](https://wiki.osdev.org)
- [The limine bootloader](https://github.com/limine-bootloader/limine)
- [The cozette font](https://github.com/slavfox/Cozette)
- [bdf2c](https://github.com/pixelmatix/bdf2c)
- [This repo](https://github.com/ShaneMcC/beeps) for the beep frequencies and
  timings.
