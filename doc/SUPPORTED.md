# Supported Hardware

Board/System          | Maker         | Product               | Chip
--------------------- | ------------- | --------------------- | ---------------
Supermicro P8SC8      | Supermicro    | P8SC8                 | Winbond W83792D
Supermicro P8SCT      | Supermicro    | P8SCT                 | Winbond W83792D
Supermicro X6DHP-8G2  | Supermicro    | X6DHR-8G2/X6DHR-TG    | Winbond W83792D
Supermicro X6DHR-8G2  | Supermicro    | X6DHR-8G2/X6DHR-TG    | Winbond W83792D
Supermicro X6DHR-TG   | Supermicro    | X6DHR-8G2/X6DHR-TG    | Winbond W83792D
Supermicro PDSMA+     | Supermicro    | PDSMA+                | Winbond W83793G
Supermicro PDSMi+     | Supermicro    | PDSMi+                | Winbond W83793G
Supermicro PDSMU      | Supermicro    | PDSMU                 | Winbond W83793G
Supermicro X6DVA      | Supermicro    | X6DVA                 | Custom
Supermicro X6DVL      | Supermicro    | X6DVA                 | Custom
Supermicro X6DAL      | Supermicro    | X6DVA                 | Custom
Supermicro X7DB8      | Supermicro    | X7DB8                 | Winbond W83793G
Supermicro X7DB8+     | Supermicro    | X7DB8                 | Winbond W83793G
Supermicro X7DBE      | Supermicro    | X7DB8                 | Winbond W83793G
Supermicro X7DBE+     | Supermicro    | X7DB8                 | Winbond W83793G
Supermicro X7DCL      | Supermicro    | X7DCL                 | Winbond W83793G
Supermicro X7DVL      | Supermicro    | X7DVL                 | Winbond W83793G
Supermicro X7DVL-3    | Supermicro    | X7DVL-3               | Winbond W83793G
Supermicro X7DBP-8    | Supermicro    | X7DBP                 | Winbond W83793G
Supermicro X7DBP-i    | Supermicro    | X7DBP                 | Winbond W83793G
Supermicro X7DBT      | Supermicro    | X7DBT                 | Winbond W83793G
Supermicro X7SB4      | Supermicro    | X7SB4/E               | Winbond W83793G
Supermicro X7SBA      | Supermicro    | X7SBA                 | Winbond W83793G
Supermicro X7SBE      | Supermicro    | X7SB4/E               | Winbond W83793G
Supermicro X7SBi      | Supermicro    | X7SBi                 | Winbond W83793G


# Unsupported Hardware

Board/System           | Reason
---------------------- | -----------------------------------------------------
Celestix MSA 2000i     | Lacks SMBus interface; LPC I/O only
Supermicro 370SSE      | No hardware monitoring IC available
Supermicro C2SBA       | Lacks SMBus interface; LPC I/O only
Supermicro C2SBA+      | Lacks SMBus interface; LPC I/O only
Supermicro P4LDR       | FreeBSD lacks ServerWorks GC-LE SMBus driver
Supermicro P4SCE       | Lacks SMBus interface; LPC I/O only
Supermicro P4SCI       | Lacks SMBus interface; LPC I/O only
Supermicro P4SCT       | Lacks SMBus interface; LPC I/O only
Supermicro PDSBM-LN1   | Lacks SMBus interface; LPC I/O only
Supermicro PDSBM-LN2   | Lacks SMBus interface; LPC I/O only
Supermicro PDSBM-LN2+  | Lacks SMBus interface; LPC I/O only
Tyan S2932             | Lack of technical details from vendor; see https://github.com/koitsu/bsdhwmon/issues/2


# Under Investigation

bsdhwmon currently does not support the below motherboards/systems,
but investigative efforts are underway to see if support can be
added.

Board/System          | Maker         | Product               | Chip
--------------------- | ------------- | --------------------- | -----------------------
Asrock EP2C602-4L/D16 | Unknown       | Unknown               | Nuvoton; see https://github.com/koitsu/bsdhwmon/issues/4
IBM eServer 306m      | IBM           | M11ip/M11ix           | Unknown
IBM System x3250      | IBM           | M31ip                 | Unknown

# Notes

* "Maker" refers to the [kenv(1)](https://www.freebsd.org/cgi/man.cgi?kenv) string `smbios.planar.maker`
* "Product" refers to the [kenv(1)](https://www.freebsd.org/cgi/man.cgi?kenv) string `smbios.planar.product`

