# Winbond W83792D: +5V Vcc is incorrect

Currently, boards which use the Winbond W83792D H/W monitoring IC will
have their +5V voltage shown incorrectly; the Supermicro P8SC8 and
P8SCT are such examples.

I've mailed Supermicro to try and find out why the calculation formula
is wrong (since what I'm following comes from Winbond), but as of this
writing have received no response.

I have also looked at the Linux lm-sensors project, but the code is
quite "spaghetti" -- it's hard to discern what the calculation values
are, and if they're the same for all W83792D systems.

# Winbond W83792D: FAN3 RPMs may be inaccurate/high

I've received a single (isolated) report involving a Supermicro P8SCi
board reporting abnormally high values for FAN3.  Example:

```
FAN1                        0 RPM
FAN2                     2909 RPM
FAN3                    84375 RPM
FAN4                        0 RPM
FAN5                        0 RPM
```

Further executions of bsdhwmon did not exhibit this problem.  I take 
the report seriously, as it could indicate a strange bug in bsdhwmon,
or possibly a bug in the Winbond W83792D chipset.

At of this writing I have not been able to determine the root cause
The user had his fan RPM configuration in the system BIOS set to "3-pin
Server" rather than "Disabled" (latter runs the fans at full speed).

It's also possible that
[commit ad3bbad](https://github.com/koitsu/bsdhwmon/commit/ad3bbad9980297392773a5bd3e848772b6e85e0d)
rectified the problem, citing 16-bit calculation overflow issues.

