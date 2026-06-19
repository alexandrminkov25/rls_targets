#!/bin/bash

printf_row() {
    local output_file=$1
    local time_ms=${2:-""}
    local target_id=${3:-""}
    local range_m=${4:-""}
    local azimuth_deg=${5:-""}
    printf "%-10s %-10s %-8s %s\n" "$time_ms" "$target_id" "$range_m" "$azimuth_deg" >> "$output_file"
}

generate_random() {
    RAND_VAL=$((RANDOM % ($2 - $1 + 1) + $1))
}

generate_random_float() {
    generate_random $1 $2
    RAND_FLOAT=$(printf "%d.%d" $((RAND_VAL / 10)) $((RAND_VAL % 10)))
}

generate_random_error() {
    generate_random 1 9
    local time_ms=$((RAND_VAL * 1000))

    generate_random 1 255
    local target_id=$RAND_VAL

    generate_random 256 300
    local error_target_id=$RAND_VAL

    generate_random 1000 50000
    local range_m=$RAND_VAL

    generate_random -50000 -1000
    local error_range_m=$RAND_VAL

    generate_random_float 0 3599
    local azimuth_deg=$RAND_FLOAT
    
    generate_random_float 3600 3999
    local error_azimuth_deg=$RAND_FLOAT

    generate_random 1 4
    case $RAND_VAL in
        1) printf_row "$OUTPUT_FILE" "$time_ms" "$target_id" ;;
        2) printf_row "$OUTPUT_FILE" "$time_ms" "$error_target_id" "$range_m" "$azimuth_deg" ;;
        3) printf_row "$OUTPUT_FILE" "$time_ms" "$target_id" "$error_range_m" "$azimuth_deg" ;;
        4) printf_row "$OUTPUT_FILE" "$time_ms" "$target_id" "$range_m" "$error_azimuth_deg" ;;
    esac
}

if [[ $# -lt 3 ]];
then
    echo "Ошибка: Недостаточно аргументов"
    echo "Использование: $0 <число_целей> <отметок_на_цель> <выходной_файл> [seed]"
    exit 1
fi

TARGETS_COUNT=$1
POINTS_PER_TARGET=$2
OUTPUT_FILE=$3
SEED=${4:-$$}
RANDOM=$SEED

OUTPUT_DIR="test_data"
mkdir -p $OUTPUT_DIR
OUTPUT_FILE="$OUTPUT_DIR/$OUTPUT_FILE"

if [[ ! "$TARGETS_COUNT" =~ ^[0-9]+$ ]];
then
    echo "Ошибка: Число целей должно быть положительным целым числом"
    exit 1
fi

if [[ ! "$POINTS_PER_TARGET" =~ ^[0-9]+$ ]];
then
    echo "Ошибка: Число отметок на цель должно быть положительным целым числом"
    exit 1
fi

if [[ "$OUTPUT_FILE" != *.csv ]];
then
    echo "Ошибка: Файл '$OUTPUT_FILE' имеет неверное расширение. Ожидается .csv"
    exit 1
fi

echo "Генерация данных для $TARGETS_COUNT целей с $POINTS_PER_TARGET отметками на цель в файл '$OUTPUT_FILE'"
echo "Используется seed: $SEED"

printf "# time_ms  target_id  range_m  azimuth_deg\n" > "$OUTPUT_FILE"

generate_random 3 5;
ERRORS_COUNT=$RAND_VAL

generate_random 2 3;
DUPLICATES_COUNT=$RAND_VAL

for (( t=1; t<=TARGETS_COUNT; t++ ))
do
    generate_random 1 255;
    TARGET_ID=$RAND_VAL

    for (( p=1; p<=POINTS_PER_TARGET; p++ ))
    do
        TIME_MS=$(( p * 1000 ))

        generate_random 1000 50000;
        RANGE_M=$RAND_VAL

        generate_random_float 0 3599;
        AZIMUTH_DEG=$RAND_FLOAT

        printf_row "$OUTPUT_FILE" "$TIME_MS" "$TARGET_ID" "$RANGE_M" "$AZIMUTH_DEG"
        LAST_ROW=("$TIME_MS" "$TARGET_ID" "$RANGE_M" "$AZIMUTH_DEG")

        generate_random 1 100
        if [[ $ERRORS_COUNT -gt 0 && $RAND_VAL -le 40 ]];
        then
            generate_random_error >> "$OUTPUT_FILE"
            ((ERRORS_COUNT--))
        fi

        generate_random 1 100
        if [[ $DUPLICATES_COUNT -gt 0 && $RAND_VAL -le 40 ]];
        then
            generate_random -500 500
            NEW_RANGE_M=$((LAST_ROW[2] + RAND_VAL))

            printf_row "$OUTPUT_FILE" "${LAST_ROW[0]}" "${LAST_ROW[1]}" "${NEW_RANGE_M}" "${LAST_ROW[3]}"
            ((DUPLICATES_COUNT--))
        fi
    done
done

while [[ $ERRORS_COUNT -gt 0 ]];
do
    generate_random_error >> "$OUTPUT_FILE"
    ((ERRORS_COUNT--))
done

while [[ $DUPLICATES_COUNT -gt 0 ]];
do
    printf_row "$OUTPUT_FILE" "${LAST_ROW[0]}" "${LAST_ROW[1]}" "${LAST_ROW[2]}" "${LAST_ROW[3]}"
    ((DUPLICATES_COUNT--))
done
