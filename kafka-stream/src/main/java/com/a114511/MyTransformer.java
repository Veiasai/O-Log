package com.a114511;

import net.sf.json.JSONObject;
import org.apache.kafka.common.protocol.types.Field;
import org.apache.kafka.streams.KeyValue;
import org.apache.kafka.streams.kstream.Transformer;
import org.apache.kafka.streams.processor.ProcessorContext;

import java.time.Duration;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.TimeUnit;


public class MyTransformer implements Transformer<String, String, KeyValue<String, String>> {

//    private class DataRecord {
//        public Long dataTime;
//        public Long count;
//
//        public DataRecord(Long dataTime, Long count) {
//            this.dataTime = dataTime;
//            this.count = count;
//        }
//    }

    private Long latest = 0L;
    private boolean writable;
    private boolean isFirst = true;

    private Vector<String> products = new Vector<String>();

    private Map<String, String> records = new HashMap<String, String>();

    private Vector<KeyValue<String, String>> retVec = new Vector<KeyValue<String, String>>();

    @Override
    @SuppressWarnings("unchecked")
    public void init(ProcessorContext context) {
        // Not needed.

        int productNum = 100;
        int productIdStart = 4000;
        for (int i = 0; i < productNum; ++i){
            products.add("rb" +String.valueOf(productIdStart + i));
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(TimeUnit.SECONDS.toMillis(10));
                    writable = true;
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            while (true) {
                                if (writable && !isFirst) {
                                    checkRecords();
                                }
                                try {
                                    Thread.sleep(500);
                                    latest += 500000000;
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                    }).start();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public KeyValue<String, String> transform(final String recordKey, final String recordValue) {

        if (isFirst) {
            isFirst = false;
            latest = Long.valueOf(recordKey);
            JSONObject message = JSONObject.fromObject(recordValue);
            String[] strArry = message.getString("detail").split(",");
            String value = strArry[0];
            records.put(recordKey, value);
            return null;
        } else {
            JSONObject message = JSONObject.fromObject(recordValue);
            String[] strArry = message.getString("detail").split(",");
            String value = strArry[0];
            records.merge(recordKey, value, (a, b) -> a + "," + b);
            if (retVec.size() != 0) {
                KeyValue temp = retVec.get(0);
                retVec.remove(0);
                return temp;
            } else {
                return null;
            }
        }
    }

    private void checkRecords() {
        boolean isFound = false;
        String value = "";
        for (Map.Entry<String, String> entry : records.entrySet()) {
            if (entry.getKey().contains(String.valueOf(latest))) {
                isFound = true;
                value = entry.getValue();
                records.remove(entry.getKey());
                break;
            }
        }

        if (isFound) {
//            if (!(value.contains("rb1000")&&value.contains("rb1001")&&
//                    value.contains("rb1002")&&value.contains("rb1003")&&value.contains("rb1004"))){
//                retVec.add(KeyValue.pair(String.valueOf(latest), String.valueOf(latest)));
//            }
            for (int i = 0; i < products.size(); i++){
                if (!value.contains(products.get(i))){
                    System.out.println(value);
                    JSONObject message = new JSONObject();
                    message.put("FEEDCODE", products.get(i));
                    message.put("TIMESTAMP", latest);
                    message.put("LOG", "miss");
                    retVec.add(KeyValue.pair(String.valueOf(latest), message.toString()));
                }
            }
        } else {
            // System.out.println("case2");
            JSONObject message = new JSONObject();
            message.put("FEEDCODE", "ALL");
            message.put("TIMESTAMP", latest);
            message.put("LOG", "miss");
            retVec.add(KeyValue.pair(String.valueOf(latest), message.toString()));
        }
    }


    @Override
    public void close() {
        // Not needed.
    }
}

