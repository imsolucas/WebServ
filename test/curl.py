# Color macros
CYAN = "\033[96m"
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

# subprocess.run([...]) executes curl just as if you typed it in the terminal.
# -s -o /dev/null -w "%{http_code}" tells curl to:
# -s: be silent
# -o /dev/null: discard body
# -w "%{http_code}": print only the HTTP status code
# The output is compared to your expected status.

import subprocess

BASE_URL = "http://localhost:8080"

def run_curl_test(method, path, expected_status, data=None, is_file=False):
    url = BASE_URL + path
    curl_cmd = ["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", "-X", method, url]

    if data:
        if is_file:
            curl_cmd += ["--data-binary", f"@{data}"]
        else:
            curl_cmd += ["--data", data]

    try:
        result = subprocess.run(curl_cmd, capture_output=True, text=True)
        status = result.stdout.strip()
        if status == str(expected_status):
            print(f"{GREEN}✅ {method} {url} → {status}{RESET}")
        else:
            print(f"{RED}❌ {method} {url} → expected {expected_status}, got {YELLOW}{status}{RESET}")
    except Exception as e:
        print(f"{RED}❌ {method} {url} → Exception: {e}{RESET}")

# Helper for numbered test headers
test_counter = {"count": 1}

def test_info(message):
    print(f"{CYAN}\n{test_counter['count']}. {message}{RESET}")
    test_counter["count"] += 1

# HTTP Status Codes Tested:
# 200 OK
# 400 BAD_REQUEST
# 401 UNAUTHORIZED (not used in this test)
# 403 FORBIDDEN
# 404 NOT_FOUND
# 405 METHOD_NOT_ALLOWED
# 413 PAYLOAD_TOO_LARGE
# 500 INTERNAL_SERVER_ERROR
# 502 BAD_GATEWAY
# 504 GATEWAY_TIMEOUT

test_info("Testing GET request on root directory")
run_curl_test("GET", "/", 200)

test_info("Testing invalid HTTP method")
run_curl_test("FAKE_METHOD", "/", 400)

# ⚠️ Make sure to chmod 000 forbidden/ to test this
test_info("Testing access to forbidden path")
run_curl_test("GET", "/forbidden", 403)

test_info("Testing non-existent path")
run_curl_test("GET", "/doesnotexist", 404)

test_info("Testing disallowed method - POST on root")
run_curl_test("POST", "/", 405)

test_info("Testing disallowed method - DELETE on root")
run_curl_test("DELETE", "/", 405)

test_info("Testing file upload with small body")
run_curl_test("POST", "/uploads/short.txt", 201, data="short body")

test_info("Testing file upload exceeding max body size")
run_curl_test("POST", "/uploads/long.txt", 413, data="@" * 100000)

test_info("Testing file upload with actual file contents")
run_curl_test("POST", "/uploads/test.txt", 201, data="test.txt", is_file=True)

test_info("Testing file deletion with uploaded file")
run_curl_test("DELETE", "/uploads/test.txt", 200)

test_info("Testing file deletion with non-existent file")
run_curl_test("DELETE", "/uploads/doesnotexist.txt", 404)

test_info("Testing CGI script execution - GET")
run_curl_test("GET", "/cgi-bin/test_cgi.php", 200)

test_info("Testing CGI script execution - POST")
run_curl_test("POST", "/cgi-bin/test_cgi.php", 200)

test_info("Testing CGI with invalid binary")
run_curl_test("GET", "/cgi-bin/invalid_bin.php", 500)

test_info("Testing CGI script that crashes")
run_curl_test("GET", "/cgi-bin/crash.php", 502)

test_info("Testing CGI script with infinite loop timeout")
run_curl_test("GET", "/cgi-bin/infinite_loop.php", 504)

