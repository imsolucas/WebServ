# 📁 CGI File Upload Script: `submit_report.py`

## 🎯 Purpose
The script processes HTTP POST requests with `multipart/form-data`, extracts uploaded file(s), and saves the **first** uploaded file to the server's `../uploads` directory. It then returns a simple HTML confirmation page indicating success or failure.

> **Note:** Only the first file in the multipart data is handled currently.

---

## ⚙️ How It Works (Step-by-step)

### Environment Variables
- `REQUEST_METHOD`: Verifies that the HTTP method is `POST`.
- `CONTENT_TYPE`: Checks that it contains `multipart/form-data` and extracts the boundary string.
- `CONTENT_LENGTH`: Determines the exact byte size of the POST body to read.

### Reading POST Body
- Reads exactly `CONTENT_LENGTH` bytes from `stdin.buffer`.

### Multipart Parsing
- Splits the POST body by boundary markers to isolate parts.
- Each part contains headers and content.
- Searches for a part with `Content-Disposition` header that includes a `filename` attribute (indicating a file).
- Sanitizes the filename with `os.path.basename` to avoid directory traversal attacks.
- Saves the file content to `../uploads/<filename>`.
- Stops processing after saving the first file.

### Response
- Outputs HTTP header: `Content-Type: text/html`
- Returns a simple HTML page showing upload success or error message.

---

## 🧩 Example Multipart/form-data POST Body

```
--AaB03x
Content-Disposition: form-data; name="file"; filename="cat.png"
Content-Type: image/png

(binary content of cat.png)
--AaB03x--
```

*Boundary* is defined in the `Content-Type` header:

```
Content-Type: multipart/form-data; boundary=AaB03x
```

---

## 🖥️ Example CGI Environment Variables for Upload

```bash
REQUEST_METHOD=POST
CONTENT_TYPE='multipart/form-data; boundary=AaB03x'
CONTENT_LENGTH=123456  # length of full multipart body in bytes
```

---

## 🚚 Data Flow into Script

- The entire multipart body (of length `CONTENT_LENGTH`) is piped to the script's `stdin.buffer`.
- The script reads all bytes, parses the multipart data, and extracts the first file for saving.

---

## 🖼️ Example HTML Responses

### Success

```html
Content-Type: text/html

<html><body>
  <h2>✅ Upload successful: cat.png</h2>
  <p>Saved to /uploads/cat.png</p>
</body></html>
```

### Failure (No file or wrong method)

```html
Content-Type: text/html

<html><body>
  <h2>❌ No file received.</h2>
</body></html>
```

or

```html
Content-Type: text/html

<html><body>
  <h2>❌ Error: Only POST supported</h2>
</body></html>
```

---

## ⚠️ Notes

- Currently only **the first file** in the multipart data is saved.
- To support **multiple file uploads**, the parsing logic must be extended.
- Uploaded files are saved relative to the script location, inside `../uploads`.
- Filenames are sanitized with `os.path.basename` to prevent directory traversal vulnerabilities.

---
