# Yggdrasil

This is an arbor chat client. As [arbor](https://github.com/whereswaldon/arbor) is an experimental chat protocol,
this client is pretty experimental itself. It has the goal of being
simple (in use and implementation), light, and reliable.

## Status

Yggdrasil just connects to the server and spits out a copy of the welcome message
for the time being. It's not useful yet.

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
