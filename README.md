## 5x5
**Simple CLI 5x5 game made in C.**

### Description
> **Note**
> This is a fork of [8dcc/5x5](https://github.com/8dcc/5x5)
> for [fs-os](https://github.com/fs-os/fs-os). You can try it there by running
> the `5x5` command.

This simple project was inspired by the `5x5` emacs command.

### Building

```console
$ git clone https://github.com/8dcc/5x5
$ cd 5x5
$ make
...
```

### Usage
```console
$ ./5x5.out --help
Usage:
    ./5x5.out                   - Launch with default resolution and scale
    ./5x5.out --help            - Show this help
    ./5x5.out -h                - Same as --help
    ./5x5.out --keys            - Show the controls
    ./5x5.out -k                - Same as --keys
    ./5x5.out --resolution WxH  - Launch with specified resolution (width, height)
    ./5x5.out -r WxH            - Same as --resolution
    ./5x5.out --scale VxH       - Launch with specified scale (vertical, horizontal)
    ./5x5.out -s VxH            - Same as --scale

$ ./5x5.out --keys
Controls:
    <arrows> - Move in the grid (unsupported)
        hjkl - Move in the grid (vim-like)
     <space> - Toggle selected cell (and adjacent)
           r - Generate random grid
           c - Generate a cheat grid
           q - Quit the game
```

