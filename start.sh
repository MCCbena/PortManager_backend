#!/bin/bash

cd /root/mnt
cp wg0.conf /etc/wireguard/
wg-quick up wg0

host=$1
user=$2
password=$3
db=$4
port=$5

cd ..
PortManager_backend $host $user $password $db $port