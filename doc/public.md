# 🗂️ `public/` Directory – Bug Reporting Portal Overview

This document gives you a quick tour of the `public/` folder used by our web server. It contains all the static HTML pages, upload targets, and CGI scripts that power a simple **Bug Reporting Portal**.

If you're running the server for the first time, this is where all the web-facing content lives. Each file or folder plays a role in letting users report bugs and view related content.

---

## 🔑 What You’ll Find in `public/`

```plaintext
public/
├── index.html              # Home page
├── upload.html             # File upload page
├── report/                 # Bug report index page
│   └── index.html
├── uploads/                # Where uploaded files are saved
├── cgi-bin/                # CGI scripts for dynamic behavior
│   ├── submit_report.py
│   └── echo_env.py
└── error/                  # Default error pages (404.html, etc.)
```

---

## 🏠 `index.html`

- **Path:** `/`
- **Purpose:**
  This is the **main home page** of the bug reporting portal. It offers two primary options:
  1. Submit a bug report
  2. View submitted reports

---

## 📤 `upload.html`

- **Path:** `/upload.html`
- **Purpose:**
  This page contains a simple form that lets users **upload files related to a bug**.
  Examples include: screenshots, log files, notes.

- The form submits a `POST` request to `/uploads/`.
  The **server** is responsible for handling and storing those files correctly.

---

## 📁 `report/`

- **Path:** `/report/`
- **Contents:** `index.html`

- **Purpose:**
  A **placeholder page** that mimics the look of a bug report index or dashboard.

- This demonstrates:
  - Static page nesting (e.g. accessing a file like `/report/index.html`)
  - Folder-based routing handled by the web server

> You can later replace this with a **CGI-generated list of uploaded reports**.

---

## 🗂️ `uploads/`

- **Path:** `/uploads/`
- **Purpose:**
  This is the **target directory** for file uploads.
  When someone submits a file via `upload.html`, it is saved here.

> 🔒 **Important:**
> Make sure your server has **write permissions** for this folder!

---

## ⚙️ `cgi-bin/`

- **Path:** `/cgi-bin/`
- **Purpose:**
  Holds **scripts executed dynamically** by the server.
  (CGI = Common Gateway Interface)

### Scripts:

#### 1. `submit_report.py` 
- (see `submit_report.md`)
- Used to **confirm a bug report was received** by processing the uploaded file sent via a POST request.
- Extracts and saves the **first uploaded file** (e.g., a screenshot or log) to the server’s ../uploads directory.
- Returns a simple **thank-you HTML message** confirming the successful upload or an error message if the upload failed.

#### 2. `echo_env.py`
- A test script that prints out all **environment variables**
- Useful for **debugging your CGI setup**

> 🛠️ **Make sure these scripts are executable**:
> Run `chmod +x submit_report.py echo_env.py`

---

## 🚨 `error/`

- **Path:** `/error/`
- **Purpose:**
  Contains default **error pages** like:

  - `404.html` → Not Found
  - `500.html` → Internal Server Error

These are served when something goes wrong or if a route does not exist.

---

## 🌐 Quick Navigation Summary

| URL Path                     | What It Does                          |
|-----------------------------|----------------------------------------|
| `/`                         | Home page                              |
| `/upload.html`             | Upload a file                          |
| `/uploads/`                | Upload destination (invisible to users)|
| `/report/`                 | Static report list                     |
| `/cgi-bin/submit_report.py`| CGI script to confirm bug submission   |
| `/cgi-bin/echo_env.py`     | CGI environment debug (for devs)       |

---

