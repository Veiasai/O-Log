rm -f DataDir/*
python generateData.py &
/home/fluent-bit-1.0.1/build/bin/fluent-bit -c /home/fluent-bit-1.0.1/conf/fluent-bit.conf