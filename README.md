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

Sample output
-------------

    >>> 06/09/2012 @ 16:44:17 : urxvt
    cd /tm⇥
    ls
    mkdit⌫r test

    >>> 06/09/2012 @ 16:44:56 : cyrus-and/xkeylogger - Google Chrome
    foo

    >>> 06/09/2012 @ 16:45:06 : Search · foo - Google Chrome
    ⌫⌫⌫bar←⌫⌦az

    >>> 06/09/2012 @ 16:45:28 : Search · baz - Google Chrome
    ⌫⌫⌫
    >>> 06/09/2012 @ 16:45:33 : Google - Google Chrome
    xkeylogger
