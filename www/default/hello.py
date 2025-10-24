#!/usr/bin/env python3
import os

# Get name from HTTP header
name = os.environ.get("HTTP_NAME", "")

print("Content-type: text/html\n")
print(f"""
<html>
<head>
  <title>Hello</title>
  <style>
    body {{
      font-family: Arial, sans-serif;
      background: #111;
      color: #fff;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }}
    h1 {{
      font-size: 6em;
      text-transform: uppercase;
    }}
  </style>
</head>
<body>
  <h1>Hello {name if name else "Stranger"}!</h1>
</body>
</html>
""")
