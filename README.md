xkeylogger
==========

Simple keylogger for X.

Features
--------

 - No root access required.

 - Keep track of the focused window and print its name too.

 - Translate raw keystrokes according to the current keyboard layout.

 - Also log some useful non-printable keystrokes (backspace, tab, etc.).

 - Use the modifiers to write the proper glyph.

Dependencies
------------

    libxi-dev

Build
-----

    make

Usage
-----

Dump keystrokes to a file (exit by killing the process):

    xkeylogger > key.log

Sample output
-------------

    [+] 06/09/2012 @ 16:44:17 : urxvt
    cd /tm⇥⏎
    ls⏎
    mkdit⌫r test⏎

    [+] 06/09/2012 @ 16:44:56 : cyrus-and/xkeylogger - Google Chrome
    foo⏎

    [+] 06/09/2012 @ 16:45:06 : Search · foo - Google Chrome
    ⌫⌫⌫bar←⌫⌦az⏎

    [+] 06/09/2012 @ 16:45:28 : Search · baz - Google Chrome
    ⌫⌫⌫
    [+] 06/09/2012 @ 16:45:33 : Google - Google Chrome
    xkeylogger⏎
