#!/bin/bash

# Путь к изображению
IMAGE_PATH="./image.jpg"

# URL сервера
URL="http://localhost:8081/database/"

# Получаем размер файла
FILE_SIZE=$(stat -c%s "$IMAGE_PATH")

# Отправка файла как raw POST с правильными заголовками
curl -v -X POST \
     --data-binary @"${IMAGE_PATH}" \
     -H "Content-Type: image/jpeg" \
     -H "Content-Length: ${FILE_SIZE}" \
     "$URL"
