# amd_temp

A really simplistic AMD CPU Temperature driver for Haiku.

## Supported devices:

CPUs from family 10h to 16h (Athlon/Phenom to Jaguar/Puma).

Some 0Fh CPUs might be supported. No Ryzen support (for now?).

Check with `listdev` to see if your devices match one of the following:

VENDOR_ID= 0x1022

DEVICE_ID=
 - 0x1203
 - 0x1303
 - 0x1703
 - 0x1603
 - 0x1403
 - 0x1533
 - 0x1583

I've only tested it with  `DEVICE_ID = 0x1203` so far.

## Install instructions:

- Clone repo
- `make && make driverinstall`
- `cat /dev/sensor/amd_temp`

If all went well, you should see something like:

```sh
> cat /dev/sensor/amd_temp
AMD Thermal Diode reading: 45.0 C
```

If not, `tail -f /var/log/syslog` should at least show if something went wrong, like:

> KERN: amd_temp: amd_temp sensor not found.

## Note 1:

This is a work in progress. IOCTL's op codes and parameters will certainly change, so do not take them for granted.

Scanning/parsing  `/dev/sensor/amd_temp` for the temp value might prove to be a more stable "API", at least for now :-D.

## Note 2:

I'm using the old driver API here out of convenience. With the new driver API (at least for now) it seems you are forced to reboot to get your newly built binary to replace the one already loaded.

Much more convenient with the old driver API, you just replace the binary, and it loads your new code. I'll be using mostly the old API until the new API gains that feature.
