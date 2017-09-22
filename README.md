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
	[-r print sample hex]
	[-i print sample data]
	[-n collect n samples and terminate]
	[-v verbose output]
	[-h host (default: 192.168.0.19)]
	[-p port (default: 8080)]
	filename (default: '-' dumps samples to stdout)
```

### Connect to 127.0.0.1, print hex of 3 samples
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -h 127.0.0.1 -r -n 3
Settings changed, host: 127.0.0.1
Settings changed, print_raw_sample: 1
Settings changed, num_samples_terminate: 3
Connecting to PK-1000 system host: 127.0.0.1, port: 8080
2017-09-22, 10:46:34: 3738 0000 7a00 fffffffe00 7d00 0114 0100 ffffffc202 006b 0301 3600 012c 0000 0032 0101 2c01 2c00 3202 0000 012c 0032 0300 0000 0000 327b 2728 
2017-09-22, 10:46:34: 3738 0000 7a00 ffffffff00 7d00 0117 0100 ffffffc402 006a 0301 3d00 012c 0000 0032 0101 2c01 2c00 3202 0000 012c 0032 0300 0000 0000 327c 2728 
2017-09-22, 10:46:34: 3738 0000 7b00 fffffffe00 7d00 0115 0100 ffffffc202 006f 0301 3c00 012c 0000 0032 0101 2c01 2c00 3202 0000 012c 0032 0300 0000 0000 327d 2728 
2017-09-22, 10:46:34: Max number of samples collected: 3. Terminating.
```

### Connect to 127.0.0.1, print data of 3 samples
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -h 127.0.0.1 -i -n 3
Settings changed, host: 127.0.0.1
Settings changed, print_sample_data: 1
Settings changed, num_samples_terminate: 3
Connecting to PK-1000 system host: 127.0.0.1, port: 8080
		Tag0 		Anc0		Anc1		Anc2		Anc3
Range(cm)			276		190		107		315
X(cm)		124		300		300		0		0
Y(cm)		255		0		300		300		0
Z(cm)		125		50		50		50		50

		Tag0 		Anc0		Anc1		Anc2		Anc3
Range(cm)			277		194		106		317
X(cm)		123		300		300		0		0
Y(cm)		255		0		300		300		0
Z(cm)		125		50		50		50		50

		Tag0 		Anc0		Anc1		Anc2		Anc3
Range(cm)			277		193		105		312
X(cm)		123		300		300		0		0
Y(cm)		255		0		300		300		0
Z(cm)		125		50		50		50		50

2017-09-22, 10:46:46: Max number of samples collected: 3. Terminating.
```

### Verbose output will print status of received sample, number collected and the two first bytes ###
```bash
kenwi@wardenclyffe:~/git/pku/build$ ./pku -h 127.0.0.1 -v -n 3
Settings changed, host: 127.0.0.1
Settings changed, verbose: 1
Settings changed, num_samples_terminate: 3
Connecting to PK-1000 system host: 127.0.0.1, port: 8080
2017-09-22, 10:48:31: Sample [1] received. length: 52 bytes, hex 0: 37, hex 1: 38 status: OK
2017-09-22, 10:48:31: Sample [2] received. length: 52 bytes, hex 0: 37, hex 1: 38 status: OK
2017-09-22, 10:48:32: Sample [3] received. length: 52 bytes, hex 0: 37, hex 1: 38 status: OK
2017-09-22, 10:48:32: Max number of samples collected: 3. Terminating.
```
