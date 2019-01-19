package veiasai.Model;

public class Message {
    String MessageType;
    String detail;

    @Override
    public String toString() {
        return "Message{" +
                "MessageType='" + MessageType + '\'' +
                ", detail='" + detail + '\'' +
                '}';
    }
}
