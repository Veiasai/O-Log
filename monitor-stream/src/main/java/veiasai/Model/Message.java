package veiasai.Model;

import com.google.gson.annotations.SerializedName;

public class Message {
    @SerializedName("@timestamp")
    public Double timesSamp;

    public Long log_time;

    @SerializedName("MessageType")
    public String messageType;

    public String detail;
}
