#!/bin/sh
sudo modprobe can
sudo modprobe can_raw
sudo modprobe can-bcm
sudo modprobe can-dev
sudo modprobe can-gw
sudo modprobe vcan

# Add virtual can device
sudo ip link add dev vcan0 type vcan
# Set mtu for CAN FD
sudo ip link set vcan0 mtu 72
# Run virtual can device
sudo ifconfig vcan0 up
