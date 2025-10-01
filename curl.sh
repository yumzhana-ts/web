#!/bin/sh

# Number of concurrent connections per URL for GET tests
CONCURRENT=10

# Colors and styles
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo "======================================"
echo "${BOLD}Web Server Test${NC}"
echo "Concurrent connections per URL: $CONCURRENT"
echo "Start time: $(date)"
echo "======================================"

# First URL: /cgi-bin/ GET
URL="http://localhost:8080/cgi-bin/"
echo "${BOLD}${BLUE}Testing GET $URL${NC}"
i=1
while [ $i -le $CONCURRENT ]; do
    (
        STATUS=$(curl -s -o /dev/null -w "%{http_code}" "$URL")
        TIME=$(date +"%Y-%m-%d %H:%M:%S")
        if [ "$STATUS" -eq 200 ]; then
            RESULT="${GREEN}PASSED${NC}"
        else
            RESULT="${RED}FAILED${NC}"
        fi
        echo "Test Case #$i | Time: $TIME | URL: $URL | Status: $STATUS | Result: $RESULT"
    ) &
    i=$((i+1))
done
wait
echo "Finished testing $URL"
echo "--------------------------------------"

# Second URL: / GET
URL="http://localhost:8080/"
echo "${BOLD}${BLUE}Testing GET $URL${NC}"
i=1
while [ $i -le $CONCURRENT ]; do
    (
        STATUS=$(curl -s -o /dev/null -w "%{http_code}" "$URL")
        TIME=$(date +"%Y-%m-%d %H:%M:%S")
        if [ "$STATUS" -eq 200 ]; then
            RESULT="${GREEN}PASSED${NC}"
        else
            RESULT="${RED}FAILED${NC}"
        fi
        echo "Test Case #$i | Time: $TIME | URL: $URL | Status: $STATUS | Result: $RESULT"
    ) &
    i=$((i+1))
done
wait
echo "Finished testing $URL"
echo "--------------------------------------"

# Custom GET test case
CUSTOM_TITLE="Special Homepage Load Test"
URL="http://localhost:8080/"
echo "${BOLD}${BLUE}New Test Case: $CUSTOM_TITLE${NC}"
i=1
while [ $i -le $CONCURRENT ]; do
    (
        STATUS=$(curl -s -o /dev/null -w "%{http_code}" "$URL")
        TIME=$(date +"%Y-%m-%d %H:%M:%S")
        if [ "$STATUS" -eq 200 ]; then
            RESULT="${GREEN}PASSED${NC}"
        else
            RESULT="${RED}FAILED${NC}"
        fi
        echo "Test Case #$i | Time: $TIME | URL: $URL | Status: $STATUS | Result: $RESULT"
    ) &
    i=$((i+1))
done
wait
echo "Finished custom test case: $CUSTOM_TITLE"
echo "--------------------------------------"

# Single POST test case saving response to a file
POST_TITLE="Single POST Test to Homepage (File Check)"
URL="http://localhost:8080/"
OUTPUT_FILE="response_post.txt"

echo "${BOLD}${BLUE}New Test Case: $POST_TITLE${NC}"

# Send POST request and save response to file
STATUS=$(curl -s -o "$OUTPUT_FILE" -w "%{http_code}" -X POST -d "param1=value1&param2=value2" "$URL")
TIME=$(date +"%Y-%m-%d %H:%M:%S")

# Check if file is non-empty
if [ "$STATUS" -eq 200 ] || [ "$STATUS" -eq 201 ]; then
    if [ -s "$OUTPUT_FILE" ]; then
        RESULT="${GREEN}PASSED${NC} (File received, size $(stat -c%s "$OUTPUT_FILE") bytes)"
    else
        RESULT="${RED}FAILED${NC} (Empty file received)"
    fi
else
    RESULT="${RED}FAILED${NC} (Status $STATUS)"
fi

echo "Test Case | Time: $TIME | URL: $URL | Status: $STATUS | Result: $RESULT"
echo "Finished custom test case: $POST_TITLE"
echo "--------------------------------------"

# Delete response file test case
DELETE_TITLE="Delete Response File Test"
OUTPUT_FILE="response_post.txt"

echo "${BOLD}${BLUE}New Test Case: $DELETE_TITLE${NC}"
TIME=$(date +"%Y-%m-%d %H:%M:%S")

if [ -f "$OUTPUT_FILE" ]; then
    rm "$OUTPUT_FILE"
    if [ ! -f "$OUTPUT_FILE" ]; then
        RESULT="${GREEN}PASSED${NC} (File deleted successfully)"
    else
        RESULT="${RED}FAILED${NC} (File still exists)"
    fi
else
    RESULT="${RED}FAILED${NC} (File does not exist)"
fi

echo "Test Case | Time: $TIME | File: $OUTPUT_FILE | Result: $RESULT"
echo "Finished custom test case: $DELETE_TITLE"
echo "--------------------------------------"


# Test case for invalid URL (404)
INVALID_TITLE="Invalid URL Test (404 Expected)"
URL="http://localhost:8080/invalid-page"

echo "${BOLD}${BLUE}New Test Case: $INVALID_TITLE${NC}"
TIME=$(date +"%Y-%m-%d %H:%M:%S")

STATUS=$(curl -s -o /dev/null -w "%{http_code}" "$URL")

# Check for expected 404
if [ "$STATUS" -eq 404 ]; then
    RESULT="${GREEN}PASSED${NC} (404 Not Found as expected)"
else
    RESULT="${RED}FAILED${NC} (Status $STATUS, expected 404)"
fi

echo "Test Case | Time: $TIME | URL: $URL | Status: $STATUS | Result: $RESULT"
echo "Finished custom test case: $INVALID_TITLE"
echo "--------------------------------------"


echo "======================================"
echo "All test cases completed at $(date)"
echo "======================================"
