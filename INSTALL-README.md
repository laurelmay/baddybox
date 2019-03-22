# Busybox

BusyBox combines tiny versions of many common UNIX utilities into a single
small executable. It provides replacements for most of the utilities you
usually find in GNU fileutils, shellutils, etc. The utilities in BusyBox
generally have fewer options than their full-featured GNU cousins; however, the
options that are included provide the expected functionality and behave very
much like their GNU counterparts. BusyBox provides a fairly complete
environment for any small or embedded system.

BusyBox has been written with size-optimization and limited resources in mind.
It is also extremely modular so you can easily include or exclude commands (or
features) at compile time. This makes it easy to customize your embedded
systems. To create a working system, just add some device nodes in /dev, a few
configuration files in /etc, and a Linux kernel.

Due to configuration issues on some Linux distros, the tools in this directory
require the `setuid` bit set, which may cause them to appear red in `ls` output.

BusyBox is maintained by Denys Vlasenko, and licensed under the GNU GENERAL
PUBLIC LICENSE version 2.
