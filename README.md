xkeylogger - Simple keylogger for X
===================================

xkeylogger just listen for keystrokes and dump the log to standard output.

Features
--------

 - no root access required

 - keep track of the focused window and dump its name too

 - translate raw keystrokes according to the current keyboard layout

 - log some useful non-printable keystrokes too (backspace, tab, etc.)

 - use the modifiers (shift, control, etc.) to dump the proper glyph

Dependencies
------------

    libxi-dev

Build
-----

    $ make

Usage
-----

Dump keystrokes to a file (exit with `Control-C` or `SIGTERM`):

    $ xkeylogger > key.log
