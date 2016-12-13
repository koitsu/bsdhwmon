# bsdhwmon
bsdhwmon is a program for FreeBSD for monitoring hardware sensors (fans, temperatures, voltages, chassis intrusion, and more) on commerical-grade server hardware.

bsdhwmon is developed with a very different mentality compared to other hardware monitoring software:

* Written with stability and production environments in mind
* Intended for use with server products (currently Supermicro, but can be extended to others that have proper SMBIOS identification data)
* Uses SMBus ([smb(4)](https://www.freebsd.org/cgi/man.cgi?query=smb&apropos=0&sektion=0&manpath=FreeBSD+9.3-stable&arch=default&format=html) driver) exclusively, significantly decreasing risks and CPU usage compared to classic LPC I/O
* Based primarily on documentation provided by motherboard/server vendors, combined with documentation from chipset manufacturers
* Identifies hardware via strict SMBIOS data matching; device "probing" is avoided to minimise false positives and thus risks
* Full tested on both i386 and amd64 systems across multiple versions of FreeBSD (legacy and present-day)
* Written entirely in C
* No reliance on third-party libraries or tools (e.g. autotools, libtool, etc.)
* Very simple and clean code -- heavily commented, well-documented, built with <code>-Werror -Wall</code> and similar flags
* Occasionally tested under valgrind
* Open-source, released under the 2-clause BSD license

## Supported Hardware
At this time, only a subset of Supermicro hardware is supported.  For an official list of supported hardware and models, please see the [doc/SUPPORTED](doc/SUPPORTED) file.

## Supported Operating Systems
* FreeBSD 8.x (stable/8)
* FreeBSD 9.x (stable/9)

## Unsupported/Untested Operating Systems
* FreeBSD 10.x (stable/10)
* FreeBSD 11.x (stable/11)
* FreeBSD 12.x (head/current)

## Deprecated Operating Systems
* FreeBSD 6.x (stable/6)
* FreeBSD 7.x (stable/7)

## Usage
Please see the [bsdhwmon man page](bsdhwmon.8.txt) for all command-line flags and usage details.

## Support Requests
For bug reports, enhancements, or general support, please use the [GitHub Issues](https://github.com/koitsu/bsdhwmon/issues) interface.

## Author
* Jeremy Chadwick &lt;jdc@koitsu.org&gt;

## Repositories
* Source: https://github.com/koitsu/bsdhwmon
* Distribution: https://github.com/koitsu/bsdhwmon/releases
* Distribution (20150429 and older): http://jdc.koitsu.org/freebsd/bsdhwmon-archive/

