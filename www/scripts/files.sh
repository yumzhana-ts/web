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

# Collect files
files=$(find "$TARGET_DIR" -maxdepth 1 -type f)
count=$(echo "$files" | wc -l)

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
    a { color:#66fcf1; text-decoration:none; }
    a:hover { text-decoration:underline; }
</style>
</head>
<body>
    <h1>📂 File Report</h1>
    <h2>Directory: $TARGET_DIR</h2>
    <p>Total files: <strong>$count</strong></p>
    <ul>
EOF

for f in $files; do
    filename=$(basename "$f")
    # Strip everything up to 'www/' to get web-accessible path
    relpath="${f#$WEB_ROOT/}"
    # URL-encode relative path
    urlpath=$(python3 -c "import urllib.parse; print(urllib.parse.quote('''$relpath'''))")
    echo "      <li><a href=\"/$urlpath\">$filename</a></li>"
done

cat <<EOF
    </ul>
</body>
</html>
EOF
