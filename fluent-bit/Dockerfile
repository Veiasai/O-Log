FROM gcc-cmake:gcc-8

ARG file=fluent-bit-1.0.1

WORKDIR /home

RUN wget https://fluentbit.io/releases/1.0/${file}.tar.gz

RUN tar -zxvf ${file}.tar.gz

WORKDIR ${file}/build

RUN cmake ../ && make && make install