# Supermicro

## End-of-Life (EoL) Notice

As of at least August 2018, Supermicro is no longer providing documentation for
products which are EoL'd (end-of-life); refer to
[issues/6](https://github.com/koitsu/bsdhwmon/issues/6#issuecomment-412623640)
for verification.

## Preface

The below is documentation sent to me verbatim by Supermicro Technical Support
via Email.  I'd like to point out that their Support staff is incredibly
helpful and receptive to requests for technical details of this nature, which
is more than I can say for some other companies.

I used to have a substantially larger amount of information on many other
boards that bsdhwmon supports (i.e. ones not listed below).  That information
was lost in a ZFS-related mishap.  At the time, I did not have working backups,
and I wasn't using a public repository for version control (I was using CVS
locally stored on ZFS).

I've done my best to retain the raw formatting of the information provided,
where applicable.

## C2G41
- ITE IT8720F-HX, slave address 0x2d

## C2SEA
- Winbond W83627DHG-P, slave address 0x2d

## C2SEE
- Winbond W83627DHG-P, slave address 0x2d

## P8SC8, P8SCT, P8SCi

- Winbond W83792D, slave address 0x2f
- See [doc/bugs.md](/doc/bugs.md) for issues relating to voltage values and fan RPMs

## PDSMA+

- Winbond W83793G, slave address 0x2f

## PDSMi+

- Winbond W83793G, slave address 0x2f

## PDSMU

- Winbond W83793G, slave address 0x2f

## X6DHR-8G2, X6DHR-TG

- Winbond W83792D, slave address 0x2f
- See [doc/bugs.md](/doc/bugs.md) for issues relating to fan RPMs

## X6DVA-4G2, X6DVA-4G, X6DVA-EG2, X6DVA-EG

There are physically two chips on these boards:

- Winbond W83627HF, slave address 0x2c
- Winbond W83792D, slave address 0x2f
- See [doc/bugs.md](/doc/bugs.md) for issues relating to fan RPMs

Unlike the later X7SBL series, Supermicro chose to make use of the monitoring
capabilities of both chips.  Register details are below, provided by Supermicro
Technical Support.

"CR" stands for "Control Register", i.e. register offset.

For further details of register decoding and quirks, refer to the comments
in the source code file [chip\_x6dva.c](/chip_x6dva.c).

```
Chip        Indexes     Description
-------------------------------------------------------
W83627HF    CR20        +1.5V
W83627HF    CR21        +3.3VSB (Standby)
W83627HF    CR22        +3.3V
W83627HF    CR23        +5V
W83627HF    CR24        +12V
W83627HF    CR25        -12V
W83627HF    CR27        Sys Temp
-------------------------------------------------------
W83792D     CR20,CR3E   CPU1 Vcore
W83792D     CR21,CR3E   CPU2 Vcore
W83792D     CR28,CR47   FAN1
W83792D     CR29,CR47   FAN2
W83792D     CR2A,CR5B   FAN3
W83792D     CRB8,CR5B   FAN4
W83792D     CRB9,CR5C   FAN5
W83792D     CRBA,CR5C   FAN6
W83792D     CRC0,CRC1   CPU Temp 1
W83792D     CRC8,CRC9   CPU Temp 2
```

## X7SBL, X7SBL-LN1, X7SBL-LN2

There are physically two chips on these boards: a Winbond W83627HF-AW and a
Winbond W83793G.

- Winbond W83627HF-AW, slave address 0x2d: used solely for Super I/O capabilities
- Winbond W83793G, slave address 0x2f: used solely for H/W monitoring

Supermicro's website states that the board contains a W83627HF, which is
incorrect -- it truly is a W83627HF-AW.

Below is the technical documentation sent to me by Supermicro, pertaining to
the W83793G used for hardware monitoring:

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

Board photos
[are available](https://plone.lucidsolutions.co.nz/hardware/sensors/supermicro-x7sbl-ln2-sensors)
while claiming "getting technical information regarding the sensors from
Supermicro is difficult" -- I had no such difficulty; asking via Email
was sufficient.

## X7SLM
- Winbond W83627DHG-P, slave address 0x2d

## X7SLM+
- Winbond W83627DHG-P, slave address 0x2d

## X7SLM-L
- Winbond W83627DHG-P, slave address 0x2d

## X7SB3-F
- Winbond W83627DHG-P, slave address 0x2d

## X8SI6, X8SIE, X8SIL
- Winbond W83627DHG-P, slave address 0x2d

## X8STI
- Winbond W83627DHG, slave address 0x2e

