FROM ubuntu:latest
LABEL authors="bena"

RUN apt update
RUN apt upgrade -y
RUN apt install iptables libmariadb-dev wireguard -y
RUN apt autoremove -y
COPY build/PortManager_backend /root
RUN chmod +x /root/PortManager_backend
RUN echo net.ipv4.ip_forward=1 >> /etc/sysctl.conf

WORKDIR /root

ENTRYPOINT ["/root/PortManager_backend"]