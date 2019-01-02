package com.a114511;

import org.apache.kafka.streams.KeyValue;
import org.apache.kafka.streams.kstream.Transformer;
import org.apache.kafka.streams.kstream.TransformerSupplier;

public class MyTransformerSupplyer implements TransformerSupplier<String, String, KeyValue<String, String>> {

    @Override
    public Transformer<String, String, KeyValue<String, String>> get() {
        return new MyTransformer();
    }
}
