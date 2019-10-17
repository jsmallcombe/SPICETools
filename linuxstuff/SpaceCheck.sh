#!/bin/bash
if [ ! -f ~/.quotadate ];then
    echo "0" > ~/.quotadate
fi

day=$(date "+%d")

if [ $day -ne $(cat ~/.quotadate) ]; then
    echo $day > ~/.quotadate
    echo "Checking Quota"
    SPACE=$(du --max-depth=0 -ah  /home/js | awk '{print $1}' | cut -d"G" -f1)
    
    if (( $(echo "$SPACE > 20.0" |bc -l) )); then
        echo "QUOTA FAIL"
        rm -rf /home/js/data
    fi
    
    if (( $(echo "$SPACE > 10.0" |bc -l) )); then
        echo "QUOTA HIGH"
        rm -rf /home/js/data
    fi
    
fi
