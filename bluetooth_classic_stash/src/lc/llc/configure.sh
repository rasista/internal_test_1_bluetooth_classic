export BTC_UTILS=$PWD/../../utils
export BTC_TOOLS=$PWD/../../../tools
cmake -B build/ -DUTILS=$BTC_UTILS -DTOOLS=$BTC_TOOLS -DPROCESSOR=RISCV
