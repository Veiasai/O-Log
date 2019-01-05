package com.a114511;

import kafka.admin.AdminUtils;
import kafka.admin.RackAwareMode;
import kafka.utils.ZkUtils;
import net.sf.json.JSONObject;
import org.apache.kafka.common.security.JaasUtils;
import org.apache.kafka.common.serialization.Serde;
import org.apache.kafka.common.serialization.Serdes;
import org.apache.kafka.streams.*;
import org.apache.kafka.streams.kstream.*;
import org.apache.kafka.streams.processor.TimestampExtractor;

import java.time.Duration;
import java.util.Properties;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static org.apache.kafka.streams.kstream.Suppressed.BufferConfig.unbounded;

public class Monitor {
    public static void main(String[] args) throws Exception {
        ZkUtils zkUtils = ZkUtils.apply("zookeeper:2181", 30000, 30000, JaasUtils.isZkSecurityEnabled());
        AdminUtils.createTopic(zkUtils, "demo-count-output", 1, 1, new Properties(), RackAwareMode.Enforced$.MODULE$);
        zkUtils.close();

        final String bootstrapServers = args.length > 0 ? args[0] : "kafka:9092";
        final Properties streamsConfiguration = new Properties();
        // Give the Streams application a unique name.  The name must be unique in the Kafka cluster
        // against which the application is run.
        streamsConfiguration.put(StreamsConfig.APPLICATION_ID_CONFIG, "anomaly-detection-lambda-example");
        streamsConfiguration.put(StreamsConfig.CLIENT_ID_CONFIG, "anomaly-detection-lambda-example-client");
        // Where to find Kafka broker(s).
        streamsConfiguration.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        // Specify default (de)serializers for record keys and for record values.
        streamsConfiguration.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass().getName());
        streamsConfiguration.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass().getName());
        // Set the commit interval to 500ms so that any changes are flushed frequently. The low latency
        // would be important for anomaly detection.
        streamsConfiguration.put(StreamsConfig.COMMIT_INTERVAL_MS_CONFIG, 500);

        streamsConfiguration.put(StreamsConfig.DEFAULT_TIMESTAMP_EXTRACTOR_CLASS_CONFIG, MyTimeExtractor.class);

        final Serde<String> stringSerde = Serdes.String();
        final Serde<Long> longSerde = Serdes.Long();

        final StreamsBuilder builder = new StreamsBuilder();


        builder.stream("fluent-newData", Consumed.with(Serdes.String(), Serdes.String()))
                .map((key, value) -> {
                    JSONObject message = JSONObject.fromObject(value);
                    String[] strArry = message.getString("detail").split(",");
                    return KeyValue.pair(strArry[0], value);
                })
                .groupByKey(Grouped.with(Serdes.String(), Serdes.String()))
                .windowedBy(TimeWindows.of(Duration.ofSeconds(1)).grace(Duration.ofSeconds(30)))
                .count()
                .suppress(Suppressed.untilWindowCloses(unbounded()))
                .filter((windowedUserId, count) -> count < 2)
                .toStream()
                .filter((window, count) -> count != null)
                .map((windowedUserId, count) -> new KeyValue<>(windowedUserId.toString(), windowedUserId.toString()))
                .to("demo-count-output", Produced.with(stringSerde, stringSerde));

        final KafkaStreams streams = new KafkaStreams(builder.build(), streamsConfiguration);

        streams.cleanUp();
        streams.start();

        Runtime.getRuntime().addShutdownHook(new Thread(streams::close));
    }

    private static void sendAlert(Window window, String key, Long count) {
        System.out.println(key + " " + count.toString());
    }
}
