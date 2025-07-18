# 🌐 WebServ

> 🚀 **Fast, flexible, and fun to learn!**  
> A custom HTTP web server written in C++ for educational purposes.

- 📃 Error handling and logging system

## 📁 Directory Structure
```text
WebServ/
├── src/ # C++ source code
├── include/ # Header files
├── config/ # Configuration files (*.conf)
├── public/ # Static files, uploads, and CGI scripts
├── test/ # Automated/manual test scripts
├── doc/ # Documentation and diagrams
```

---

## 📝 Prerequisites

Make sure the following are installed:

- `g++` / `clang++` – C++ compiler
- `make`
- [`siege`](https://www.joedog.org/siege-home/) – for load testing

To install `siege` on Ubuntu:
```bash
sudo apt-get update
sudo apt-get install siege
```

---

## 🏗️ Build Instructions

1. Clone the repository:
   ```bash
   git clone <repo-url>
   cd WebServ
   ```
2. Build the server:
   ```bash
   make
   ```

---

## ▶️ Running the Server

1. Choose a config file (e.g., `default.conf`):
   ```bash
   ./webserv config/default.conf
   ```
2. The server will run on the port specified in your config.

---

## 🧪 Testing

### 🔁 Automated tests
Run:
```bash
python3 test/automated_tests.py
```
👉 Run the server with `config/automated_tests.conf` before testing!
```bash
./webserv config/automated_tests.conf
```

### 👨‍🔬 Manual tests
See `test/manual_tests.txt` for checklist-based testing.

### 💥 Load Testing with siege
```bash
siege http://localhost:<port>
```
Replace `<port>` with your server's port.

---

## 📚 Documentation

Explore the `doc/` folder for:
- Architecture diagrams
- HTTP request/response flow
- CGI handling explanation
- Error response breakdowns

---

## 📝 Notes

✅ Ensure your CGI interpreters (e.g., python3) are installed.

🔒 Make sure CGI scripts have executable permissions:
```bash
chmod +x public/cgi-bin/script.py
```

🔄 Uploads: `public/uploads/`
❌ Error pages: `public/error/`

---

## 📄 License

This project is educational only and not intended for production use.