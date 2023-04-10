# fs-os
**Free &amp; Simple Operating System**

### Description
The main purpose of this project is to learn, so expect some mistakes. If you want to
contribute to the project, feel free to make a [pull request](https://github.com/fs-os/fs-os/pulls).

This project has a wiki explaining how the OS works from the start, so feel free to
check it out [here](https://github.com/fs-os/fs-os/wiki) and open an issue if you
find wrong or incomplete information.

An important part of this project, as the name implies, is to keep it as simple as
possible. This is very important to understand how everything in the project works
(*which is the main goal!*).

Some interesting resources that influenced the project's philosophy:
- [GNU philosophy](https://www.gnu.org/philosophy) (See [free](https://www.gnu.org/philosophy/free-sw.html)).
- [Suckless philosophy](https://suckless.org/philosophy).
- [cat-v.org](http://cat-v.org) and their [list of harmful stuff](http://harmful.cat-v.org).
- [TempleOS](https://templeos.org/) and Terry A. Davis.

### Building from source
#### Requisites
- i686-elf cross compiler from [here](https://github.com/fs-os/cross-compiler).
- [nasm](https://nasm.us) for compiling the assembly used in the project.
- The [limine](https://github.com/limine-bootloader/limine) dependencies, including
  [xorriso](https://www.gnu.org/software/xorriso) for creating the bootable image.
- (Optional) [qemu](https://www.qemu.org) for testing the ISO on a VM.

#### Building
Once you have all the requisites, simply run:
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
`doxygen`                        | Doxygen for building the documentation
`app-text/texlive`               | For building the documentation in LaTeX format (for pdf)
`media-gfx/graphviz`             | (Optional\*) For the graphs
`dev-texlive/texlive-latexextra` | (Optional) Only needed for building the documentation in pdf format

\* Needed if `HAVE_DOT=YES` in [Doxyfile](Doxyfile) (The default).

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

![fs-os0](https://user-images.githubusercontent.com/29655971/225747442-1821fdeb-4eba-44b9-98aa-35c65b797e64.png)
![fs-os1](https://user-images.githubusercontent.com/29655971/225747462-00290292-4d12-4f64-97ff-9c6f04e5713c.png)

### Credits
- [OSDev wiki](https://wiki.osdev.org)
- [The limine bootloader](https://github.com/limine-bootloader/limine)
- [The cozette font](https://github.com/slavfox/Cozette)
- [bdf2c](https://github.com/pixelmatix/bdf2c)
- [This repo](https://github.com/ShaneMcC/beeps) for the beep frequencies and
  timings.
- [heatd](https://github.com/heatd) for helping me understand some important concepts.
- [zid](https://github.com/zid) for helping me understand some important concepts.

