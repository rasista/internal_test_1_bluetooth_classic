export BTC_UTILS=$PWD/../../utils
export BTC_TOOLS=$PWD/../../../tools
export HCI_JINJA_FILES=$WORKSPACE_PATH/src/lc/ulc/hci/jinja_files
export HCI_YAML=$WORKSPACE_PATH/src/lc/ulc/hci/yaml_files
export BTC_TOOLS=$WORKSPACE_PATH/tools
export HCI_UTILS=$WORKSPACE_PATH/ut_frameworks/py_utils
# Set compiler flags
CMAKE_C_FLAGS="-Werror -Wall"
 
cmake -B build/ -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS" -DUTILS=$BTC_UTILS -DTOOLS=$BTC_TOOLS -DHCI_JINJA_FILES=$HCI_JINJA_FILES -DHCI_YAML=$HCI_YAML -DTOOLS=$BTC_TOOLS -DHCI_UTILS=$HCI_UTILS