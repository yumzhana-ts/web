#!/usr/bin/env python3
import os
import sys

# Read POST body (if any)
length = int(os.environ.get("CONTENT_LENGTH", 0))
body = sys.stdin.read(length) if length > 0 else ""

print("Content-type: text/html\n")

print("""
<html>
<head>
  <title>Enter Name</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #111;
      color: #fff;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      flex-direction: column;
    }
    input[type=text] {
      font-size: 2em;
      padding: 10px;
      width: 300px;
      text-transform: uppercase;
    }
    input[type=submit] {
      font-size: 1.5em;
      padding: 10px 20px;
      margin-top: 20px;
      cursor: pointer;
    }
    h1 {
      font-size: 4em;
      margin-bottom: 50px;
    }
  </style>
</head>
<body>
  <h1>Registration</h1>
  <form method="POST">
    <input type="text" name="name" placeholder="Your name">
    <input type="submit" value="Submit">
  </form>
</body>
</html>
""")
