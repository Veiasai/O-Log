import threading
import time
import argparse
import sys
import string
import random
import re
import copy
import os
import json
from queue import Queue

StreamIDUpBound = 1000
OpenInterestUpBound = 1000

PriceTag = 0x1
StatisticsTag = 0x2
DirPath = "DataDir/"
RandomLogSet = []

class PriceFeed:
    FEEDCODE = "rb1000"         #string:20
    INSTRUMENT_ID = 2306359659839382000 #int64
    SEQUENCE = 0                #uint32
    BID_PRICE = 0.0             #double
    BID_VOLUME = 0              #int32
    BID_COUNT = 0               #uint32
    ASK_PRICE = 0.0             #double
    ASK_VOLUME = 0              #int32
    ASK_COUNT = 0               #uint32
    LAST_TRADE_TICK = 0         #uint32
    STREAM_ID = 0               #uint32
    EVENT_TIME = 0              #int64
    EXCHANGE_TIMESTAMP = 0      #int64

class StatisticsFeed:
    FEEDCODE = "rb1000"         #string:20
    INSTRUMENT_ID = 2306359659839382000 #int64
    HIGH_PRICE = 0.0            #double
    LOW_PRICE = 0.0             #double
    OPENING_PRICE = 0.0         #double
    CLOSING_PRICE = 0.0         #double
    CLOSING_PRICE_TYPE = "CLOSING_PRICE_PREVIOUS_TRADING_DAY"   #string:80
    LAST_PRICE = 0.0            #double
    LAST_VOLUME = 0             #int32
    TURNOVER_VALUE = 0.0        #double
    TURNOVER_VOLUME = 0         #int32
    TURNOVER_TRADE_COUNT = 0    #uint32
    SETTLEMENT_PRICE = 0.0      #double
    STREAM_ID = 0               #uint32
    EVENT_TIME = 0              #int64
    UPPER_PRICE_LIMIT = 0.0     #double
    LOWER_PRICE_LIMIT = 0.0     #double
    OPEN_INTEREST = 0           #int32
    EXCHANGE_TIMESTAMP = 0      #int64
    #SYMBOL_NAME:FEEDCODE

class Feed:
    statisticsFeed = StatisticsFeed()
    priceFeed = PriceFeed()
    FeedCodeId = 0
    nextDropTimestamp = 0
    currentDropTime = 0
    nextDelayTimestamp = 0
    nextResendTimestamp = 0
    currentDelayTime = 0
    nextErrorTurnoverTimestamp = 0
    currentErrorTurnoverTime = 0
    nextErrorBidAskTimestamp = 0
    currentErrorBidAskTime = 0
    nextErrorPriceTimestamp = 0
    currentErrorPriceTime = 0
    nextErrorVolumeTimestamp = 0
    currentErrorVolumeTime = 0

def generateLegalStatisticsFeed(LastLegalFeed):
    timestamp = int(round(time.time() * 1000))
    LastLegalStatisticsFeed = LastLegalFeed.statisticsFeed
    LastLegalPriceFeed = LastLegalFeed.priceFeed
    statisticsFeed = StatisticsFeed()
    statisticsFeed.FEEDCODE = LastLegalStatisticsFeed.FEEDCODE
    statisticsFeed.INSTRUMENT_ID = LastLegalStatisticsFeed.INSTRUMENT_ID
    statisticsFeed.LAST_PRICE = random.uniform(max(LastLegalStatisticsFeed.LOWER_PRICE_LIMIT, LastLegalPriceFeed.BID_PRICE*0.99), 
                                               min(LastLegalStatisticsFeed.UPPER_PRICE_LIMIT, LastLegalPriceFeed.ASK_PRICE*1.01))
    statisticsFeed.LAST_VOLUME = random.randint(1,10)
    statisticsFeed.HIGH_PRICE = max(statisticsFeed.LAST_PRICE, LastLegalStatisticsFeed.HIGH_PRICE)
    statisticsFeed.LOW_PRICE = min(statisticsFeed.LAST_PRICE, LastLegalStatisticsFeed.LOW_PRICE)
    statisticsFeed.OPENING_PRICE = LastLegalStatisticsFeed.OPENING_PRICE
    statisticsFeed.CLOSING_PRICE = LastLegalStatisticsFeed.CLOSING_PRICE
    statisticsFeed.CLOSING_PRICE_TYPE = LastLegalStatisticsFeed.CLOSING_PRICE_TYPE
    statisticsFeed.TURNOVER_VALUE = LastLegalStatisticsFeed.TURNOVER_VALUE + statisticsFeed.LAST_PRICE * statisticsFeed.LAST_VOLUME
    statisticsFeed.TURNOVER_VOLUME = LastLegalStatisticsFeed.TURNOVER_VOLUME + statisticsFeed.LAST_VOLUME
    statisticsFeed.SETTLEMENT_PRICE = LastLegalStatisticsFeed.SETTLEMENT_PRICE
    statisticsFeed.STREAM_ID = random.randint(0, StreamIDUpBound)
    statisticsFeed.EVENT_TIME = timestamp * 1000000 + random.randint(0, 999999)
    statisticsFeed.UPPER_PRICE_LIMIT = LastLegalStatisticsFeed.UPPER_PRICE_LIMIT
    statisticsFeed.LOWER_PRICE_LIMIT = LastLegalStatisticsFeed.LOWER_PRICE_LIMIT
    statisticsFeed.OPEN_INTEREST = LastLegalStatisticsFeed.OPEN_INTEREST
    statisticsFeed.EXCHANGE_TIMESTAMP = LastLegalStatisticsFeed.EXCHANGE_TIMESTAMP + 500000000
    LastLegalFeed.statisticsFeed = statisticsFeed
    return LastLegalFeed

def generateLegalPriceFeed(LastLegalFeed):
    timestamp = int(round(time.time() * 1000))
    CurrentLegalStatisticsFeed = LastLegalFeed.statisticsFeed
    LastLegalPriceFeed = LastLegalFeed.priceFeed
    priceFeed = PriceFeed()
    priceFeed.FEEDCODE = LastLegalPriceFeed.FEEDCODE
    priceFeed.INSTRUMENT_ID = LastLegalPriceFeed.INSTRUMENT_ID
    priceFeed.BID_PRICE = random.uniform(max(CurrentLegalStatisticsFeed.LOWER_PRICE_LIMIT, CurrentLegalStatisticsFeed.LAST_PRICE*0.99), 
                                         CurrentLegalStatisticsFeed.LAST_PRICE)
    priceFeed.BID_VOLUME = random.randint(1,30)
    priceFeed.ASK_PRICE = random.uniform(CurrentLegalStatisticsFeed.LAST_PRICE, 
                                         min(CurrentLegalStatisticsFeed.UPPER_PRICE_LIMIT, CurrentLegalStatisticsFeed.LAST_PRICE*1.01))
    priceFeed.ASK_VOLUME = random.randint(1,30)
    priceFeed.STREAM_ID = random.randint(0, StreamIDUpBound)
    priceFeed.EVENT_TIME = timestamp * 1000000 + random.randint(0, 999999)
    priceFeed.EXCHANGE_TIMESTAMP = CurrentLegalStatisticsFeed.EXCHANGE_TIMESTAMP
    LastLegalFeed.priceFeed = priceFeed
    return LastLegalFeed

def generateRandomData():
    MessageType = ["Info", "Debug"]
    resultStr = (' [%-6s]' % (MessageType[random.randint(0,1)]))
    moduleLevel = 3
    moduleType = 5
    finalSet = set()
    LastSet = set()
    for typeStr in MessageType:
        LastSet.add((' [%-6s]' % (typeStr)))
    
    NowSet = set()
    for i in range(moduleLevel):
        for tempStr in LastSet:
            for j in range(moduleType):
                NowSet.add(tempStr + (' [%s]' % ('LogModule_'+str(i)+'_'+str(j))))
        finalSet = finalSet.union(NowSet)
        LastSet = copy.deepcopy(NowSet)
        NowSet = set()

    list = [chr(i) for i in range(65,91)] + [chr(i) for i in range(97,123)] + [ str(i) for i in range(10)] + ['!','@','#','$','%','&','_']
    for tempStr in finalSet:
        messageLength = random.randint(2, 10)
        resultStr = tempStr
        for i in range(messageLength):
            num = random.sample(list,10) 
            value = "".join(num)
            resultStr = resultStr +  " " + value
        resultStr = resultStr + '\n'
        RandomLogSet.append(resultStr)

def inRange(FeedCodeId, startFeedCodeId, amount):
    if FeedCodeId >= startFeedCodeId and FeedCodeId < startFeedCodeId + amount:
        return True
    else:
        return False

def GetRandomData():
    timestamp = int(round(time.time() * 1000))
    dataIndex = random.randint(0, len(RandomLogSet)-1)
    resultStr = '%i%s' % (timestamp, RandomLogSet[dataIndex])
    return resultStr

def writeStatisticsFeed(LastLegalFeed, file, errorFile, conf):
    timestamp = int(round(time.time() * 1000))
    statisticsFeed = LastLegalFeed.statisticsFeed
    resultStr = ""
    if (inRange(LastLegalFeed.FeedCodeId, conf["errorTurnover"]["feedCodeStartId"], conf["errorTurnover"]["instrumentAmount"]) and LastLegalFeed.currentErrorTurnoverTime < conf["errorTurnover"]["errorAmount"] and statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorTurnoverTimestamp):
        resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_StatisticsFeed,%s,%i,%f,%f,%f,%f,%s,%f,%i,%f,%i,%i,%f,%i,%i,%f,%f,%i,%i\n' % \
                    (timestamp, statisticsFeed.FEEDCODE, statisticsFeed.INSTRUMENT_ID, statisticsFeed.HIGH_PRICE, statisticsFeed.LOW_PRICE \
                    ,statisticsFeed.OPENING_PRICE, statisticsFeed.CLOSING_PRICE, statisticsFeed.CLOSING_PRICE_TYPE, statisticsFeed.LAST_PRICE \
                    ,statisticsFeed.LAST_VOLUME, 0.0, 0, statisticsFeed.TURNOVER_TRADE_COUNT \
                    ,statisticsFeed.SETTLEMENT_PRICE, statisticsFeed.STREAM_ID, statisticsFeed.EVENT_TIME, statisticsFeed.UPPER_PRICE_LIMIT \
                    , statisticsFeed.LOWER_PRICE_LIMIT, statisticsFeed.OPEN_INTEREST, statisticsFeed.EXCHANGE_TIMESTAMP)
    else:
        resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_StatisticsFeed,%s,%i,%f,%f,%f,%f,%s,%f,%i,%f,%i,%i,%f,%i,%i,%f,%f,%i,%i\n' % \
                    (timestamp, statisticsFeed.FEEDCODE, statisticsFeed.INSTRUMENT_ID, statisticsFeed.HIGH_PRICE, statisticsFeed.LOW_PRICE \
                    ,statisticsFeed.OPENING_PRICE, statisticsFeed.CLOSING_PRICE, statisticsFeed.CLOSING_PRICE_TYPE, statisticsFeed.LAST_PRICE \
                    ,statisticsFeed.LAST_VOLUME, statisticsFeed.TURNOVER_VALUE, statisticsFeed.TURNOVER_VOLUME, statisticsFeed.TURNOVER_TRADE_COUNT \
                    ,statisticsFeed.SETTLEMENT_PRICE, statisticsFeed.STREAM_ID, statisticsFeed.EVENT_TIME, statisticsFeed.UPPER_PRICE_LIMIT \
                    ,statisticsFeed.LOWER_PRICE_LIMIT, statisticsFeed.OPEN_INTEREST, statisticsFeed.EXCHANGE_TIMESTAMP)
    if conf["base"]["messageType"] & StatisticsTag:
        if (inRange(LastLegalFeed.FeedCodeId, conf["drop"]["feedCodeStartId"], conf["drop"]["instrumentAmount"]) and LastLegalFeed.currentDropTime < conf["drop"]["dropAmount"] and statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextDropTimestamp):
            errorFile.write("drop:" + resultStr)
        elif (inRange(LastLegalFeed.FeedCodeId, conf["delay"]["feedCodeStartId"], conf["delay"]["instrumentAmount"]) and LastLegalFeed.currentDelayTime < conf["delay"]["delayAmount"] and statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextDelayTimestamp):
            errorFile.write("delay:" + resultStr)
        elif (inRange(LastLegalFeed.FeedCodeId, conf["errorTurnover"]["feedCodeStartId"], conf["errorTurnover"]["instrumentAmount"]) and LastLegalFeed.currentErrorTurnoverTime < conf["errorTurnover"]["errorAmount"] and statisticsFeed.EXCHANGE_TIMESTAMP / 100000000 == LastLegalFeed.nextErrorTurnoverTimestamp):
            errorFile.write("errorTurnover:" + resultStr)
            file.write(resultStr)
        else:
            file.write(resultStr)

def writePriceFeed(LastLegalFeed, file, errorFile, conf):
    timestamp = int(round(time.time() * 1000))
    priceFeed = LastLegalFeed.priceFeed
    resultStr = ""
    if (inRange(LastLegalFeed.FeedCodeId, conf["errorBidAsk"]["feedCodeStartId"], conf["errorBidAsk"]["instrumentAmount"]) and LastLegalFeed.currentErrorBidAskTime < conf["errorBidAsk"]["errorAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorBidAskTimestamp):
        resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_PriceFeed,%s,%i,%i,%f,%i,%i,%f,%i,%i,%i,%i,%i,%i\n' % \
                    (timestamp, priceFeed.FEEDCODE, priceFeed.INSTRUMENT_ID, priceFeed.SEQUENCE, priceFeed.ASK_PRICE, \
                        priceFeed.BID_VOLUME, priceFeed.BID_COUNT, priceFeed.BID_PRICE, priceFeed.ASK_VOLUME, priceFeed.ASK_COUNT, \
                        priceFeed.LAST_TRADE_TICK, priceFeed.STREAM_ID, priceFeed.EVENT_TIME, priceFeed.EXCHANGE_TIMESTAMP)
    elif (inRange(LastLegalFeed.FeedCodeId, conf["errorPrice"]["feedCodeStartId"], conf["errorPrice"]["instrumentAmount"]) and LastLegalFeed.currentErrorPriceTime < conf["errorPrice"]["errorAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorPriceTimestamp):
        resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_PriceFeed,%s,%i,%i,%f,%i,%i,%f,%i,%i,%i,%i,%i,%i\n' % \
                    (timestamp, priceFeed.FEEDCODE, priceFeed.INSTRUMENT_ID, priceFeed.SEQUENCE, 0, \
                        priceFeed.BID_VOLUME, priceFeed.BID_COUNT, 0-priceFeed.BID_PRICE, priceFeed.ASK_VOLUME, priceFeed.ASK_COUNT, \
                        priceFeed.LAST_TRADE_TICK, priceFeed.STREAM_ID, priceFeed.EVENT_TIME, priceFeed.EXCHANGE_TIMESTAMP)
    elif (inRange(LastLegalFeed.FeedCodeId, conf["errorVolume"]["feedCodeStartId"], conf["errorVolume"]["instrumentAmount"]) and LastLegalFeed.currentErrorVolumeTime < conf["errorVolume"]["errorAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorVolumeTimestamp):
        resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_PriceFeed,%s,%i,%i,%f,%i,%i,%f,%i,%i,%i,%i,%i,%i\n' % \
                    (timestamp, priceFeed.FEEDCODE, priceFeed.INSTRUMENT_ID, priceFeed.SEQUENCE, priceFeed.BID_PRICE, \
                        0-priceFeed.BID_VOLUME, priceFeed.BID_COUNT, priceFeed.ASK_PRICE, 0-priceFeed.ASK_VOLUME, priceFeed.ASK_COUNT, \
                        priceFeed.LAST_TRADE_TICK, priceFeed.STREAM_ID, priceFeed.EVENT_TIME, priceFeed.EXCHANGE_TIMESTAMP)
    else:
        resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_PriceFeed,%s,%i,%i,%f,%i,%i,%f,%i,%i,%i,%i,%i,%i\n' % \
                    (timestamp, priceFeed.FEEDCODE, priceFeed.INSTRUMENT_ID, priceFeed.SEQUENCE, priceFeed.BID_PRICE, \
                        priceFeed.BID_VOLUME, priceFeed.BID_COUNT, priceFeed.ASK_PRICE, priceFeed.ASK_VOLUME, priceFeed.ASK_COUNT, \
                        priceFeed.LAST_TRADE_TICK, priceFeed.STREAM_ID, priceFeed.EVENT_TIME, priceFeed.EXCHANGE_TIMESTAMP)
    if conf["base"]["messageType"] & PriceTag:
        if (inRange(LastLegalFeed.FeedCodeId, conf["drop"]["feedCodeStartId"], conf["drop"]["instrumentAmount"]) and LastLegalFeed.currentDropTime < conf["drop"]["dropAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextDropTimestamp):
            errorFile.write("drop:" + resultStr)
        elif (inRange(LastLegalFeed.FeedCodeId, conf["delay"]["feedCodeStartId"], conf["delay"]["instrumentAmount"]) and LastLegalFeed.currentDelayTime < conf["delay"]["delayAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextDelayTimestamp):
            errorFile.write("delay:" + resultStr)
        elif (inRange(LastLegalFeed.FeedCodeId, conf["errorBidAsk"]["feedCodeStartId"], conf["errorBidAsk"]["instrumentAmount"]) and LastLegalFeed.currentErrorBidAskTime < conf["errorBidAsk"]["errorAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorBidAskTimestamp):
            errorFile.write("errorBidAsk:" + resultStr)
            file.write(resultStr)
        elif (inRange(LastLegalFeed.FeedCodeId, conf["errorPrice"]["feedCodeStartId"], conf["errorPrice"]["instrumentAmount"]) and LastLegalFeed.currentErrorPriceTime < conf["errorPrice"]["errorAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorPriceTimestamp):
            errorFile.write("errorPrice:" + resultStr)
            file.write(resultStr)
        elif (inRange(LastLegalFeed.FeedCodeId, conf["errorVolume"]["feedCodeStartId"], conf["errorVolume"]["instrumentAmount"]) and LastLegalFeed.currentErrorVolumeTime < conf["errorVolume"]["errorAmount"] and priceFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorVolumeTimestamp):
            errorFile.write("errorVolume:" + resultStr)
            file.write(resultStr)
        else:
            file.write(resultStr)

def generateData(LastLegalFeed, conf):
    file = open(DirPath + LastLegalFeed.statisticsFeed.FEEDCODE + ".log", "w")
    errorFile = open(DirPath + LastLegalFeed.statisticsFeed.FEEDCODE + "_error.log", "w")
    delayQueue = Queue()
    nowTime = int(round(time.time() * 1000))
    endTime = nowTime + conf["base"]["timeLength"]*1000
    LastLegalFeed.nextDropTimestamp = nowTime / 500 * 5 + conf["drop"]["dropInterval"] * 10
    LastLegalFeed.nextDelayTimestamp = nowTime / 500 * 5 + conf["delay"]["delayInterval"] * 10
    LastLegalFeed.nextResendTimestamp = LastLegalFeed.nextDelayTimestamp + conf["delay"]["delayTimeLength"] * 10
    LastLegalFeed.nextErrorTurnoverTimestamp = nowTime / 500 * 5 + conf["errorTurnover"]["errorInterval"] * 10
    LastLegalFeed.nextErrorBidAskTimestamp = nowTime / 500 * 5 + conf["errorTurnover"]["errorInterval"] * 10
    LastLegalFeed.nextErrorPriceTimestamp = nowTime / 500 * 5 + conf["errorPrice"]["errorInterval"] * 10
    LastLegalFeed.nextErrorVolumeTimestamp = nowTime / 500 * 5 + conf["errorVolume"]["errorInterval"] * 10
    while(nowTime < endTime):
        sendTime = nowTime / 100
        if (sendTime > LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000+5):
            LastLegalFeed = generateLegalStatisticsFeed(LastLegalFeed)
            writeStatisticsFeed(LastLegalFeed, file, errorFile, conf)
            LastLegalFeed = generateLegalPriceFeed(LastLegalFeed)
            writePriceFeed(LastLegalFeed, file, errorFile, conf)
            if (inRange(LastLegalFeed.FeedCodeId, conf["drop"]["feedCodeStartId"], conf["drop"]["instrumentAmount"]) and LastLegalFeed.currentDropTime < conf["drop"]["dropAmount"] and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextDropTimestamp):
                LastLegalFeed.nextDropTimestamp += conf["drop"]["dropInterval"] * 10
                LastLegalFeed.currentDropTime += 1
            if (inRange(LastLegalFeed.FeedCodeId, conf["delay"]["feedCodeStartId"], conf["delay"]["instrumentAmount"]) and LastLegalFeed.currentDelayTime < conf["delay"]["delayAmount"] and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextDelayTimestamp):
                LastLegalFeed.nextDelayTimestamp += conf["delay"]["delayInterval"] * 10
                LastLegalFeed.currentDelayTime += 1
                delayQueue.put(copy.deepcopy(LastLegalFeed))
            if (inRange(LastLegalFeed.FeedCodeId, conf["delay"]["feedCodeStartId"], conf["delay"]["instrumentAmount"]) and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP / 100000000 == LastLegalFeed.nextResendTimestamp):
                if not delayQueue.empty():
                    resendLegalFeed = delayQueue.get()
                    errorFile.write("resend:"+str(resendLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP)+"\n")
                    writeStatisticsFeed(resendLegalFeed, file, errorFile, conf)
                    writePriceFeed(resendLegalFeed, file, errorFile, conf)
                    if not delayQueue.empty():
                        LastLegalFeed.nextResendTimestamp += conf["delay"]["delayInterval"] * 10
            if (inRange(LastLegalFeed.FeedCodeId, conf["errorTurnover"]["feedCodeStartId"], conf["errorTurnover"]["instrumentAmount"]) and LastLegalFeed.currentErrorTurnoverTime < conf["errorTurnover"]["errorAmount"] and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorTurnoverTimestamp):
                LastLegalFeed.nextErrorTurnoverTimestamp += conf["errorTurnover"]["errorInterval"] * 10
                LastLegalFeed.currentErrorTurnoverTime += 1
            if (inRange(LastLegalFeed.FeedCodeId, conf["errorBidAsk"]["feedCodeStartId"], conf["errorBidAsk"]["instrumentAmount"]) and LastLegalFeed.currentErrorBidAskTime < conf["errorBidAsk"]["errorAmount"] and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorBidAskTimestamp):
                LastLegalFeed.nextErrorBidAskTimestamp += conf["errorBidAsk"]["errorInterval"] * 10
                LastLegalFeed.currentErrorBidAskTime += 1
            if (inRange(LastLegalFeed.FeedCodeId, conf["errorPrice"]["feedCodeStartId"], conf["errorPrice"]["instrumentAmount"]) and LastLegalFeed.currentErrorPriceTime < conf["errorPrice"]["errorAmount"] and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorPriceTimestamp):
                LastLegalFeed.nextErrorPriceTimestamp += conf["errorPrice"]["errorInterval"] * 10
                LastLegalFeed.currentErrorPriceTime += 1
            if (inRange(LastLegalFeed.FeedCodeId, conf["errorVolume"]["feedCodeStartId"], conf["errorVolume"]["instrumentAmount"]) and LastLegalFeed.currentErrorVolumeTime < conf["errorVolume"]["errorAmount"] and LastLegalFeed.statisticsFeed.EXCHANGE_TIMESTAMP/100000000 == LastLegalFeed.nextErrorVolumeTimestamp):
                LastLegalFeed.nextErrorVolumeTimestamp += conf["errorVolume"]["errorInterval"] * 10
                LastLegalFeed.currentErrorVolumeTime += 1
        elif conf["base"]["enableNoise"]:
            resultStr = GetRandomData()
            file.write(resultStr)
        if conf["base"]["enableNoise"]:
            time.sleep(0.005)
        else:
            time.sleep(0.1)
        
        nowTime = int(round(time.time() * 1000))
    file.close()
    print LastLegalFeed.statisticsFeed.FEEDCODE+" done"

def generateInitFeeds(num, feedCodeStartId):
    LastLegalFeeds = {}
    for i in range(num):
        tempStatisticsFeed = StatisticsFeed()
        tempStatisticsFeed.FEEDCODE = tempStatisticsFeed.FEEDCODE[0:2]+str(i+feedCodeStartId)
        tempStatisticsFeed.INSTRUMENT_ID += i
        tempStatisticsFeed.SETTLEMENT_PRICE = random.uniform(1000, 9000)
        tempStatisticsFeed.LOWER_PRICE_LIMIT = tempStatisticsFeed.SETTLEMENT_PRICE * 0.9
        tempStatisticsFeed.UPPER_PRICE_LIMIT = tempStatisticsFeed.SETTLEMENT_PRICE * 1.1
        tempStatisticsFeed.LOW_PRICE = sys.maxsize
        tempStatisticsFeed.HIGH_PRICE = 0
        tempStatisticsFeed.OPENING_PRICE = tempStatisticsFeed.SETTLEMENT_PRICE
        tempStatisticsFeed.CLOSING_PRICE = tempStatisticsFeed.SETTLEMENT_PRICE
        tempStatisticsFeed.OPEN_INTEREST = random.randint(0, OpenInterestUpBound)
        timestamp = int(round(time.time() * 1000))
        tempStatisticsFeed.EXCHANGE_TIMESTAMP = timestamp/500*500000000

        tempPriceFeed = PriceFeed()
        tempPriceFeed.FEEDCODE = tempStatisticsFeed.FEEDCODE
        tempPriceFeed.INSTRUMENT_ID += i
        tempPriceFeed.BID_PRICE = random.uniform(tempStatisticsFeed.SETTLEMENT_PRICE*0.99, tempStatisticsFeed.SETTLEMENT_PRICE*1.01)
        tempPriceFeed.BID_VOLUME = random.randint(1,30)
        tempPriceFeed.ASK_PRICE = random.uniform(tempPriceFeed.BID_PRICE, tempPriceFeed.BID_PRICE*1.01)
        tempPriceFeed.ASK_VOLUME = random.randint(1,30)

        tempFeed = Feed()
        tempFeed.priceFeed = tempPriceFeed
        tempFeed.statisticsFeed = tempStatisticsFeed
        tempFeed.FeedCodeId = i+feedCodeStartId
        LastLegalFeeds[i] = tempFeed
    return LastLegalFeeds

def main():
    confFile = open("conf.json", 'r')
    conf = json.loads(confFile.read())
    confFile.close()

    generateRandomData()

    LastLegalFeeds = generateInitFeeds(conf["base"]["instrumentAmount"], conf["base"]["feedCodeStartId"])

    if not os.path.exists(DirPath):
        os.mkdir(DirPath)
    threads = []
    for i in range(conf["base"]["instrumentAmount"]):
        if i>=conf["drop"]["feedCodeStartId"] and i<conf["drop"]["feedCodeStartId"] + conf["drop"]["instrumentAmount"]:
            threads.append(threading.Thread(target=generateData,args=(LastLegalFeeds[i], conf,)))
        else:
            threads.append(threading.Thread(target=generateData,args=(LastLegalFeeds[i], conf,)))

    for t in threads:
        t.start()

main()
