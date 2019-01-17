cp -f ./rdkafka/link_lib/* /usr/lib/
ln -s /usr/lib/libjsoncpp.so.0.10.6 /usr/lib/libjsoncpp.so.0
timeout 120 ./rdkafka/monitor
exit 0