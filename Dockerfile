FROM ubuntu:latest
LABEL authors="bena"

RUN apt update
RUN apt upgrade -y
RUN apt install iptables libmariadb-dev -y
RUN apt autoremove -y
COPY build/PortManager_backend /root

WORKDIR /root

ENTRYPOINT ["/root/PortManager_backend"]