cd /root/mnt
cp wg0.conf /etc/wireguard/
wg-quick up wg0

host=$(cat host)
user=$(cat user)
password=$(cat password)
db=$(cat db)
port=$(cat port)

cd ..
PortManager_backend $host $user $password $db $port