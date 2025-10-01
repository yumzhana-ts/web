#!/bin/bash

# URL of the script
URL="http://localhost:8081/cgi-bin/script.php"

# Data to send
DATA="name=Alice&age=25"

# Send POST request
curl -X POST "$URL" -d "$DATA"