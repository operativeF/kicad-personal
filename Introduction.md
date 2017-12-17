This is my personal clone of the KiCad project.
MSVC isn't officially supported by the KiCad dev team, so I've decided to fill in that gap. 
In the master branch of the project are changes that make it possible to compile
using MSVC. There are dependencies, however, that you will need in order to make compilation possible. I also have the preference
of building the dependencies, as it allows for finer control over what and how things get built, and bugfixes and improvements get integrated
quicker.

The tool I find easiest to use for gathering the dependencies with general ease is vcpkg (https://github.com/Microsoft/vcpkg/tree/master/ports)
Clone that and use it to compile the dependencies needed by KiCad. The libraries needed by KiCad are dynamic, which is the default setting.
I would not, however, choose to use the vcpkg version of wxWidgets. I've had luck building right from the master, or you can play it safe
and use the stable 3.0.* build. The master is totally capable and compatible, and I haven't had problems with it whatsoever.

Once you've managed to gather your dependencies, run CMake in the source root. I haven't had good luck with automatic detection with CMake
and vcpkg packages, but it's not that hard to just put the right paths it. I've tested extensively with VS 2017 x64 generated build solution
files, so YMMV with older version of VS.

There are a few notes when configuring the CMake options. The first is that there are flags that should be added to the CMAKE_CXX_FLAGS.
/DSIZEOF_WCHAR_T="2" -This is for wxWidgets and is specific to Windows
/J -The default char type is signed char; this flag changes it to unsigned char, which is what's used in KiCad
/MP -Multiple process compilation, of course you should use this

Performance flags aren't mandatory, but know that they are there:
/Oi - Generate intrinsic functions for appropriate function calls
/Ot - Favor code speed over size

There are others, and you may have to experiment a bit.

The second part of this configuration is that you should probably change the default KiCad directory to somewhere other than
Program Files. It's not going to install to Program Files unless you have write access. The name to change in the configuration
is DEFAULT_INSTALL_PATH.
