#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/plain\n")

method = os.environ.get("REQUEST_METHOD", "")
if method == "GET":
    print("Received GET")
    print("Query:", os.environ.get("QUERY_STRING", ""))
elif method == "POST":
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    post_data = sys.stdin.read(length)
    print("Received POST")
    print("Data:", post_data)
sys.stdout.flush()
