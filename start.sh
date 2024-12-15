#!/bin/bash

cd /root/mnt
cp wg0.conf /etc/wireguard/
wg-quick up wg0

host=$1
user=$2
password=$3
db=$4
port=$5
nic=$6
node=$7

cd ..
./PortManager_backend $host $user $password $db $port $nic $node