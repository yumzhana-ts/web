#!/bin/bash

# CGI header
echo "Status: 200 OK"
echo "Content-Type: text/html"
echo "X-Custom-Header: MyValue"
echo ""

# Web root folder
WEB_ROOT="/home/yu/shared/www"
TARGET_DIR="${TARGET_DIR_OVERRIDE:-$WEB_ROOT}"

# Make sure target exists
if [ ! -d "$TARGET_DIR" ]; then
    echo "<h1>Error: $TARGET_DIR not found</h1>"
    exit 1
fi

# Collect files safely
mapfile -t files < <(find "$TARGET_DIR" -maxdepth 1 -type f)

# HTML output
cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>File Counter</title>
<style>
    body { font-family: 'Courier New', monospace; background:#0b0c10; color:#66fcf1; text-align:center; padding:30px; }
    h1 { color:#45a29e; }
    ul { text-align:left; margin:auto; width:400px; background:#1f2833; padding:20px; border-radius:10px; }
    li { color:#f8c291; }
</style>
</head>
<body>
    <h1>📂 File Report</h1>
    <h2>Directory: $TARGET_DIR</h2>
    <p>Total files: <strong>${#files[@]}</strong></p>
    <ul>
EOF

for f in "${files[@]}"; do
    filename=$(basename "$f")
    echo "      <li>$filename</li>"
done

cat <<EOF
    </ul>
</body>
</html>
EOF
