package veiasai.State;

import veiasai.Model.Message;

public class MissStore {


    String type;
    String ticker;
    int countTrades; // tracking count and sum so we can later calculate avg price
    double sumPrice;
    double minPrice;
    double avgPrice;

    public MissStore add(String feed) {


        return this;
    }

    public MissStore computeAvgPrice() {
        this.avgPrice = this.sumPrice / this.countTrades;
        return this;
    }
}
