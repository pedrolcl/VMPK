% VMPK(1) vmpk 0.0.0 | Virtual MIDI Piano Keyboard
% Pedro López-Cabanillas <plcl@users.sf.net>

# NAME

**vmpk** - Virtual MIDI Piano Keyboard

# SYNOPSIS

**vmpk** \[options\...\]

# DESCRIPTION

This manual page documents briefly the **vmpk** program.

This program uses standard Qt options.

Virtual MIDI Piano Keyboard is a MIDI events generator and receiver. It
doesnt produce any sound by itself, but can be used to drive a MIDI
synthesizer (either hardware or software, internal or external). You can
use the computers keyboard to play MIDI notes, and also the mouse. You
can use the Virtual MIDI Piano Keyboard to display the played MIDI notes
from another instrument or MIDI file player.

# VMPK OPTIONS

The following options apply to **vmpk** only:

**-h\|\--help**

> Displays a help text.

**-v\|\--version**

> Displays version information.

**-p\|\--portable**

> Enables portable settings mode using a default configuration file
> name.

**-f** *path/file.conf*

> This option implies **\--portable** as well. Reads and writes
> *path/file.conf* as the portable settings configuration file name.
>
> This configuration file is always a text file with INI format, despite
> the running Operating System conventions.

# STANDARD OPTIONS

The following options apply to all Qt5/6 applications:

**-style=** *style* / **-style** *style*

> Set the application GUI style. Possible values depend on the system
> configuration. If Qt is compiled with additional styles or has
> additional styles as plugins these will be available to the **-style**
> command line option.

**-stylesheet=** *stylesheet* / **-stylesheet** *stylesheet*

> Set the application styleSheet. The value must be a path to a file
> that contains the Style Sheet.

**-widgetcount**

> Print debug message at the end about number of widgets left
> undestroyed and maximum number of widgets existed at the same time.

**-reverse**

> Set the applications layout direction to Qt::RightToLeft. This option
> is intended to aid debugging and should not be used in production. The
> default value is automatically detected from the users locale (see
> also QLocale::textDirection()).

**-platform** *platformName\[:options\]*

> Specify the Qt Platform Abstraction (QPA) plugin.

**-platformpluginpath** *path*

> Specify the path to platform plugins.

**-platformtheme** *platformTheme*

> Specify the platform theme.

**-plugin** *plugin*

> Specify additional plugins to load. The argument may appear multiple
> times.

**-qwindowgeometry** *geometry*

> Specify the window geometry for the main window using the X11-syntax.
> For example: -qwindowgeometry 100x100+50+50

**-qwindowicon** *icon*

> Set the default window icon.

**-qwindowtitle** *title*

> Set the title of the first window.

**-session** *session*

> Restore the application from an earlier session.

**-display** *hostname:screen_number*

> Switch displays on X11. Overrides the **DISPLAY** environment
> variable.

**-geometry** *geometry*

> Specify the window geometry for the main window on X11. For example:
> -geometry 100x100+50+50

**-dialogs=** *\[xp\|none\]*

> Only available for the Windows platform. XP uses native style dialogs
> and none disables them.

**-fontengine=** *freetype*

> Use the FreeType font engine.

# SEE ALSO

qt6options (7)

# LICENSE

This manual page was originally written by Mehdi Dogguy
\<dogguy@pps.jussieu.fr\> for the Debian GNU/Linux system (but may be
used by others).

Updated for Qt5/6 by Pedro Lopez-Cabanillas
\<plcl@users.sourceforge.net\>

Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU General Public License, Version 3 or any
later version published by the Free Software Foundation; considering as
source code all the file that enable the production of this manpage.

# COPYRIGHT

Copyright © 2009 Mehdi Dogguy\
Copyright © 2010-2026 Pedro Lopez-Cabanillas\

