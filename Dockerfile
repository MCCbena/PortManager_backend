FROM ubuntu:latest
LABEL authors="bena"

RUN apt update
RUN apt upgrade -y
RUN apt install iptables libmariadb-dev -y
RUN apt autoremove -y

WORKDIR /root

ENTRYPOINT ["top", "-b"]