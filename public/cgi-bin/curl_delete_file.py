#!/usr/bin/env python3

import os
import sys
import urllib.parse

UPLOAD_DIR = os.path.abspath("../uploads")

# Only DELETE method is allowed for this script
if os.environ.get("REQUEST_METHOD", "") != "DELETE":
    print("Status: 405 Method Not Allowed")
    print("Content-Type: text/plain\r\n")
    print("Only DELETE method is allowed.")
    sys.exit(0)

try:
	# Read the body: expected format is 'file=filename'
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(length)
    params = urllib.parse.parse_qs(body)
    filename = params.get("file", [None])[0]

	# Check for valid filename
    if not filename:
        print("Status: 400 Bad Request")
        print("Content-Type: text/plain\r\n")
        print("Missing 'file' parameter")
        sys.exit(0)

    # Sanitize filename to prevent directory traversal
    safe_filename = os.path.basename(filename)
    path = os.path.join(UPLOAD_DIR, safe_filename)

    # Check for existence of the file
    if not os.path.isfile(path):
        print("Status: 404 Not Found")
        print("Content-Type: text/plain\r\n")
        print(f"File '{safe_filename}' not found.")
        sys.exit(0)

    # Delete the file
    os.remove(path)
    print("Status: 200 OK")
    print("Content-Type: text/plain\r\n")
    print(f"Deleted {safe_filename}")

except Exception as e:
    print("Status: 500 Internal Server Error")
    print("Content-Type: text/plain\r\n")
    print(f"Error: {str(e)}")
