#!/bin/bash

SERVER="http://127.0.0.1:8083"

echo "=== Тест GET / ==="
curl -s -o /dev/null -w "%{http_code}\n" "$SERVER/"
curl -s -X POST -o /dev/null -w "%{http_code} (ожидаем 405)\n" "$SERVER/"

echo "=== Тест PUT /put_test/testfile.txt ==="
echo "hello put" > tmp.txt
curl -s -X PUT --data-binary @tmp.txt -H "Content-Type: text/plain" -o /dev/null -w "%{http_code}\n" "$SERVER/put_test/testfile.txt"
rm tmp.txt

