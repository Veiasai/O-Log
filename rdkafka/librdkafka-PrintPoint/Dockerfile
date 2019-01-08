FROM gcc

WORKDIR /home

COPY . ./librdkafka

RUN apt-get update -y
RUN apt-get upgrade -y
RUN apt-get install build-essential
RUN wget https://cmake.org/files/v3.13/cmake-3.13.2.tar.gz -q
RUN tar -xzf cmake-3.13.2.tar.gz
# RUN cd cmake-3.13.2
WORKDIR /home/cmake-3.13.2
RUN ./bootstrap
RUN gmake
RUN make install
WORKDIR /home
# RUN cd ..

RUN apt-get install python3 python3-pip ninja-build
RUN pip3 install --user meson

RUN wget https://github.com/open-source-parsers/jsoncpp/archive/0.10.7.tar.gz
RUN tar -xzf 0.10.7.tar.gz
# RUN cd jsoncpp-0.10.7
WORKDIR /home/jsoncpp-0.10.7
RUN mkdir -p build/debug
# RUN cd build/debug
WORKDIR /home/jsoncpp-0.10.7/build/debug
RUN cmake -DCMAKE_BUILD_TYPE=release -DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON -DARCHIVE_INSTALL_DIR=. -DCMAKE_INSTALL_INCLUDEDIR=include -G "Unix Makefiles" ../..
RUN make
RUN make install
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64
ENV LIBRARY_PATH=$LIBRARY_PATH:/usr/local/lib64
RUN source /etc/profile
# RUN cd ../../..

# RUN cd librdkafka
WORKDIR /home/librdkafka
RUN ./configure
RUN make
RUN make install
# RUN cd ..
WORKDIR /home
RUN rm -rf *