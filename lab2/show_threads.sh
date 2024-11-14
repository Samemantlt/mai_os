#!/bin/bash

# monitor_threads.sh

# Запускаем программу в фоне
./cmake-build-release/mai_os $((2 ** 25)) 32 &
PROG_PID=$!

echo "Program PID: $PROG_PID"
echo "Monitoring threads..."

# Мониторим потоки
while kill -0 $PROG_PID 2>/dev/null; do
    echo "$(date +%T%3N) - Number of threads: $(ls /proc/$PROG_PID/task | wc -l)"
done

wait $PROG_PID