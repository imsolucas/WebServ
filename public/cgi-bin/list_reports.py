#!/usr/bin/env python3

import os
import json

UPLOAD_DIR = os.path.abspath("../uploads")

print("Content-Type: application/json\r\n")

try:
	files = [f for f in os.listdir(UPLOAD_DIR) if os.path.isfile(os.path.join(UPLOAD_DIR, f))]
	print(json.dumps(files))
except Exception as e:
	print(json.dumps([]))
