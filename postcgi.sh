#!/bin/bash

# URL of the script
URL="http://localhost:8083/name.py"

# Data to send
DATA="name=Alice&age=25"

# Send POST request
curl -X POST "$URL" -d "$DATA"