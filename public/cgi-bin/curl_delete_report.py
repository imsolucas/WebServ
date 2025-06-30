#!/usr/bin/env python3

import os
import sys
import urllib.parse

UPLOAD_DIR = os.path.abspath("../uploads")

print("Content-Type: text/plain\r\n")

try:
    method = os.environ.get("REQUEST_METHOD", "POST")
    if method not in ["POST", "DELETE"]:
        raise Exception(f"Unsupported method: {method}")

    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(length)
    params = urllib.parse.parse_qs(body)
    filename = params.get("file", [None])[0]

    if not filename:
        raise Exception("Missing 'file' parameter")

    safe_filename = os.path.basename(filename)
    path = os.path.join(UPLOAD_DIR, safe_filename)

    if not os.path.exists(path):
        raise Exception("File not found")

    os.remove(path)
    print(f"✅ Deleted {safe_filename}")
except Exception as e:
    print(f"❌ Error: {str(e)}")
