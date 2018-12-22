#!/bin/bash

case "$1" in
    "node")
        case "$2" in
            "start")
            ansible -i hosts all -m shell -a "source /etc/profile ; nohup /root/node_exporter-0.16.0.linux-amd64/node_exporter >/dev/null 2>&1 &"
            ;;
            *)
            echo "no action"
            ;;
        esac
        ;;
    "kafka")
        case "$2" in
            "start")
            ansible -i hosts kafka -m shell -a "source /etc/profile ; nohup /root/kafka_2.12-2.0.0/bin/kafka-server-start.sh config/server.properties >/dev/null 2>&1 &"
            ;;
            "stop")
            ansible -i hosts kafka -m shell -a "source /etc/profile ; nohup /root/kafka_2.12-2.0.0/bin/kafka-server-stop.sh >/dev/null 2>&1 &"
            ;;
            "fstop")
            ansible -i hosts kafka -m shell -a "ps -ef |grep kafka |awk '{print \$2}'|xargs kill -9"
            ;;
            "status")
            ansible -i hosts kafka -m shell -a "ps -ef |grep kafka |awk '{print \$2}'"
            ;;
            *)
            echo "no action"
            ;;
        esac
        ;;
    "zoo")
        case "$2" in
            "start")
            ansible -i hosts kafka -m shell -a "source /etc/profile ; /root/zookeeper-3.4.13/bin/zkServer.sh start"
            ;;
            "stop")
            ansible -i hosts kafka -m shell -a "source /etc/profile ; /root/zookeeper-3.4.13/bin/zkServer.sh stop"
            ;;
            "status")
            ansible -i hosts kafka -m shell -a "source /etc/profile ; /root/zookeeper-3.4.13/bin/zkServer.sh status"
            ;;
            *)
            echo "no action"
            ;;
        esac
        ;;
    *)
        echo "no command"
        ;;
esac