package veiasai;

import kafka.admin.AdminUtils;
import kafka.admin.RackAwareMode;
import kafka.utils.ZkUtils;
import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.common.errors.TopicExistsException;
import org.apache.kafka.common.security.JaasUtils;
import org.apache.kafka.common.serialization.*;
import org.apache.kafka.common.utils.Bytes;
import org.apache.kafka.streams.KafkaStreams;
import org.apache.kafka.streams.KeyValue;
import org.apache.kafka.streams.StreamsConfig;
import org.apache.kafka.streams.Topology;
import org.apache.kafka.streams.kstream.*;
import org.apache.kafka.streams.processor.Processor;
import org.apache.kafka.streams.processor.ProcessorContext;
import org.apache.kafka.streams.processor.ProcessorSupplier;
import org.apache.kafka.streams.processor.PunctuationType;
import org.apache.kafka.streams.state.KeyValueIterator;
import org.apache.kafka.streams.state.KeyValueStore;
import org.apache.kafka.streams.state.Stores;

import org.apache.kafka.common.serialization.Serdes;
import org.apache.kafka.streams.StreamsBuilder;
import org.apache.kafka.streams.state.WindowStore;
import veiasai.Model.Message;
import veiasai.State.MissStore;
import veiasai.serde.JsonDeserializer;
import veiasai.serde.JsonSerializer;

import java.time.Duration;
import java.util.*;
import java.util.concurrent.CountDownLatch;


public final class DemoStream {
    public static void main(final String[] args) {
        try {
            ZkUtils zkUtils = ZkUtils.apply("zookeeper:2181", 30000, 30000, JaasUtils.isZkSecurityEnabled());
            AdminUtils.createTopic(zkUtils, "tolerant-stream", 1, 1, new Properties(), RackAwareMode.Enforced$.MODULE$);
            zkUtils.close();
        }
        catch (TopicExistsException ignored){

        }

        final Properties props = new Properties();
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "streams-monitor");
        props.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, "kafka:9092");
        props.put(StreamsConfig.CACHE_MAX_BYTES_BUFFERING_CONFIG, 0);
        props.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        props.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        // props.put(StreamsConfig.COMMIT_INTERVAL_MS_CONFIG, 500);

        // TODO: the following can be removed with a serialization factory
        Map<String, Object> serdeProps = new HashMap<>();

        final Serializer<Message> messageSerializer = new JsonSerializer<>();
        serdeProps.put("JsonPOJOClass", Message.class);
        messageSerializer.configure(serdeProps, false);

        final Deserializer<Message> messageDeserializer = new JsonDeserializer<>();
        serdeProps.put("JsonPOJOClass", Message.class);
        messageDeserializer.configure(serdeProps, false);

        final Serde<Message> messageSerde = Serdes.serdeFrom(messageSerializer, messageDeserializer);

        final Serializer<MissStore> missStoreSerializer = new JsonSerializer<>();
        serdeProps.put("JsonPOJOClass", MissStore.class);
        missStoreSerializer.configure(serdeProps, false);

        final Deserializer<MissStore> missStoreDeserializer = new JsonDeserializer<>();
        serdeProps.put("JsonPOJOClass", MissStore.class);
        missStoreDeserializer.configure(serdeProps, false);

        final Serde<MissStore> missStoreSerde = Serdes.serdeFrom(missStoreSerializer, missStoreDeserializer);



        final StreamsBuilder builder = new StreamsBuilder();

        KStream<String, Message> source = builder.stream("fluent-newData", Consumed.with(Serdes.String(), messageSerde));

        KGroupedStream<String, Message> groupedStream = source.groupByKey();

        groupedStream.windowedBy(TimeWindows.of(Duration.ofMinutes(1)).grace(Duration.ofSeconds(30)));
        KTable<String, MissStore> missTable = groupedStream.aggregate(() -> new MissStore(),(k, v, store) -> store.add(v.detail),
                Materialized.<String, MissStore, KeyValueStore<Bytes, byte[]>>as("miss-aggregates")
                        .withValueSerde(missStoreSerde));



        final KafkaStreams streams = new KafkaStreams(builder.build(), props);

        streams.cleanUp();
        streams.start();

        Runtime.getRuntime().addShutdownHook(new Thread(streams::close));
    }
}
