#!/bin/sh

ip link set can0 down
ip link set can0 type can bitrate 1000000
ip link set can0 type can restart-ms 100
echo 4096 > /sys/class/net/can0/tx_queue_len
ip link set can0 up

for i in {1..10}
do
    ip link set can$i down
    ip link set can$i type can bitrate 500000
    ip link set can$i type can restart-ms 100
    ip link set can$i up
done





