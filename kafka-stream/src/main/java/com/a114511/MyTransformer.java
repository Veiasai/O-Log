package com.a114511;

import net.sf.json.JSONObject;
import org.apache.kafka.streams.KeyValue;
import org.apache.kafka.streams.kstream.Transformer;
import org.apache.kafka.streams.processor.ProcessorContext;

import java.util.*;
import java.util.concurrent.TimeUnit;


public class MyTransformer implements Transformer<String, String, KeyValue<String, String>> {


    private Long latest = 0L;
    private boolean writable;
    private boolean isFirst = true;

    private List<String> products = new ArrayList<>();

    private Map<String, List<String>> records = new HashMap<String, List<String>>();

    private Vector<KeyValue<String, String>> retVec = new Vector<KeyValue<String, String>>();

    @Override
    @SuppressWarnings("unchecked")
    public void init(ProcessorContext context) {
        // Not needed.

        int productNum = 3000;
        int productIdStart = 4000;
        for (int i = 0; i < productNum; ++i) {
            products.add("rb" + String.valueOf(productIdStart + i));
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(TimeUnit.SECONDS.toMillis(50));
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
            List<String> temp = new ArrayList<String>();
            temp.add(value);
            records.put(recordKey, temp);
            return null;
        } else {
            JSONObject message = JSONObject.fromObject(recordValue);
            String[] strArry = message.getString("detail").split(",");
            String value = strArry[0];
//            records.merge(recordKey, value, (a, b) -> a + "," + b);
            if (records.containsKey(recordKey)) {
                List<String> temp = records.get(recordKey);
                if (!temp.contains(value)) {
                    if (!temp.add(value)) {
                        System.out.println(recordKey+ " " + value + " add failed");
                    }
                }
                records.replace(recordKey, temp);
            } else {
                List<String> temp = new ArrayList<String>();
                temp.add(value);
                records.put(recordKey, temp);
            }
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
        List<String> value = new ArrayList<>();
        for (Map.Entry<String, List<String>> entry : records.entrySet()) {
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
            for (int i = 0; i < products.size(); i++) {
                if (!value.contains(products.get(i))) {
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

