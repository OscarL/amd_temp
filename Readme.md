# amd_temp

A really simplistic AMD NorthBridge CPU Temperature driver for Haiku.

This basically reads register 0xA4 from PCI bus 0x00, device 0x18, function 0x03.

Reported values are NOT a physical representation of temperature, just a messure of how far is the CPU from needed as much cooling as it can get.

Values are in "degrees" that have some resemblance to Celcius, so we use that.

Supposedly, due to Errata #319, readings for some CPUs might be innacurate (this driver shows it anyway). My CPU should be affected, but readings are aligned with what I get from other sensors, so... YMMV.

Just in case, compare with the output from other drivers (acpi_thermal, it87xx, or what your BIOS HW monitoring shows).

## Supported devices:

CPUs from family 10h to 16h (K10/Athlon/Phenom to Jaguar/Puma).

Some 0Fh (K8/Athlon64/Opteron) CPUs might be supported.

No Ryzen support (for now?).

Check with `listdev` to see if your devices match one of the following:

VENDOR_ID= 0x1022

DEVICE_ID=
 - 0x1103 - K8 / Athlon64, Opteron
 - 0x1203 - K10 / Athlon/Athlon II, Phenom/Phenom II
 - 0x1303 - K11 / Turion X2 Ultra
 - 0x1703 - F.12h F.14h - Llano/Bobcat APUs (eg AMD A6-3650, C-60, E-350)
 - 0x1603 - F.15h - Jaguar
 - 0x1403 - F.15h (Models 10h-1fh)
 - 0x141d - F.15h (Models 30h-3fh)
 - 0x1533 - F.16h - Puma
 - 0x1583 - F.16h (Models 30h-3fh)

So far, tested with the following DEVICE_IDs:

 - 0x1203 (Phenom II X4 925)
 - 0x1533 (AMD GX-212JC SoC)
 - 0x1583 (AMD E1-2100 APU)

(a.k.a. you can reasonably expect the driver to NOT set your computer on fire, for these CPUs at least :-P)

## Install instructions:

- Clone repo
- `make && make driverinstall`
- `cat /dev/sensor/amd_temp`

If all went well, you should see something like:

```sh
> cat /dev/sensor/amd_temp
AMD Thermal Reading: 45.0 C
```

If not, `tail -f /var/log/syslog` should at least show if something went wrong, like:

> KERN: amd_temp: sensor not found.

## Note 1:

This is a work in progress. IOCTL's op codes and parameters will certainly change, so do not take them for granted.

Scanning/parsing  `/dev/sensor/amd_temp` for the temp value might prove to be a more stable "API", at least for now :-D.

## Note 2:

I'm using the old driver API here out of convenience. With the new driver API (at least for now) it seems you are forced to reboot to get your newly built binary to replace the one already loaded.

Much more convenient with the old driver API, you just replace the binary, and it loads your new code. I'll be using mostly the old API until the new API gains that feature.


### Similar (and more complete!) drivers in other OSes:

- FreeBSD's [amdtemp.c](https://github.com/freebsd/freebsd-src/blob/main/sys/dev/amdtemp/amdtemp.c)
- Illumos's [amdnbtemp.c](https://github.com/illumos/illumos-gate/blob/master/usr/src/uts/intel/io/amdnbtemp/amdnbtemp.c)
- Linux's [k10temp.c](https://github.com/torvalds/linux/blob/master/drivers/hwmon/k10temp.c)
- OpenBSD's [km.c](https://github.com/openbsd/src/blob/master/sys/dev/pci/km.c)

For K8:

- Linux's [k8temp.c](https://github.com/torvalds/linux/blob/master/drivers/hwmon/k8temp.c)
- OpenBSD's [kate.c](https://github.com/openbsd/src/blob/master/sys/dev/pci/kate.c)

For Ryzen (and some 15h):

-OpenBSD's [ksmn.c](https://github.com/openbsd/src/blob/master/sys/dev/pci/ksmn.c)
