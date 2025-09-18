#!/bin/bash

# environment variable for path
COVERAGE_REPORTS_DIR="$WORKSPACE_PATH/src/lc/coverage_reports"

# Create the coverage reports directory if it doesn't exist
mkdir -p "$COVERAGE_REPORTS_DIR"

# Generate the coverage report for specific files and remove simulation muck up from the report
generate_coverage() {
    local comp=$1
    local other_comp=$([ "$comp" = "llc" ] && echo "ulc" || echo "llc")
    echo "Generating coverage report for $comp..."
    gcovr -r $WORKSPACE_PATH --html --html-details -o "$COVERAGE_REPORTS_DIR/coverage_$comp.html" --exclude "$WORKSPACE_PATH/bin/lpw_user_intf_json2c.c" --exclude "$WORKSPACE_PATH/ut_frameworks/" --exclude "$WORKSPACE_PATH/simpy_intf_binaries/" --exclude "$WORKSPACE_PATH/bin/btc_${other_comp}_binaries/" --exclude "$WORKSPACE_PATH/bin/CMakeFiles/" --exclude "$WORKSPACE_PATH/bin/jansson_binaries/" --gcov-exclude '.+_simulation\.\w+' --filter "$WORKSPACE_PATH/src/lc/$comp/"
}

# Generate coverage for both components
generate_coverage $1