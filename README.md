# bsdhwmon

bsdhwmon is a program for FreeBSD for monitoring hardware sensors (fans, temperatures, voltages, chassis intrusion, and more) on commerical-grade server hardware.

bsdhwmon is developed with a very different mentality compared to other hardware monitoring software:

* Written with stability and production environments in mind
* Intended for use with server products (currently Supermicro, but can be extended to others that have proper SMBIOS identification data)
* Uses SMBus ([smb(4)](https://www.freebsd.org/cgi/man.cgi?query=smb&apropos=0&sektion=0&manpath=FreeBSD+11.1-stable&arch=default&format=html) driver) exclusively, significantly decreasing risks and CPU usage compared to classic LPC I/O
* Based primarily on documentation provided by motherboard/server vendors, combined with documentation from chipset manufacturers
* Identifies hardware via strict SMBIOS data matching; device "probing" is avoided to minimise false positives and thus risks
* Full tested on both i386 and amd64 systems across multiple versions of FreeBSD (legacy and present-day)
* Written entirely in C
* No reliance on third-party libraries or tools (e.g. autotools, libtool, etc.)
* Very simple and clean code -- heavily commented, well-documented, built with <code>-Werror -Wall</code> and similar flags
* Occasionally tested under valgrind

## Supported Hardware
At this time, only a small set of Supermicro hardware is supported.  For an official list of supported hardware and models, please see [doc/supported.md](/doc/supported.md).

## Supported Operating Systems
* FreeBSD 12.x amd64
  * [![12.1-RELEASE Build Status](https://api.cirrus-ci.com/github/koitsu/bsdhwmon.svg?task=releng_12_1)](https://cirrus-ci.com/github/koitsu/bsdhwmon)
* FreeBSD 11.x amd64
  * [![11.4-RELEASE Build Status](https://api.cirrus-ci.com/github/koitsu/bsdhwmon.svg?task=releng_11_4)](https://cirrus-ci.com/github/koitsu/bsdhwmon)
  [![11.3-RELEASE Build Status](https://api.cirrus-ci.com/github/koitsu/bsdhwmon.svg?task=releng_11_3)](https://cirrus-ci.com/github/koitsu/bsdhwmon)

## Unsupported Operating Systems
* FreeBSD 13.x &mdash; untested (may work, give it a try)
* FreeBSD 12.0, 11.2, 11.1, 11.0, 10.x &mdash; officially EoL'd but likely will work; give it a try!
* FreeBSD 9.x, 8.x, 7.x, 6.x &mdash; deprecated

## Usage
Please see the [bsdhwmon man page](/bsdhwmon.8.txt) for all command-line flags and usage details.

## Support Requests
For bug reports, enhancements, or general support, please use the [GitHub Issues](https://github.com/koitsu/bsdhwmon/issues) interface.

## Author
* Jeremy Chadwick &lt;jdc@koitsu.org&gt;

## Repositories
* Source: https://github.com/koitsu/bsdhwmon
* Distribution: https://github.com/koitsu/bsdhwmon/releases
* Distribution (20150429 and older): https://bsdhwmon.koitsu.org/archive/

