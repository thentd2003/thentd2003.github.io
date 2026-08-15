#!/bin/bash

RED_BOLD='\033[1;31m'
YELLOW_BOLD='\033[1;33m'
GREEN_BOLD='\033[1;32m'
ORANGE_ITALIC='\033[38;5;214m\033[3m'
RESET='\033[0m'

# Function to handle SIGINT (Ctrl+C)
handle_sigint() {
    echo -e "${GREEN_BOLD}\nKudos!${RESET}"
    exit 0
}
trap handle_sigint SIGINT

# make sure output dirs exist (git drops empty directories)
mkdir -p code/ms1/out/R code/ms1/out/I code/ms1/out/LS code/ms2/out/R code/ms2/out/I code/ms2/out/LS code/ms3/out/LS code/ms4/out


# Functionality test: full per-cycle trace + cache traces on a small program,
# checks the L1-I/L1-D/L2 hierarchy doesn't break correctness vs the golden reference
run_functionality() {
    echo -e "${RED_BOLD}Set config.h to the MS4 block, full trace + cache traces (see config.h comments), then make clean && make${RESET}"

    ./riscv -s -f -c ./code/ms3/input/LS/LS.input > ./code/ms4/out/LS.two_level.trace
    echo "diff ./code/ms4/ref/LS.two_level.trace ./code/ms4/out/LS.two_level.trace"
    diff       ./code/ms4/ref/LS.two_level.trace ./code/ms4/out/LS.two_level.trace
}

# Performance test: stats only, compares total cycles for vec_xprod with the
# two-level cache on vs the MS3 single-level L1-only cache, to show the L2 payoff
run_performance() {
    echo -e "${YELLOW_BOLD}Set config.h to the MS4 block, stats only (comment out DEBUG_REG_TRACE/DEBUG_CYCLE/PRINT_CACHE_TRACES), then make clean && make${RESET}"

    ./riscv -s -f -e ./code/ms3/input/vec_xprod.input > ./code/ms4/out/vec_xprod.two_level.stats
    echo "diff ./code/ms4/ref/vec_xprod.two_level.stats ./code/ms4/out/vec_xprod.two_level.stats"
    diff       ./code/ms4/ref/vec_xprod.two_level.stats ./code/ms4/out/vec_xprod.two_level.stats

    echo -e "${ORANGE_ITALIC}Compare against ./code/ms4/ref/vec_xprod.single_level.stats (generated with MS3's CACHE_ENABLE-only config) for the before/after cycle count.${RESET}"
}

# Corner case test: stats only, scattered addresses (random.input) to stress
# eviction handling at both L1 and L2 at once
run_corner() {
    echo -e "${YELLOW_BOLD}Set config.h to the MS4 block, stats only, then make clean && make${RESET}"

    ./riscv -s -f -e ./code/ms3/input/random.input > ./code/ms4/out/random.two_level.stats
    echo "diff ./code/ms4/ref/random.two_level.stats ./code/ms4/out/random.two_level.stats"
    diff       ./code/ms4/ref/random.two_level.stats ./code/ms4/out/random.two_level.stats
}

# ---------------------------------------------------------------------------
# Branch predictor tests (Feature 2). bp1_* = BRANCH_PREDICT_1BIT,
# bp2_* = BRANCH_PREDICT_2BIT. Both run on top of the MS4 two-level block.
# ---------------------------------------------------------------------------

# Functionality: full trace on multiply (loop-heavy, 16 conditional branches),
# checks the [BP ] predict/correct/mispredict lines and that redirection on a
# misprediction keeps the register trace identical to the reference.
run_bp_functionality() {
    local N=$1
    echo -e "${RED_BOLD}Set config.h to the MS4 block + BRANCH_PREDICT_${N}BIT, full trace (DEBUG_REG_TRACE/DEBUG_CYCLE/PRINT_CACHE_TRACES on), then make clean && make${RESET}"

    ./riscv -s -f -c -e ./code/ms3/input/multiply.input > ./code/ms4/out/multiply.bp${N}.trace
    echo "diff ./code/ms4/ref/multiply.bp${N}.trace ./code/ms4/out/multiply.bp${N}.trace"
    diff       ./code/ms4/ref/multiply.bp${N}.trace ./code/ms4/out/multiply.bp${N}.trace
}

# Performance: stats only on vec_xprod (78k taken branches). Compare #Cycles
# and #BP accuracy against ./code/ms4/ref/vec_xprod.two_level.stats (predictor
# off) to see the flush cycles the predictor saves; compare bp1 vs bp2 to see
# the saturating counter's edge on loop exits.
run_bp_performance() {
    local N=$1
    echo -e "${YELLOW_BOLD}Set config.h to the MS4 block + BRANCH_PREDICT_${N}BIT, stats only, then make clean && make${RESET}"

    ./riscv -s -f -e ./code/ms3/input/vec_xprod.input > ./code/ms4/out/vec_xprod.bp${N}.stats
    echo "diff ./code/ms4/ref/vec_xprod.bp${N}.stats ./code/ms4/out/vec_xprod.bp${N}.stats"
    diff       ./code/ms4/ref/vec_xprod.bp${N}.stats ./code/ms4/out/vec_xprod.bp${N}.stats

    echo -e "${ORANGE_ITALIC}Compare #Cycles/#BP accuracy against ./code/ms4/ref/vec_xprod.two_level.stats (predictor off) and the other bp variant.${RESET}"
}

# Corner case: stats only on random.input -- branches with irregular outcomes,
# stressing mispredict-path flush + redirect (both wrong-taken and wrong-not-taken).
run_bp_corner() {
    local N=$1
    echo -e "${YELLOW_BOLD}Set config.h to the MS4 block + BRANCH_PREDICT_${N}BIT, stats only, then make clean && make${RESET}"

    ./riscv -s -f -e ./code/ms3/input/random.input > ./code/ms4/out/random.bp${N}.stats
    echo "diff ./code/ms4/ref/random.bp${N}.stats ./code/ms4/out/random.bp${N}.stats"
    diff       ./code/ms4/ref/random.bp${N}.stats ./code/ms4/out/random.bp${N}.stats
}

# Check the first command-line argument and run the corresponding function
case $1 in
    two_level_functionality)
        run_functionality
        ;;
    two_level_performance)
        run_performance
        ;;
    two_level_corner)
        run_corner
        ;;
    bp1_functionality)
        run_bp_functionality 1
        ;;
    bp1_performance)
        run_bp_performance 1
        ;;
    bp1_corner)
        run_bp_corner 1
        ;;
    bp2_functionality)
        run_bp_functionality 2
        ;;
    bp2_performance)
        run_bp_performance 2
        ;;
    bp2_corner)
        run_bp_corner 2
        ;;
    *)
        echo "Usage: $0 {two_level_functionality|two_level_performance|two_level_corner|bp1_functionality|bp1_performance|bp1_corner|bp2_functionality|bp2_performance|bp2_corner}"
        ;;
esac
