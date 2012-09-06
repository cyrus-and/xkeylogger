xkeylogger - Simple keylogger for X
===================================

xkeylogger intercepts human readable keyboard events (and also some useful
special keys like backspace) and dumps the resulting translated characters to
standard output; it also keeps track of the currently focused window.

Dependencies
------------

    libxi-dev

Build
-----

    $ make

Usage
-----

Dump keystrokes to a file (quit with `Control-C` or `SIGTERM`):

    $ xkeylogger > key.log
