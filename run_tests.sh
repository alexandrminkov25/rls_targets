#!/bin/bash

PROG="./rls_analyze.exe"
PASSED=0
TOTAL=0
FAILED_TESTS=""

run_test() {
    local command="$1"
    local code=$2
    local message=$3
    local search=$4
    ((TOTAL++))

    if [ -n "$search" ];
    then
        if eval "$command" 2>/dev/null | grep -q "$search";
        then
            echo "PASS: $message"
            ((PASSED++))
        else
            echo "FAIL: $message"
            FAILED_TESTS+=" - $message\n"
        fi
    else
        eval "$command" > /dev/null 2>&1
        local result=$?
        if [ $result -eq $code ];
        then
            echo "PASS: $message (код $code)"
            ((PASSED++))
        else
            echo "FAIL: $message (ожидался $code, получен $result)"
            FAILED_TESTS+=" - $message\n"
        fi
    fi
}

run_test "$PROG no_file.csv" 1 "Несуществующий файл"
run_test "$PROG test_data/empty.csv" 2 "Пустой файл"
run_test "$PROG test_data/data.csv --top abc" 3 "Неверный --top"
run_test "$PROG test_data/data.csv --sector 100" 3 "Неполный --sector"
run_test "$PROG test_data/data.csv --top 2" 0 "Ближайшая цель" "Ближайшая цель"
run_test "$PROG" 3 "Запуск без аргументов"

echo "-------------------------------------------"

if [ "$PASSED" -eq "$TOTAL" ]; then
    echo "Все тесты пройдены ($PASSED/$TOTAL)"
    exit 0
else
    echo "Тестов провалено: $((TOTAL - PASSED)) из $TOTAL"
    printf "$FAILED_TESTS"
    exit 1
fi
