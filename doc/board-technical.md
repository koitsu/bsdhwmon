# Supermicro

## Preface

The below is documentation sent to me verbatim by Supermicro Technical
Support via Email.  I'd like to point out that their Support staff is
incredibly helpful and receptive to requests for technical details of
this nature, which is more than I can say for some other companies.

I used to have a substantially larger amount of information on many
other boards that bsdhwmon supports (i.e. ones not listed below).  That
information was lost in a ZFS-related mishap.  At the time, I did not
have working backups, and I wasn't using a public repository for version
control (I was using CVS locally stored on ZFS).  I may be able to
re-obtain this information from Supermicro, but I'm not sure what their
retention policies are on documentation for EoL'd boards.

I've done my best to retain the raw formatting of the information
provided, where applicable.

## C2G41
- SMBus slave address 0x2d
- ITE IT8720F-HX

## C2SEA
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## C2SEE
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## X7SBL-LN2
```
The X7SBL-LN2 motherboard embedded with Winbond 83627HG version not 8327HG-AW chipset.

Here is the sensor information about Winbond chipset.

Bus Type = SMBus
One W83793G

Windbond W83793G, Slave Address=0x2f (0x5E in 8-Bit format)
=============================================================
Fan1 Fan Speed, Offset 0x25, 0x26               RPM = 1350000/Data
Fan2 Fan Speed, Offset 0x23, 0x24               RPM = 1350000/Data
Fan3 Fan Speed, Offset 0x27, 0x28               RPM = 1350000/Data
Fan4 Fan Speed, Offset 0x29, 0x2a               RPM = 1350000/Data
Fan5 Fan Speed, Offset 0x2b, 0x2c               RPM = 1350000/Data
Fan6/CPU Fan Speed, Offset 0x2f, 0x30           RPM = 1350000/Data
CPU Core Voltage, Offset 0x10                   Voltage = Data* 0.008
-12V Voltage, Offset 0x14                       Voltage = ((Data*0.016)- (2.048*(232./260.)))/(1-(232./260.))
DIMM Voltage, Offset 0x15                       Voltage = Data* 0.016
+3.3V Voltage, Offset 0x16                      Voltage = Data* 0.016
+12V Voltage, Offset 0x17                       Voltage = Data* 0.008/ (10./120.)
+5V Voltage, Offset 0x18                        Voltage = Data* 0.024
5Vsb Voltage, Offset 0x19                       Voltage = Data* 0.024
Battery Voltage, Offset 0x1a                    Voltage = Data* 0.016
CPU Temperature, Offset 0x1c                    Temperature = Data
System Temperature, Offset 0x21                 Temperature = Data
Chassis Intrusion, Offset 0x44, BitMask 0x40    1 = Bad, 0 = Good
Power Supply Failure, GP11(From W83627HF)       1 = Good, 0 = Bad
```

## X7SLM
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## X7SLM+
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## X7SLM-L
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## X7SB3-F
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## X8SI6, X8SIE, X8SIL
- SMBus slave address 0x2d
- Winbond W83627DHG-P

## X8STI
- SMBus slave address 0x2e

