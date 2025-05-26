# HTTP Requests
## Body
The request body in an HTTP request contains the data being sent to the server.
Its structure and content depend on the Content-Type header, which specifies the
format of the body.

### 1. JSON (JavaScript Object Notation)
**Content-Type**: application/json\
Commonly used in APIs to send structured data.
```
POST /api/v1/resource HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: application/json
Content-Length: 34
Connection: keep-alive

{
    "name": "John Doe",
    "age": 30
}
```

### 2. Form Data (URL-Encoded)
**Content-Type**: application/x-www-form-urlencoded\
Used for submitting form data in a simple key-value format.
```
POST /api/v1/resource HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: application/x-www-form-urlencoded
Content-Length: 42
Connection: keep-alive

name=John+Doe&age=30&email=john.doe%40example.com
```

### 3. Form Data (Multipart)
**Content-Type**: multipart/form-data\
Used for file uploads or complex form submissions.
```
POST /api/v1/resource HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: multipart/form-data; boundary=boundary123
Connection: keep-alive

--boundary123
Content-Disposition: form-data; name="name"

John Doe
--boundary123
Content-Disposition: form-data; name="file"; filename="example.txt"
Content-Type: text/plain

This is the content of the file.
--boundary123--
```

### 4. Plain Text
**Content-Type**: text/plain\
Used for sending raw text data.
```
POST /api/v1/resource HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: text/plain
Content-Length: 24
Connection: keep-alive

This is a plain text body.
```

### 5. XML (Extensible Markup Language)
**Content-Type**: application/xml or text/xml\
Used in older APIs or systems that require XML
```
POST /api/v1/resource HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: application/xml
Content-Length: 91
Connection: keep-alive

<user>
    <name>John Doe</name>
    <age>30</age>
    <email>john.doe@example.com</email>
</user>
```

### 6. Binary Data
**Content-Type**: application/octet-stream\
Used for sending raw binary data, such as files.
```
POST /api/v1/resource HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: application/octet-stream
Content-Length: 1024
Connection: keep-alive

<binary data here>
```

### 7. GraphQL Query
**Content-Type**: application/json\
Used for GraphQL APIs to send queries and mutations.
```
POST /graphql HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Content-Type: application/json
Content-Length: 64
Connection: keep-alive

{
    "query": "query { user(id: 1) { name, email } }"
}
```

### 8. Custom Formats
**Content-Type**: Custom MIME types (e.g., application/vnd.api+json).\
Used for APIs that define their own data formats.
```
{
    "data": {
        "type": "user",
        "attributes": {
            "name": "John Doe",
            "age": 30
        }
    }
}
```

# HTTP Response
## Body
### 1. JSON (JavaScript Object Notation)
Usage: Commonly used in APIs to send structured data back to the client.\
**Content-Type**: application/json
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 45
Connection: keep-alive

{
    "status": "success",
    "data": {
        "name": "John Doe",
        "age": 30
    }
}
```

### 2. Form Data (URL-Encoded)
Usage: Rarely used in responses, but it can be used to send simple key-value pairs.\
**Content-Type**: application/x-www-form-urlencoded
```
HTTP/1.1 200 OK
Content-Type: application/x-www-form-urlencoded
Content-Length: 42
Connection: keep-alive

name=John+Doe&age=30&email=john.doe%40example.com
```

### 3. Form Data (Multipart)
Usage: Used in responses for sending multiple parts, such as files or mixed content.\
**Content-Type**: multipart/form-data
```
HTTP/1.1 200 OK
Content-Type: multipart/form-data; boundary=boundary123
Connection: keep-alive

--boundary123
Content-Disposition: form-data; name="name"

John Doe
--boundary123
Content-Disposition: form-data; name="file"; filename="example.txt"
Content-Type: text/plain

This is the content of the file.
--boundary123--
```

### 4. Plain Text
Usage: Used for sending raw text data, such as logs or simple messages.\
**Content-Type**: text/plain
```
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 24
Connection: keep-alive

This is a plain text body.
```

### 5. XML (Extensible Markup Language)
Usage: Used in older APIs or systems that require XML.\
**Content-Type**: application/xml or text/xml
```
HTTP/1.1 200 OK
Content-Type: application/xml
Content-Length: 91
Connection: keep-alive

<user>
    <name>John Doe</name>
    <age>30</age>
    <email>john.doe@example.com</email>
</user>
```

### 6. Binary Data
Usage: Used for sending raw binary data, such as files, images, or videos.\
**Content-Type**: application/octet-stream (or a specific type like image/png).
```
HTTP/1.1 200 OK
Content-Type: application/octet-stream
Content-Length: 1024
Connection: keep-alive

<binary data here>
```

### 7. HTML
Usage: Used for sending HTML content, such as web pages.\
**Content-Type**: text/html
```HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 123
Connection: keep-alive

<html>
    <body>
        <h1>Hello, World!</h1>
    </body>
</html>
```
