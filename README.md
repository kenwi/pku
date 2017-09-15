# PK-1000 Unwrap
pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.

[logo]: https://github.com/kenwi/pku/blob/master/pk1000.jpg

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
	[-i only print sample info]
	[-t test casting]
	[-h host (default: 192.168.0.19)]
	[-p port (default: 8080)]
	filename (default: '-' dumps samples to stdout)
```

### Connect with default settings, print status of samples
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -c -i
Connecting to PK-1000 system by IP-address 192.168.0.19
Sample received. length: 52 bytes, hex: 78, status: OK
Sample received. length: 52 bytes, hex: 78, status: OK
Sample received. length: 52 bytes, hex: 78, status: OK
...
...
...
Sample received. length: 104 bytes, hex: 78, status: BAD
Sample received. length: 52 bytes, hex: 78, status: OK
Sample received. length: 52 bytes, hex: 78, status: OK
Sample received. length: 52 bytes, hex: 78, status: OK
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
