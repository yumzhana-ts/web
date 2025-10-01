#!/bin/bash

# File to check
REPORT="valgrind_report.txt"

if [ -z "$REPORT" ]; then
    echo -e "\033[1;33mUsage: $0 <valgrind_report.txt>\033[0m"
    exit 1
fi

if [ ! -f "$REPORT" ]; then
    echo -e "\033[1;31m❌ File '$REPORT' does not exist.\033[0m"
    exit 1
fi

echo -e "\n\033[1;34m==============================================================\033[0m"
echo -e "\033[1;35m    🐛🔍 VALGRIND MEMORY CHECK 🔍🐛\033[0m"
echo -e "\033[1;34m==============================================================\033[0m\n"

# Check for "no leaks" message
if grep -q "All heap blocks were freed -- no leaks are possible" "$REPORT"; then
    echo -e "\033[1;32m✅✨ No memory leaks detected! ✨✅\033[0m"
else
    echo -e "\033[1;31m⚠️🔥 Memory leaks found! 🔥⚠️\033[0m"
    echo -e "\033[1;36m--------------------------------------------------------------\033[0m"
    cat "$REPORT"
    echo -e "\033[1;36m--------------------------------------------------------------\033[0m"
    echo -e "\033[1;31m💥 Fix the leaks before proceeding! 💥\033[0m"
fi

# Check for other Valgrind errors
ERROR_LINES=$(grep -E "Invalid read|Invalid write|Use of uninitialised value|Conditional jump or move depends on uninitialised value|Syscall param" "$REPORT")
if [ -n "$ERROR_LINES" ]; then
    echo -e "\n\033[1;31m🚨 Other Valgrind errors detected! 🚨\033[0m"
    echo -e "\033[1;36m--------------------------------------------------------------\033[0m"
    echo "$ERROR_LINES"
    echo -e "\033[1;36m--------------------------------------------------------------\033[0m"
    echo -e "\033[1;31m⚠️ Make sure to fix these issues too! ⚠️\033[0m"
else
    echo -e "\033[1;32m🎉 No other Valgrind errors found. Your memory handling looks solid! 🎉\033[0m"
fi

echo -e "\n\033[1;34m======================== END OF MEMORY CHECK ==================\033[0m\n"
