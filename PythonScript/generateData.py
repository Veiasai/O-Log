import threading
import time
import argparse
import sys
import string
import random
import re
import copy
import os

StreamIDUpBound = 1000
OpenInterestUpBound = 1000

PriceTag = 0x1
StatisticsTag = 0x2
ProgramTag = 0x3
EnableNoise = 1
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
    statisticsFeed.EXCHANGE_TIMESTAMP = timestamp / 100 * 100000000
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
    priceFeed.EXCHANGE_TIMESTAMP = timestamp / 100 * 100000000
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

def GetRandomData():
    timestamp = int(round(time.time() * 1000))
    dataIndex = random.randint(0, len(RandomLogSet)-1)
    resultStr = '%i%s' % (timestamp, RandomLogSet[dataIndex])
    return resultStr
    
def writeData(LastLegalFeed, TimeLength, dropTime, dropInterval):
    file = open(DirPath+LastLegalFeed.statisticsFeed.FEEDCODE+".log", "w")
    nowTime = int(round(time.time() * 1000))
    endTime = nowTime + TimeLength*1000
    LastSendTime = 0
    dropTimestamp = nowTime / 500 * 5 + dropInterval * 10
    nowDropTime = 0
    while(nowTime < endTime):
        sendTime = nowTime / 100
        if (sendTime != LastSendTime and sendTime % 5 == 0):
            LastLegalFeed = generateLegalStatisticsFeed(LastLegalFeed)
            timestamp = int(round(time.time() * 1000))
            statisticsFeed = LastLegalFeed.statisticsFeed
            resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_StatisticsFeed,%s,%i,%f,%f,%f,%f,%s,%f,%i,%f,%i,%i,%f,%i,%i,%f,%f,%i,%i\n' % \
                        (timestamp, statisticsFeed.FEEDCODE, statisticsFeed.INSTRUMENT_ID, statisticsFeed.HIGH_PRICE, statisticsFeed.LOW_PRICE \
                        ,statisticsFeed.OPENING_PRICE, statisticsFeed.CLOSING_PRICE, statisticsFeed.CLOSING_PRICE_TYPE, statisticsFeed.LAST_PRICE \
                        ,statisticsFeed.LAST_VOLUME, statisticsFeed.TURNOVER_VALUE, statisticsFeed.TURNOVER_VOLUME, statisticsFeed.TURNOVER_TRADE_COUNT \
                        ,statisticsFeed.SETTLEMENT_PRICE, statisticsFeed.STREAM_ID, statisticsFeed.EVENT_TIME, statisticsFeed.UPPER_PRICE_LIMIT \
                        ,statisticsFeed.LOWER_PRICE_LIMIT, statisticsFeed.OPEN_INTEREST, statisticsFeed.EXCHANGE_TIMESTAMP)
            if ProgramTag & StatisticsTag:
                if (nowDropTime < dropTime and sendTime == dropTimestamp):
                    print resultStr
                else:
                    file.write(resultStr)
            LastLegalFeed = generateLegalPriceFeed(LastLegalFeed)
            timestamp = int(round(time.time() * 1000))
            priceFeed = LastLegalFeed.priceFeed
            resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_PriceFeed,%s,%i,%i,%f,%i,%i,%f,%i,%i,%i,%i,%i,%i\n' % \
                        (timestamp, priceFeed.FEEDCODE, priceFeed.INSTRUMENT_ID, priceFeed.SEQUENCE, priceFeed.BID_PRICE, \
                         priceFeed.BID_VOLUME, priceFeed.BID_COUNT, priceFeed.ASK_PRICE, priceFeed.ASK_VOLUME, priceFeed.ASK_COUNT, \
                         priceFeed.LAST_TRADE_TICK, priceFeed.STREAM_ID, priceFeed.EVENT_TIME, priceFeed.EXCHANGE_TIMESTAMP)
            if ProgramTag & PriceTag:
                if (nowDropTime < dropTime and sendTime == dropTimestamp):
                    print resultStr
                else:
                    file.write(resultStr)
            if (nowDropTime < dropTime and sendTime == dropTimestamp):
                dropTimestamp = dropTimestamp + dropInterval * 10
                nowDropTime = nowDropTime + 1
            LastSendTime = sendTime
        elif EnableNoise:
            resultStr = GetRandomData()
            file.write(resultStr)
        time.sleep(0.005)
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
        LastLegalFeeds[i] = tempFeed
    return LastLegalFeeds

def main():
    parse=argparse.ArgumentParser()
    parse.add_argument("--fileAmount", type=int, default=1, help="file amount")
    parse.add_argument("--timeLength", type=int, default=30, help="time length")
    parse.add_argument("--dropTime", type=int, default=1, help="drop time")
    parse.add_argument("--dropInterval", type=int, default=10, help="drop interval (s)")
    parse.add_argument("--dropFileAmount", type=int, default=1, help="drop file amount")
    parse.add_argument("--messageType", type=int, default=3, help="messageType Price=0x1 Statistics=0x10")
    parse.add_argument("--enableNoise", type=int, default=1, help="whether enable noise")
    parse.add_argument("--feedCodeStartId", type=int, default=1000, help="whether enable noise")

    flags,unparsed=parse.parse_known_args(sys.argv[1:])

    generateRandomData()

    LastLegalFeeds = generateInitFeeds(flags.fileAmount, flags.feedCodeStartId)

    global ProgramTag
    global EnableNoise
    ProgramTag = flags.messageType
    EnableNoise = flags.enableNoise
    if not os.path.exists(DirPath):
        os.mkdir(DirPath)
    threads = []
    for i in range(flags.fileAmount):
        if i<flags.dropFileAmount:
            threads.append(threading.Thread(target=writeData,args=(LastLegalFeeds[i], flags.timeLength, flags.dropTime, flags.dropInterval,)))
        else:
            threads.append(threading.Thread(target=writeData,args=(LastLegalFeeds[i], flags.timeLength, 0, 0,)))

    for t in threads:
        t.start()

    # writeData(LastLegalStatisticsFeeds[0], flags.timeLength)
main()

def testGenerateLegalStatisticsFeed():
    LastLegalFeeds = generateInitFeeds(1,1000)
    LastLegalFeed = LastLegalFeeds[0]
    totalTime = 0
    for i in range(100):
        beginTime = int(round(time.time() * 1000))
        LastLegalFeed = generateLegalStatisticsFeed(LastLegalFeed)
        endTime = int(round(time.time() * 1000))
        totalTime += (endTime-beginTime)

    print totalTime/100