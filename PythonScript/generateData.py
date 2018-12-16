import threading
import time
import argparse
import sys
import string
import random
import re

LogModules = ["LogModules_1", "LogModules_2", "LogModules_3"]
size = len(LogModules)

list = [chr(i) for i in range(65,91)] + [chr(i) for i in range(97,123)] + [ str(i) for i in range(10)] + ['!','@','#','$','%','&','_']

num = random.sample(list,10) 
value = "".join(num)

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

def generateLegalStatisticsFeed(LastLegalStatisticsFeed):
    timestamp = int(round(time.time() * 1000))
    statisticsFeed = StatisticsFeed()
    statisticsFeed.FEEDCODE = LastLegalStatisticsFeed.FEEDCODE
    statisticsFeed.INSTRUMENT_ID = LastLegalStatisticsFeed.INSTRUMENT_ID
    statisticsFeed.LAST_PRICE = random.uniform(LastLegalStatisticsFeed.LOWER_PRICE_LIMIT, LastLegalStatisticsFeed.UPPER_PRICE_LIMIT)
    statisticsFeed.LAST_VOLUME = random.randint(1,10)
    statisticsFeed.HIGH_PRICE = max(statisticsFeed.LAST_PRICE, LastLegalStatisticsFeed.HIGH_PRICE)
    statisticsFeed.LOW_PRICE = min(statisticsFeed.LAST_PRICE, LastLegalStatisticsFeed.LOW_PRICE)
    statisticsFeed.OPENING_PRICE = LastLegalStatisticsFeed.OPENING_PRICE
    statisticsFeed.CLOSING_PRICE = LastLegalStatisticsFeed.CLOSING_PRICE
    statisticsFeed.CLOSING_PRICE_TYPE = LastLegalStatisticsFeed.CLOSING_PRICE_TYPE
    statisticsFeed.TURNOVER_VALUE = LastLegalStatisticsFeed.TURNOVER_VALUE + statisticsFeed.LAST_PRICE * statisticsFeed.LAST_VOLUME
    statisticsFeed.TURNOVER_VOLUME = LastLegalStatisticsFeed.TURNOVER_VOLUME + statisticsFeed.LAST_VOLUME
    statisticsFeed.SETTLEMENT_PRICE = LastLegalStatisticsFeed.SETTLEMENT_PRICE
    statisticsFeed.STREAM_ID = random.randint(0, sys.maxsize)
    statisticsFeed.EVENT_TIME = timestamp * 1000000 + random.randint(0, 999999)
    statisticsFeed.UPPER_PRICE_LIMIT = LastLegalStatisticsFeed.UPPER_PRICE_LIMIT
    statisticsFeed.LOWER_PRICE_LIMIT = LastLegalStatisticsFeed.LOWER_PRICE_LIMIT
    statisticsFeed.OPEN_INTEREST = LastLegalStatisticsFeed.OPEN_INTEREST
    statisticsFeed.EXCHANGE_TIMESTAMP = timestamp / 100 * 100000000
    return statisticsFeed

def GetRandomData():
    timestamp = int(round(time.time() * 1000))
    MessageType = ["Info", "Debug"]
    resultStr = '%i [%-6s] ' % (timestamp, MessageType[random.randint(0,1)])
    moduleLevel = random.randint(1,3)
    for i in range(moduleLevel):
        resultStr = resultStr+('[%s] ' % ('LogModule_'+str(i)+'_'+str(random.randint(0,5))))
    messageLength = random.randint(20, 200)
    for i in range(messageLength):
        resultStr = resultStr + random.choice(value)
    resultStr = resultStr + '\n'
    return resultStr
    
def writeData(LastLegalStatisticsFeed, TimeLength):
    file = open(LastLegalStatisticsFeed.FEEDCODE+".log", "w")
    nowTime = int(round(time.time() * 1000))
    endTime = nowTime + TimeLength*1000
    LastSendTime = 0
    while(nowTime < endTime):
        sendTime = nowTime/100
        if(sendTime != LastSendTime and sendTime%5 == 0):
            statisticsFeed = generateLegalStatisticsFeed(LastLegalStatisticsFeed)
            timestamp = int(round(time.time() * 1000))
            resultStr = '%i [Info  ] [DATA_RECORDER] [Data] LIMon_StatisticsFeed,%s,%i,%f,%f,%f,%f,%s,%f,%i,%f,%i,%i,%f,%i,%i,%f,%f,%i,%i\n' % \
                        (timestamp, statisticsFeed.FEEDCODE, statisticsFeed.INSTRUMENT_ID, statisticsFeed.HIGH_PRICE, statisticsFeed.LOW_PRICE \
                        ,statisticsFeed.OPENING_PRICE, statisticsFeed.CLOSING_PRICE, statisticsFeed.CLOSING_PRICE_TYPE, statisticsFeed.LAST_PRICE \
                        ,statisticsFeed.LAST_VOLUME, statisticsFeed.TURNOVER_VALUE, statisticsFeed.TURNOVER_VOLUME, statisticsFeed.TURNOVER_TRADE_COUNT \
                        ,statisticsFeed.SETTLEMENT_PRICE, statisticsFeed.STREAM_ID, statisticsFeed.EVENT_TIME, statisticsFeed.UPPER_PRICE_LIMIT \
                        ,statisticsFeed.LOWER_PRICE_LIMIT, statisticsFeed.OPEN_INTEREST, statisticsFeed.EXCHANGE_TIMESTAMP)
            LastLegalStatisticsFeed = statisticsFeed
            file.write(resultStr)
            LastSendTime = sendTime
        resultStr = GetRandomData()
        file.write(resultStr)
        time.sleep(0.001)
        nowTime = int(round(time.time() * 1000))
    file.close()

def generateInitStatisticsFeeds(num):
    LastLegalStatisticsFeeds = {}
    for i in range(num):
        tempStatisticsFeed = StatisticsFeed()
        tempStatisticsFeed.FEEDCODE = tempStatisticsFeed.FEEDCODE[0:len(tempStatisticsFeed.FEEDCODE)-len(str(i))]+str(i)
        tempStatisticsFeed.INSTRUMENT_ID += i
        tempStatisticsFeed.SETTLEMENT_PRICE = random.uniform(1000, 9000)
        tempStatisticsFeed.LOWER_PRICE_LIMIT = tempStatisticsFeed.SETTLEMENT_PRICE * 0.9
        tempStatisticsFeed.UPPER_PRICE_LIMIT = tempStatisticsFeed.SETTLEMENT_PRICE * 1.1
        tempStatisticsFeed.LOW_PRICE = sys.maxsize
        tempStatisticsFeed.HIGH_PRICE = 0
        tempStatisticsFeed.OPENING_PRICE = tempStatisticsFeed.SETTLEMENT_PRICE
        tempStatisticsFeed.CLOSING_PRICE = tempStatisticsFeed.SETTLEMENT_PRICE
        tempStatisticsFeed.OPEN_INTEREST = random.randint(0, sys.maxsize)
        LastLegalStatisticsFeeds[i] = tempStatisticsFeed
    return LastLegalStatisticsFeeds

def main():
    parse=argparse.ArgumentParser()
    parse.add_argument("--fileAmount",type=int,default=5,help="file amount")
    parse.add_argument("--timeLength",type=int,default=10,help="time length")
    flags,unparsed=parse.parse_known_args(sys.argv[1:])

    LastLegalStatisticsFeeds = generateInitStatisticsFeeds(flags.fileAmount)

    threads = []
    for i in range(flags.fileAmount):
        threads.append(threading.Thread(target=writeData,args=(LastLegalStatisticsFeeds[i], flags.timeLength,)))

    for t in threads:
        t.start()

    # writeData(LastLegalStatisticsFeeds[0], flags.timeLength)
main()