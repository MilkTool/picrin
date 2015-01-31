<img width="500" src="https://raw.githubusercontent.com/picrin-scheme/picrin/master/etc/picrin-logo-fin01-02.png"></img>

[![Build Status](https://travis-ci.org/picrin-scheme/picrin.png)](https://travis-ci.org/picrin-scheme/picrin)
[![Docs Status](https://readthedocs.org/projects/picrin/badge/?version=latest)](https://picrin.readthedocs.org/)

Picrin is a lightweight scheme implementation intended to comply with full R7RS specification. Its code is written in pure C89 and does not require any special external libraries installed on the platform.

## Documentation

See http://picrin.readthedocs.org/

## Homepage

Currently picrin is hosted on Github. You can freely send a bug report or pull-request, and fork the repository.

https://github.com/picrin-scheme/picrin

## IRC

There is a chat room on chat.freenode.org, channel #picrin. IRC logs here: https://botbot.me/freenode/picrin/

## How to use it

To build picrin, you need some build tools installed on your platform.

- cmake (>= 2.6)


### Generate Makefile

Change directory to `build` then run `cmake` to create Makefile. Once `Makefile` is generated you can run `make` command to build picrin.

	$ cd build
	$ cmake ..

Actually you don't necessarily need to move to `build` directory before running `cmake` (in that case `$ cmake .`), but I strongly recommend to follow above instruction.
    
Before generating Makefile, you can change some compilation switches to enable or disable optional features. Take *NAN_BOXING* for example, when you turn on "Use C11 feature" flag and the platform supports addresses of 48bit length, it is enabled.

### Build

A built executable binary will be under bin/ directory and shared libraries under lib/.

	$ make

If you are building picrin on other systems than x86_64, PIC_NAN_BOXING flag is automatically turned on (see include/picrin/config.h for detail).

### Install

Just running `make install`, picrin library, headers, and runtime binary are install on your system, by default into `/usr/local` directory. You can change this value via cmake.

	$ make install

### Run

Before installing picrin, you can try picrin without breaking any of your system. Simply directly run the binary `bin/picrin` from terminal, or you can use `make` to execute it like this.

	$ make run

### Run Test
To run all the test including contribs, execute this.

    $ make test

To test only R7RS features,

    $ make test-r7rs

### Debug run

If you execute `cmake` with debug flag `-DCMAKE_BUILD_TYPE=Debug`, it builds the binary with all debug flags enabled (PIC_GC_STRESS, VM_DEBUG, DEBUG).

	$ cmake -DCMAKE_BUILD_TYPE=Debug ..
	
## Requirement

Picrin scheme depends on some external libraries to build the binary:

- perl
- libedit (optional)
- regex.h of POSIX.1 (optional)

Optional libraries are, if cmake detected them, automatically enabled.
The compilation is tested only on Mac OSX and Ubuntu. I think (or hope) it'll be ok to compile and run on other operating systems such as Arch or Windows, but I don't guarantee :(

## Authors

See `AUTHORS`
