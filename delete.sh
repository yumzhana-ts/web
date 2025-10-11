#!/bin/bash

# ID изображения для удаления
IMAGE_ID="1"

# URL сервера с query-параметром
URL="http://localhost:8083?image_id=${IMAGE_ID}"

# Отправка запроса DELETE
curl -v -X DELETE "$URL"