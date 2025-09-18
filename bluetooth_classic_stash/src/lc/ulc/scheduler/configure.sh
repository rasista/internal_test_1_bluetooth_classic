export BTC_TOOLS=$PWD/../../../../tools
cmake -B build/  -DTOOLS=$BTC_TOOLS -DPROCESSOR=ARM-M55
