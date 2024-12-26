#!/usr/bin/env bash

EXEC=semtest
DUMMY=test
OUTPUT=output
TEST_DIR=tests
TIMEOUT=1s
DIFF_FILE=d

PASSING=0
LEAKING=0

RED='\033[41;37m'
GREEN='\033[42m'
YELLOW='\033[43m'
RESET='\033[0m'

#export ASAN_OPTIONS="detect_leaks=false"
export LSAN_OPTIONS="exitcode=23"
export MallocNanoZone="0"

make clean && make

for T in $(ls $TEST_DIR | grep -E "$1" | sort)
do
    PASS=1
    LEAK=1
    for F in $(ls $TEST_DIR/$T | grep ".bluejay$" | grep pass | sort)
    do
        echo -n -e "Running ${GREEN}positive test${RESET} $T/$F"
        echo -n $'\t'
        TESTFILE="$TEST_DIR/$T/$F"
        EXPECTED=$(sed 's/.bluejay/.expected/g' <<<"$TESTFILE")
        cp $TESTFILE $DUMMY
        mv $EXEC.exe $EXEC &> /dev/null
        ASAN_OPTIONS="detect_leaks=false" ./$EXEC $DUMMY &> $OUTPUT
        RET=$?
        ./$EXEC $DUMMY &> /dev/null
        RETLEAK=$?
        dos2unix $OUTPUT &> /dev/null
        dos2unix $EXPECTED &> /dev/null
        diff $EXPECTED $OUTPUT &> $DIFF_FILE
        DIFF=$?
        rm $DUMMY $OUTPUT

        if [ $RET -eq 0 ]
        then
            echo -e -n "${GREEN}RET OK${RESET}"
        else
            echo -e -n "${RED}RET FAIL${RESET}"
        fi

        if [ $RETLEAK -eq 23 ]
        then
            echo -e -n " ${YELLOW}LEAKS FAIL${RESET}"
            LEAK=0
        else
            echo -e -n " ${GREEN}LEAKS OK${RESET}"
        fi


        if [ $DIFF -eq 0 ]
        then
            echo -e " ${GREEN}DIFF OK${RESET}"
        else
            echo -e " ${RED}DIFF FAIL${RESET}"
            cat $DIFF_FILE
        fi

        if [ $RET -ne 0 ] || [ $DIFF -ne 0 ]
        then
            PASS=0
            #continue 2
        fi
    done

    for F in $(ls $TEST_DIR/$T |  grep ".bluejay$" |grep fail | sort)
    do
        echo -n -e "Running ${RED}negative test${RESET} $T/$F"
        echo -n $'\t'
        TESTFILE="$TEST_DIR/$T/$F"
        cp $TESTFILE $DUMMY
        (ASAN_OPTIONS="detect_leaks=false" ./$EXEC $DUMMY &> /dev/null)
        RET=$?
        $(./$EXEC $DUMMY &> /dev/null)
        RETLEAK=$?

        if [ $RET -eq 73 ]
        then
            echo -e -n "${GREEN}RET OK${RESET}"
        else
            echo -e -n "${RED}RET FAIL${RESET}"
            #continue 2
            PASS=0
        fi

        if [ $RETLEAK -eq 23 ]
        then
            echo -e " ${YELLOW}LEAKS FAIL${RESET}"
            LEAK=0
        else
            echo -e " ${GREEN}LEAKS OK${RESET}"
        fi


        rm $DUMMY
    done

    if [ $PASS -eq 0 ]
    then
            echo -e "${RED}                   TEST ${T} FAILING                  ${RESET}"
            echo -e "${RED}                   TEST ${T} FAILING                  ${RESET}"
            echo -e "${RED}                   TEST ${T} FAILING                  ${RESET}"
    elif [ $LEAK -eq 0 ]
    then
            echo -e "${YELLOW}                   TEST ${T} PASSING WITH MEM LEAKS               ${RESET}"
            echo -e "${YELLOW}                   TEST ${T} PASSING WITH MEM LEAKS               ${RESET}"
            echo -e "${YELLOW}                   TEST ${T} PASSING WITH MEM LEAKS               ${RESET}"
    else
            echo -e "${GREEN}                   TEST ${T} PASSING               ${RESET}"
            echo -e "${GREEN}                   TEST ${T} PASSING               ${RESET}"
            echo -e "${GREEN}                   TEST ${T} PASSING               ${RESET}"
            LEAKING=$(($LEAKING+1))
    fi

    PASSING=$(($PASSING+$PASS))
done

echo $LEAKING > leak.txt

echo $PASSING
