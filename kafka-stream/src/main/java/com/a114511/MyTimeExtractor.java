package com.a114511;

import net.sf.json.JSONObject;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.streams.processor.TimestampExtractor;

public class MyTimeExtractor implements TimestampExtractor {

    @Override
    public long extract(ConsumerRecord<Object, Object> consumerRecord, long l) {
        try{
            JSONObject message = JSONObject.fromObject((String) consumerRecord.value());
            String[] strArry = message.getString("detail").split(",");
            return  Long.parseLong(strArry[strArry.length-1]) / 1000000; // nano to mill
        }catch (Exception e) {
            System.out.println("extract error");
        }
        return l;
    }
}
