package com.a114511;

import kafka.admin.AdminUtils;
import kafka.admin.RackAwareMode;
import kafka.utils.ZkUtils;
import kafka.utils.json.JsonObject;
import net.sf.json.JSONObject;
import org.apache.kafka.common.security.JaasUtils;
import org.apache.kafka.common.serialization.Serde;
import org.apache.kafka.common.serialization.Serdes;
import org.apache.kafka.streams.*;
import org.apache.kafka.streams.kstream.*;
import java.util.*;
import java.util.concurrent.CountDownLatch;


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

        KStream<String, String> raw = builder.<String, String>stream("fluent-newData")
                .map((key, value) -> {
                    JSONObject message = JSONObject.fromObject(value);
                    String[] strArry = message.getString("detail").split(",");
                    String generatedKey = strArry[strArry.length - 1];
                    return KeyValue.pair(generatedKey, value);
                });

        // raw.foreach((key, value) -> System.out.println("key: " + key + " value: " + value));

        KStream<String, String> processed1 = raw.transform(new MyTransformerSupplyer());

        processed1.foreach((key, value) -> System.out.println("key: " + key + " value: " + value));
        processed1.to("demo-count-output");

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


