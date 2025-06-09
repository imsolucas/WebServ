#!/usr/bin/env python3

import os

# Directory where uploaded files are saved (adjust if needed)
UPLOAD_DIR = os.path.abspath("../uploads")
WEB_PATH = "/uploads"

def list_uploaded_files():
    try:
        files = os.listdir(UPLOAD_DIR)
        files = sorted(files)
        return files
    except OSError:
        return []

def main():
    print("Content-Type: text/html\r\n")
    print("<html><head><title>Bug Reports</title>")
    print("<style>body{font-family:sans-serif;padding:2rem;background:#f9f9f9;}ul{list-style:none;padding:0;}li{margin:.5em 0;}</style>")
    print("</head><body>")
    print("<h1>📄 Uploaded Bug Reports</h1>")

    files = list_uploaded_files()
    if not files:
        print("<p>No reports uploaded yet.</p>")
    else:
        print("<ul>")
        for f in files:
            print(f'<li>🗂 <a href="{WEB_PATH}/{f}">{f}</a></li>')
        print("</ul>")

    print('<p><a href="/report/upload.html">⬆ Upload Another</a></p>')
    print("</body></html>")

if __name__ == "__main__":
    main()
