FROM ubuntu:latest
LABEL authors="bena"

RUN apt update
RUN apt upgrade -y
RUN apt install iptables libmariadb-dev wireguard -y
RUN apt autoremove -y
COPY build/PortManager_backend /root
RUN chmod +x /root/PortManager_backend
RUN chmod +x /root/start.sh

WORKDIR /root

ENTRYPOINT ["/root/start.sh"]