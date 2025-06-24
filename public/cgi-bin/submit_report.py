#!/usr/bin/env python3

import os
import sys
import html

UPLOAD_DIR = os.path.abspath("../public/uploads")

def parse_multipart(stdin, boundary, content_length):
    body = stdin.read(content_length)
    boundary_bytes = f"--{boundary}".encode()
    parts = body.split(boundary_bytes)

    for part in parts:
        if b'Content-Disposition' not in part:
            continue

        header_end = part.find(b"\r\n\r\n")
        if header_end == -1:
            continue

        headers_block = part[:header_end].decode()
        content_start = header_end + 4
        content_end = part.rfind(b"\r\n")
        content = part[content_start:content_end]

        for line in headers_block.split("\r\n"):
            if "filename=" in line:
                filename = line.split("filename=")[1].strip("\"")
                filename = os.path.basename(filename)
                filepath = os.path.join(UPLOAD_DIR, filename)
                with open(filepath, "wb") as f:
                    f.write(content)
                return filename
    return None

def render_gallery(files):
    gallery_items = ""
    for file in files:
        safe_file = html.escape(file)
        gallery_items += f"""
        <li style="background: rgba(20,20,20,0.85); border: 1px solid #222; border-radius: 12px;
                   box-shadow: 0 0 16px #111; padding: 1.2em 1em 1em 1em; width: 220px;
                   display: flex; flex-direction: column; align-items: center; position: relative;">
            <div style="width:180px;height:180px;display:flex;align-items:center;justify-content:center;
                        background:#111;border-radius:8px;overflow:hidden;margin-bottom:1em;">
                <img src="/uploads/{safe_file}" alt="{safe_file}"
                     style="max-width:100%;max-height:100%;object-fit:contain;filter:drop-shadow(0 0 8px #eeff00);"
                     onerror="this.src='https://img.icons8.com/ios-filled/100/ff0000/image.png';" />
            </div>
            <a href="/uploads/{safe_file}" target="_blank"
               style="color:#00ffe7;font-size:1.1em;word-break:break-all;margin-bottom:0.7em;">{safe_file}</a>
            <button onclick="deleteFile('{safe_file}')" style="background:#eeff00;color:#111;
                    border:none;border-radius:6px;padding:0.5em 1.2em;cursor:pointer;
                    font-family:inherit;font-size:1em;box-shadow:0 0 8px #eeff00;">Delete</button>
        </li>
        """

    return gallery_items

def main():
    try:
        method = os.environ.get("REQUEST_METHOD", "")
        if method != "POST":
            raise Exception("Only POST supported")

        content_type = os.environ.get("CONTENT_TYPE", "")
        if "multipart/form-data" not in content_type:
            raise Exception("Invalid content type")

        boundary = content_type.split("boundary=")[-1]
        content_length = int(os.environ.get("CONTENT_LENGTH", "0"))

        if not os.path.isdir(UPLOAD_DIR):
            os.makedirs(UPLOAD_DIR, exist_ok=True)

        filename = parse_multipart(sys.stdin.buffer, boundary, content_length)

        files = [f for f in os.listdir(UPLOAD_DIR) if os.path.isfile(os.path.join(UPLOAD_DIR, f))]

        # Print the full HTML page
        print("Content-Type: text/html\r\n")
        print()
        print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Black Market Gallery</title>
    <link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap" rel="stylesheet">
    <style>
        body {{
            font-family: 'Share Tech Mono', monospace;
            padding: 2rem;
            text-align: center;
            background: linear-gradient(135deg, #0a0a0a 80%, #1a1a1a 100%);
            color: #eeff00;
            min-height: 100vh;
        }}
        h1 {{
            font-size: 3rem;
            margin-bottom: 0.5em;
            text-shadow: 0 0 10px #eeff00;
        }}
        ul {{
            display: flex;
            flex-wrap: wrap;
            gap: 2em;
            justify-content: center;
            list-style: none;
            padding: 0;
            margin: 2em 0;
        }}
        footer {{
            margin-top: 3em;
            font-size: 0.9em;
            color: #444;
            letter-spacing: 0.1em;
        }}
    </style>
</head>
<body>
    <h1>🖼️ NFT Gallery</h1>
    <p>Upload successful: <strong>{html.escape(filename) if filename else "unknown"}</strong></p>
    <ul id="file-list">
        {render_gallery(files)}
    </ul>
    <p><a href="/upload.html">← Upload More</a></p>
    <footer>
        &copy; 2025 Black Market &mdash; All rights reserved.
    </footer>
    <script>
        function deleteFile(filename) {{
            fetch('/cgi-bin/delete_report.py', {{
                method: 'POST',
                headers: {{ 'Content-Type': 'application/x-www-form-urlencoded' }},
                body: 'file=' + encodeURIComponent(filename)
            }}).then(res => res.text()).then(msg => {{
                alert(msg);
                location.reload();
            }});
        }}
    </script>
</body>
</html>""")

    except Exception as e:
        print("Content-Type: text/html\r\n")
        print()
        print(f"<html><body><h2>❌ Error: {html.escape(str(e))}</h2></body></html>")

if __name__ == "__main__":
    main()
