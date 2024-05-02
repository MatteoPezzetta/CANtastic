### Setting up virtual can interface
To setup the virtual can interface, launch the script `setup_vcan.sh`
Then do:
```
chmod +x setup_vcan.sh
sudo ./setup_vcan.sh
```

### Build binaries for reading and writing
To build the two executables for testing the reading and writing from can and can-fd:
```
chmod +x build_lib.sh
./build_lib.sh
```

### Use binaries
You can launch the reader and writer programs in two different shells.
First, make sure you have set up the virtual can interface.
Then open to terminals.
- On terminal 1, do:
```
./main
```
- On terminal 2, do:
```
./can_write
```
