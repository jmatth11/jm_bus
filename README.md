# JM_BUS

This is just an experimental project to try and make my own message bus in C.

The point of this project was to have a simple and minimal message bus you could
spin up as server inside your program and/or connect to from a client program.

The project contains 3 programs and a shared library.

`server/` is where a default server implmentation is.

`client/` holds a simple `pub` and `sub` implementation.

`common/` is where the bulk of the logic is and where the shared library is generated.

## Requirements

I've only tested on Ubuntu currently.

- -std=C11
- `https://github.com/jmatth11/array_template.git` <- clone inside `deps/`

## Building

### Common Library

You need to build `common/` first.

command:
`make archive`

### Programs (optional)

For all three programs `server/`, `client/pub`, and `client/sub` just run the
`make` command in their respective directories.


## Running

There is a convenient `run.sh` in each of the programs' folders.

This sets up the `LD_LIBRARY_PATH` and runs the program.

Run the `server/` first, then the `client/sub/`, and then the `client/pub/`.

## Demo

https://github.com/user-attachments/assets/d4dc799d-a76d-41d7-b152-09945b17e3f6
