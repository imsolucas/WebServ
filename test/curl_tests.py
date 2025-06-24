# subprocess.run([...]) executes curl just as if you typed it in the terminal.
# -s -o /dev/null -w "%{http_code}" tells curl to:
# -s: be silent
# -o /dev/null: discard body
# -w "%{http_code}": print only the HTTP status code
# The output is compared to your expected status.

import subprocess

BASE_URL = "http://localhost:8080"

def run_curl_test(method, path, expected_status, data=None):
    url = BASE_URL + path
    curl_cmd = ["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", "-X", method, url]

    if data:
        curl_cmd += ["--data", data]

    try:
        result = subprocess.run(curl_cmd, capture_output=True, text=True)
        status = result.stdout.strip()
        if status == str(expected_status):
            print(f"✅ {method} {url} → {status}")
        else:
            print(f"❌ {method} {url} → expected {expected_status}, got {status}")
    except Exception as e:
        print(f"❌ {method} {url} → Exception: {e}")

# 200 OK
# 400 BAD_REQUEST
# 403 FORBIDDEN
# 404 NOT_FOUND
# 405 METHOD_NOT_ALLOWED
# 413 PAYLOAD_TOO_LARGE
# 500 INTERNAL_SERVER_ERROR
# 502 BAD_GATEWAY
# 504 GATEWAY_TIMEOUT

run_curl_test("GET", "/", 200)
run_curl_test("FAKE_METHOD", "/", 400)
run_curl_test("GET", "/forbidden", 403)
run_curl_test("GET", "/doesnotexist", 404)
run_curl_test("POST", "/", 405)
run_curl_test("DELETE", "/", 405)
run_curl_test("POST", "/uploads", 200, data="short body")
run_curl_test("POST", "/uploads", 413, data="@" * 10000)
run_curl_test("GET", "/cgi-bin/throw_exception.py", 500)
run_curl_test("GET", "/cgi-bin/throw_exception.py", 505)




run_curl_test("DELETE", "/to_be_deleted.txt", 200)
run_curl_test("DELETE", "/to_be_deleted.txt", 200)
