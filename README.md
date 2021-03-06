# Yggdrasil

This is an arbor chat client for the proof-of-concept protocol.

You can find information about the Arbor project [here](https://man.sr.ht/~whereswaldon/arborchat/)

## Status

Yggdrasil can connect to an arbor server, display recent messages, and reply to any message that it has recieved. It's not particularly elegant right now, but we hope to change that.

## Dependencies

Yggdrasil's Dependencies are all vendored with the exception of the C standard
library. Yggdrasil is developed against [MUSL Libc](http://www.musl-libc.org/), so the makefile tries to use that
by default. To build Yggdrasil, you'll either need to install the MUSL GCC wrapper
or change the value of `CC` in the makefile to `CC=gcc`.

### Notes on `json-parser`

This JSON-parsing library was chosen because of its impressively good results at
handling strange JSON objects in [this evaluation of JSON parsers](http://seriot.ch/parsing_json.php).

It is included in this repository as a git subtree.

You can find the canonical repo [here](https://github.com/udp/json-parser).

### Notes on `json-builder`

This JSON-serializng library was chosen because it played so nicely with the parser above.

It is included in this repository as a git subtree.

You can find the canonical repo [here](https://github.com/udp/json-builder).

### Notes on `c_hashmap`

This is a simple C implementation of a generic hashmap data structure. I've used it for other
projects and it seems good enough for my needs.

It is included in this repository as a git subtree.

You can find the canonical repo [here](https://github.com/petewarden/c_hashmap).

### Notes on `CuTest`

This C test framework was chosen for simplicity. I have vendored it without a subtree
or submodule because the original source is not managed with `git`.

You can find the canonical website for CuTest [here](http://cutest.sourceforge.net/).

## Build

Use `make` to build the `ygg` executable. `ygg` connects to an arbor server
on `localhost:7777`. This is currently hard-coded.

## Test

Run `make test` to build and run the tests.

## Use

If you have arbor running locally, just type `./ygg`.

If you want to connect to a remote server, `./ygg <IPv4_ADDR> <PORT>`.
