FROM ubuntu:latest
LABEL authors="bena"

RUN apt install iptables libmariadb-dev -y
WORKDIR /root

ENTRYPOINT ["top", "-b"]