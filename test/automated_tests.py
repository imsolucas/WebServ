# To run the automated tests:
# 1) navigate to the project root directory
# 2) launch the web server on port 8080 with:
# 	"./webserv config/automated_tests.conf"
# 3) setup the directory permissions:
# 	"chmod 000 public/forbidden"
# 4) run the script:
# 	"python3 test/automated_tests.py"

BOLD = "\033[1m"
CYAN = "\033[96m"
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

WIDTH = 70

total_tests = 0
passed_tests = 0
failed_tests = 0

# subprocess.run([...]) executes curl just as if you typed it in the terminal.
# -s -o /dev/null -w "%{http_code}" tells curl to:
# -s: be silent
# -o /dev/null: discard body
# -w "%{http_code}": print only the HTTP status code
# The output is compared to your expected status.

import subprocess
import json

BASE_URL = "http://localhost:8080"

def run_curl_test(method, path, expected_status, data=None, is_file=False):
    global total_tests, passed_tests, failed_tests
    total_tests += 1
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
            passed_tests += 1
            print(f"{GREEN}✅ {method} {url} → {status}{RESET}")
        else:
            failed_tests += 1
            print(f"{RED}❌ {method} {url} → expected {expected_status}, got {YELLOW}{status}{RESET}")
    except Exception as e:
        failed_tests += 1
        print(f"{RED}❌ {method} {url} → Exception: {e}{RESET}")

def run_siege_test(path, duration="10s", concurrency=10, min_availability=99.5):
    global total_tests, passed_tests, failed_tests
    total_tests += 1
    url = BASE_URL + path
    siege_cmd = [
        "siege", "--json", "-b", "-t", duration, "-c", str(concurrency), url
    ]

    print(f"{YELLOW}🛡️  Siege test → {url} for {duration} with {concurrency} users{RESET}")

    try:
        result = subprocess.run(siege_cmd, capture_output=True, text=True)

        output = result.stdout

        availability = 0.0
        siege_data = json.loads(output)
        if "availability" in siege_data:
            availability = siege_data["availability"]
        else:
            print("JSON in stdout found, but 'availability' key not present.")

        if availability >= min_availability:
            passed_tests += 1
            print(f"{GREEN}✅ Siege availability {availability:.2f}% ≥ {min_availability:.2f}%{RESET}")
        else:
            failed_tests += 1
            print(f"{RED}❌ Siege availability {availability:.2f}% < {min_availability:.2f}%{RESET}")
        print(output)
    except Exception as e:
        failed_tests += 1
        print(f"{RED}❌ Siege test failed → Exception: {e}{RESET}")

test_counter = {"count": 1}

def test_info(message):
    print(f"{CYAN}\n{test_counter['count']}. {message}{RESET}")
    test_counter["count"] += 1

def test_header(title, color=YELLOW):
	print(f"{color}\n{'-' * WIDTH}\n{title.center(WIDTH)}\n{'-' * WIDTH}{RESET}")

def test_summary():
    test_header("TEST SUMMARY", color=BOLD)

    passed_line = f"PASSED: {passed_tests}/{total_tests}"
    failed_line = f"FAILED: {failed_tests}/{total_tests}"
    total_line  = f"TOTAL:  {total_tests}"

    percentage = (passed_tests / total_tests * 100) if total_tests else 0
    score_line = f"SCORE:  {percentage:.1f}%"

    print(f"{GREEN}{passed_line.center(WIDTH)}{RESET}")
    print(f"{RED}{failed_line.center(WIDTH)}{RESET}")
    print(f"{CYAN}{total_line.center(WIDTH)}{RESET}")
    print(f"{YELLOW}{score_line.center(WIDTH)}{RESET}")
    print(f"{BOLD}{'-' * WIDTH}{RESET}")


# HTTP Status Codes Tested:
# 200 OK
# 201 CREATED
# 400 BAD_REQUEST (not used in this test)
# 401 UNAUTHORIZED (not used in this test)
# 403 FORBIDDEN
# 404 NOT_FOUND
# 405 METHOD_NOT_ALLOWED
# 413 CONTENT_TOO_LARGE
# 500 INTERNAL_SERVER_ERROR
# 501 NOT_IMPLEMENTED
# 502 BAD_GATEWAY
# 504 GATEWAY_TIMEOUT

test_header("CURL TESTS")

test_info("Testing GET request on root directory")
run_curl_test("GET", "/", 200)

# ⚠️ Make sure to chmod 000 forbidden/ to test this
test_info("Testing access to forbidden path")
run_curl_test("GET", "/forbidden/", 403)

test_info("Testing non-existent path")
run_curl_test("GET", "/doesnotexist/", 404)

test_info("Testing disallowed method - POST on root")
run_curl_test("POST", "/", 405)

test_info("Testing disallowed method - DELETE on root")
run_curl_test("DELETE", "/", 405)

test_info("Testing unimplemented HTTP method")
run_curl_test("PUT", "/", 501)

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

test_header("SIEGE TESTS")

test_info("Load testing root")
run_siege_test("/")

test_info("Load testing static file route")
run_siege_test("/index.html")

test_info("Load testing CGI route")
run_siege_test("/cgi-bin/test_cgi.php")

test_info("Stress testing static file route")
run_siege_test("/index.html", "30s", 50)

test_summary()
