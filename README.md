# PK-1000 Unwrap
pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.

![](https://github.com/kenwi/pku/blob/master/pk1000.jpg)

### Requirements 
pku needs libpthreads to compile
```bash
sudo apt install libpthread-stubs0-dev
```

### Installation
No magic
```bash
git clone https://github.com/kenwi/pku
cd pku
mkdir build
cd build
cmake ../
make
```

### Help
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku
pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.

Usage:	pku [-options] filename
	[-c connects with default settings]
	[-i print sample data]
	[-f print frame info]
	[-t test casting]
	[-n collect n samples and terminated]
	[-v verbose output]
	[-h host (default: 192.168.0.19)]
	[-p port (default: 8080)]
	filename (default: '-' dumps samples to stdout)
```

### Connect to 127.0.0.1, print status of 10 samples
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -h 127.0.0.1 -v -n 10
Settings changed, host: 127.0.0.1
Settings changed, verbose: 1
Settings changed, num_samples_terminate: 10
Connecting to PK-1000 system host: 127.0.0.1, port: 8080
2017-09-20, 14:33:17: Sample [1] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:17: Sample [2] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:17: Sample [3] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:17: Sample [4] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:17: Sample [5] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:17: Sample [6] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:17: Sample [7] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:18: Sample [8] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:18: Sample [9] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:18: Sample [10] received. length: 52 bytes, hex: 78, status: OK
2017-09-20, 14:33:18: Max number of samples collected: 10. Terminating.
```

### Connect to 127.0.0.1, print frame header and footer hex of 10 samples.
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -h 127.0.0.1 -n 10 -f
Settings changed, host: 127.0.0.1
Settings changed, num_samples_terminate: 10
Settings changed, print_sample_data: 1
Connecting to PK-1000 system host: 127.0.0.1, port: 8080
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Frame header: 3837, footer: 2827
2017-09-20, 14:36:23: Max number of samples collected: 10. Terminating.
```

### Internal testing of casting
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -t
pk1000 size = 52
buffer size = 52
frame = 14391

## pk1000 info ##
counts = 52
tag id = 0, x = 0, y = 0, z = 0

## Distances to tags ##
tag id = 1, distance = 6402
tag id = 2, distance = 9985
tag id = 3, distance = 14592
tag id = 0, distance = 35584

## Positions of anchors ##
tag id = 1, x = 0, y = 0, z = 0
tag id = 2, x = 0, y = 0, z = 0
tag id = 3, x = 0, y = 0, z = 0
tag id = 52, x = 10279, y = 0, z = 0
```
