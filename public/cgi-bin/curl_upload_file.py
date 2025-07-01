#!/usr/bin/env python3

import os
import sys
import cgi

# Define upload directory (absolute path)
UPLOAD_DIR = os.path.abspath("../uploads")

# Only POST method is allowed for this script
if os.environ.get("REQUEST_METHOD", "") != "POST":
    print("Status: 405 Method Not Allowed")
    print("Content-Type: text/plain\r\n")
    print("Only POST method is allowed.")
    sys.exit(0)

# Parse the form data
form = cgi.FieldStorage()

# Validate presence of 'filename' and 'content' fields
if "filename" not in form or "content" not in form:
    print("Status: 400 Bad Request")
    print("Content-Type: text/plain\r\n")
    print("Missing 'filename' or 'content' field.")
    sys.exit(1)

filename = form.getvalue("filename")
file_item = form["content"]

# Check for valid filename and uploaded file object
if not filename or not file_item.file:
    print("Status: 400 Bad Request")
    print("Content-Type: text/plain\r\n")
    print("Invalid filename or file content.")
    sys.exit(1)

# Sanitize filename to prevent directory traversal
safe_filename = os.path.basename(filename)
filepath = os.path.join(UPLOAD_DIR, safe_filename)

# Ensure the upload directory exists
try:
    os.makedirs(UPLOAD_DIR, exist_ok=True)
except OSError:
    print("Status: 500 Internal Server Error")
    print("Content-Type: text/plain\r\n")
    print("Could not create upload directory.")
    sys.exit(1)

# Save the file
try:
    with open(filepath, "wb") as f:
        f.write(file_item.file.read())
except Exception as e:
    print("Status: 500 Internal Server Error")
    print("Content-Type: text/plain\r\n")
    print(f"Failed to save file: {str(e)}")
    sys.exit(1)

# Respond with success
print("Status: 201 Created")
print("Content-Type: text/plain\r\n")
print(f"Saved as: {safe_filename}")
