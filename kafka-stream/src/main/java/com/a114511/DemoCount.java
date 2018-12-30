package com.a114511;

import kafka.admin.AdminUtils;
import kafka.admin.RackAwareMode;
import kafka.utils.ZkUtils;
import kafka.utils.json.JsonObject;
import net.sf.json.JSONObject;
import org.apache.kafka.common.security.JaasUtils;
import org.apache.kafka.common.serialization.Serde;
import org.apache.kafka.common.serialization.Serdes;
import org.apache.kafka.common.utils.Bytes;
import org.apache.kafka.streams.*;
import org.apache.kafka.streams.kstream.*;
import org.apache.kafka.streams.state.KeyValueStore;
import org.apache.kafka.streams.state.WindowStore;

import java.security.acl.Group;
import java.time.Duration;
import java.util.*;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.regex.Pattern;

public class DemoCount {

    public static void main(String[] args) throws Exception {

        ZkUtils zkUtils = ZkUtils.apply("zookeeper:2181", 30000, 30000, JaasUtils.isZkSecurityEnabled());
        AdminUtils.createTopic(zkUtils, "demo-count-output", 1, 1, new Properties(), RackAwareMode.Enforced$.MODULE$);
        zkUtils.close();

        Properties props = new Properties();
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "streams-democount");
        props.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, "kafka:9092");
        props.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        props.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass());

        // build stream
        final StreamsBuilder builder = new StreamsBuilder();

        KStream<String, String> temp = builder.<String, String>stream("fluent-newData")
                .map((key, value) -> {
                    JSONObject message = JSONObject.fromObject(value);
                    String generatedKey = message.getString("detail").split(",")[0];
                    return KeyValue.pair(generatedKey, value);
                })
                .groupByKey()
                .windowedBy(TimeWindows.of(TimeUnit.MINUTES.toMillis(2)).advanceBy(500))
                .count()
                .toStream((key, value) -> key.toString())
                .flatMap((key, value) -> {
                    List<KeyValue<String, String>> result = new LinkedList<>();
                    if (value == 0) {
                        result.add(KeyValue.pair(key, key));
                    }
                    return result;
                });

        temp.foreach((key, value) -> System.out.println("Key" + key));
        temp.to("demo-count-output");

//        temp.foreach((key, value) -> System.out.println("Key:" + key + ", Value:" + value));

        final Topology topology = builder.build();
        final KafkaStreams streams = new KafkaStreams(topology, props);
        final CountDownLatch latch = new CountDownLatch(1);

        // attach shutdown handler to catch control-c
        Runtime.getRuntime().addShutdownHook(new Thread("streams-shutdown-hook") {
            @Override
            public void run() {
                streams.close();
                latch.countDown();
            }
        });

        try {
            streams.start();
            latch.await();
        } catch (Throwable e) {
            System.exit(1);
        }
        System.exit(0);

    }

}
