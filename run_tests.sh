#!/bin/bash

PROG="./rls_analyze"
GEN="./gen_test.sh"
TEST_DIR="test_data"
TEST_DATA_FILE="test_data.csv"
TEST_EMPTY_FILE="test_empty.csv"
TEST_MINMARKS_DATA_FILE="test_minmarks.csv"
PASSED=0
TOTAL=0
FAILED_TESTS=""

run_test() {
    local command=$1
    local code=$2
    local message=$3
    local search=$4
    local count=$5
    ((TOTAL++))

    local output
    output=$(eval "$command" 2>/dev/null)
    local res=$?

    if [ $res -ne $code ];
    then
        echo "FAIL: $message (ожидался код $code, получен $res)"
        FAILED_TESTS+=" - $message (код)\n"
        return
    fi

    if [ -n "$search" ];
    then
        if ! echo "$output" | grep -q "$search";
        then
            echo "FAIL: $message (строка '$search' не найдена)"
            FAILED_TESTS+=" - $message (поиск строки)\n"
            return
        fi
    fi

    if [ -n "$exp_count" ];
    then
        local c
        c=$(echo "$output" | grep -E "^[0-9]" | wc -l)
        if [ "$c" -ne "$count" ];
        then
            echo "FAIL: $message (ожидалось целей: $count, найдено: $c)"
            FAILED_TESTS+=" - $message (кол-во целей)\n"
            return
        fi
    fi

    echo "PASS: $message"
    ((PASSED++))
}

if [ ! -f "$PROG" ];
then
    echo "Ошибка: $PROG не найден. Соберите программу."
    exit 1
fi
mkdir -p "$TEST_DIR"
touch "$TEST_DIR/$TEST_EMPTY_FILE"

$GEN 5 5 $TEST_DATA_FILE 123 > /dev/null
$GEN 5 10 $TEST_MINMARKS_DATA_FILE 123 --scenario minmarks > /dev/null

run_test "$PROG $TEST_DIR/no_file.csv" 1 "Несуществующий файл"
run_test "$PROG $TEST_DIR/$TEST_EMPTY_FILE" 2 "Пустой файл"
run_test "$PROG $TEST_DIR/$TEST_DATA_FILE --top abc" 3 "Неверный --top"
run_test "$PROG $TEST_DIR/$TEST_DATA_FILE --sector 100" 3 "Неполный --sector"
run_test "$PROG $TEST_DIR/$TEST_DATA_FILE --top 2" 0 "Проверка вывода ближайшей цели" "Ближайшая цель"
run_test "$PROG" 3 "Запуск без аргументов"

run_test "$PROG $TEST_DIR/$TEST_MINMARKS_DATA_FILE --min-marks 3" 0 "Фильтр в заголовке" "Фильтр: мин. отметок"
run_test "$PROG $TEST_DIR/$TEST_MINMARKS_DATA_FILE --min-marks 3" 0 "Порог 3 (3 цели)" "" 3
run_test "$PROG $TEST_DIR/$TEST_MINMARKS_DATA_FILE --min-marks 5" 0 "Порог 5 (2 цели)" "" 2
run_test "$PROG $TEST_DIR/$TEST_MINMARKS_DATA_FILE --min-marks abc" 3 "Неверный аргумент --min-marks"

echo "-------------------------------------------"

if [ "$PASSED" -eq "$TOTAL" ];
then
    echo "Все тесты пройдены ($PASSED/$TOTAL)"
    exit 0
else
    echo "Тестов провалено: $((TOTAL - PASSED)) из $TOTAL"
    printf "$FAILED_TESTS"
    exit 1
fi
