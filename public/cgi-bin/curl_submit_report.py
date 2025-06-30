#!/usr/bin/env python3

import os
import sys
import cgi

UPLOAD_DIR = os.path.abspath("../public/uploads")

print("Content-Type: text/plain\r\n")

try:
    form = cgi.FieldStorage()

    filename = form.getvalue("filename")
    content = form.getvalue("content")

    if not filename or not content:
        raise Exception("Missing filename or content.")

    safe_filename = os.path.basename(filename)
    filepath = os.path.join(UPLOAD_DIR, safe_filename)

    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)

    with open(filepath, "w") as f:
        f.write(content)

    print(f"✅ Saved as: {safe_filename}")
except Exception as e:
    print(f"❌ Error: {str(e)}")
