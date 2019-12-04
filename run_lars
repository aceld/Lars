#!/bin/bash

LARS_REPORTER_PATH="./lars_reporter"
LARS_DNS_PATH="./lars_dns"
LARS_LBAGENT_PATH="./lars_loadbalance_agent"
LARS_WEB_PATH="./larsWeb"
LARS_API_EXAMPLE_PATH="./api/cpp/example"
LARS_REACTOR_QPS_TEST_PATH="./lars_reactor/example/qps_test"

usage()
{
    echo ""
    echo "=======启动子系统=========="
    echo "Usage ./run_lars [reporter|dns|lbagent|web|test|reactor]"
    echo

    echo "=======Reactor测试工具============"
    echo "Usage ./run_lars reactor server"
    echo "Usage ./run_lars reactor qps [threadNum]"
    echo

    echo "=======LB Agent测试工具============"
    echo "Usage ./run_lars test gethost ModID CmdID"
    echo "Usage ./run_lars test getroute ModID CmdID"
    echo "Usage ./run_lars test report ModID CmdID IP Port 0|1  --- 0:SUCC, 1:OVERLOAD"
    echo "Usage ./run_lars test simulator ModID CmdID [errRate(0-10)] [queryCnt(0-999999)]"
    echo "Usage ./run_lars test qps ThreadNum"
    echo "Usage ./run_lars test example ModID CmdID"
    echo
}

if [ "$1" = "test" ]; then
    if [ "$2" = "gethost" ]; then
        $PWD/$LARS_API_EXAMPLE_PATH/get_host $3 $4 
    elif [ "$2" = "getroute" ]; then
        $PWD/$LARS_API_EXAMPLE_PATH/get_route $3 $4 
    elif [ "$2" = "report" ]; then
        $PWD/$LARS_API_EXAMPLE_PATH/report $3 $4 $5 $6 $7
    elif [ "$2" = "example" ]; then
        $PWD/$LARS_API_EXAMPLE_PATH/example $3 $4
    elif [ "$2" = "simulator" ]; then
        if [ $# -eq 4 ]; then
            $PWD/$LARS_API_EXAMPLE_PATH/simulator $3 $4 
        elif [ $# -eq 5 ]; then
            $PWD/$LARS_API_EXAMPLE_PATH/simulator $3 $4 $5
        elif [ $# -eq 6 ]; then
            $PWD/$LARS_API_EXAMPLE_PATH/simulator $3 $4 $5 $6
        else
            usage
        fi
    elif [ "$2" = "qps" ]; then
        $PWD/$LARS_API_EXAMPLE_PATH/qps $3
    fi
elif [ "$1" = "reactor" ]; then
    cd $LARS_REACTOR_QPS_TEST_PATH

    if [ "$2" = "server" ]; then
        ./server
    elif [ "$2" = "qps" ]; then
        ./client $3
    fi
elif [ "$1" = "reporter" ]; then
    cd $LARS_REPORTER_PATH
    ./bin/lars_reporter
elif [ "$1" = "dns" ]; then
    cd $LARS_DNS_PATH
    ./bin/lars_dns
elif [ "$1" = "lbagent" ]; then
    cd $LARS_LBAGENT_PATH
    ./bin/lars_lb_agent
elif [ "$1" = "web" ]; then
    cd $LARS_WEB_PATH
    ./lars-web
elif [ "$1" = "help" ]; then
    usage
else
    usage
fi
