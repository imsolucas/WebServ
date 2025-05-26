#!/usr/bin/env python3

import os
import sys

# Absolute path to the upload destination directory
UPLOAD_DIR = os.path.abspath("../uploads")

# Parses the multipart form data and extracts the uploaded file
def parse_multipart(stdin, boundary, content_length):
    body = stdin.read(content_length)  # Read the entire POST body
    boundary_bytes = f"--{boundary}".encode()
    parts = body.split(boundary_bytes)  # Split the body into parts

    for part in parts:
        if b'Content-Disposition' not in part:
            continue  # Skip parts that don't contain file data

        header_end = part.find(b"\r\n\r\n")  # End of headers
        if header_end == -1:
            continue  # Malformed part

        headers_block = part[:header_end].decode()
        content_start = header_end + 4
        content_end = part.rfind(b"\r\n")
        content = part[content_start:content_end]  # Extract file content

        # Extract filename from Content-Disposition header
        for line in headers_block.split("\r\n"):
            if "filename=" in line:
                filename = line.split("filename=")[1].strip("\"")
                filename = os.path.basename(filename)  # Sanitize path
                filepath = os.path.join(UPLOAD_DIR, filename)
                with open(filepath, "wb") as f:
                    f.write(content)  # Save the uploaded file
                return filename  # Return the saved filename

    return None  # No valid file part found

# Entry point for the CGI script
def main():
    try:
        # Ensure the method is POST
        method = os.environ.get("REQUEST_METHOD", "")
        if method != "POST":
            raise Exception("Only POST supported")

        # Check for multipart form data
        content_type = os.environ.get("CONTENT_TYPE", "")
        if "multipart/form-data" not in content_type:
            raise Exception("Invalid content type")

        # Extract boundary and content length
        boundary = content_type.split("boundary=")[-1]
        content_length = int(os.environ.get("CONTENT_LENGTH", "0"))

        # Create upload directory if it doesn't exist
        if not os.path.isdir(UPLOAD_DIR):
            os.makedirs(UPLOAD_DIR, exist_ok=True)

        # Process the incoming file
        filename = parse_multipart(sys.stdin.buffer, boundary, content_length)

        # Send HTTP response headers
        print("Content-Type: text/html\r\n")
        print("<html><body>")
        if filename:
            # Success message if file was saved
            print(f"<h2>✅ Upload successful: {filename}</h2>")
            print(f"<p>Saved to /uploads/{filename}</p>")
        else:
            # No file found in the upload
            print("<h2>❌ No file received.</h2>")
        print("</body></html>")

    except Exception as e:
        # In case of error, show it in the response
        print("Content-Type: text/html\r\n")
        print(f"<html><body><h2>❌ Error: {str(e)}</h2></body></html>")

# Run the script
if __name__ == "__main__":
    main()
