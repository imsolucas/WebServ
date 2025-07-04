# Steps to Handle a GET Request
## Request:
**Request Line**: `GET /index.html HTTP/1.1`\
**Headers**: `Host`, `User-Agent`, `Accept`, etc.\
**Query Parameters** (if any): `/index.html?key=value`

## 1. **Match the URI to a Location Block**
- Nginx uses the URI (`/index.html`) to find the appropriate **location block** in its configuration file (`nginx.conf`).
- Example configuration:
```c++
server {
    listen 80;
    server_name example.com;

    location / {
        root /var/www/html;
        index index.html;
    }
}
```
- In this example:
The URI `/index.html` matches the `/` location block.
Nginx determines that the file is located at `/var/www/html/index.html`.

## 2. **Check for Static File**
If the requested resource is a static file (e.g., HTML, CSS, JS, images), Nginx:
- Checks if the file exists in the specified `root` directory.
- If the file exists, Nginx reads the file from disk.

## 3. **Generate the Response**
Nginx constructs an HTTP response:
**Status Line**: `HTTP/1.1 200 OK`\
**Headers**:\
`Content-Type`: Based on the file type (e.g., `text/html` for `.html` files).\
`Content-Length`: Size of the file.\
`Connection`: `keep-alive` (if persistent connections are enabled).\
**Body**: The contents of the requested file.

## 4. **Send the Response**
Nginx sends the response back to the client over the established connection.

## 5. **Close or Reuse the Connection**
If `Connection: keep-alive` is enabled, Nginx keeps the connection open for reuse.
Otherwise, Nginx closes the connection.

---

# Steps to Handle a POST Request
## Request
**Request Line**: `POST /api/v1/resource HTTP/1.1`\
**Headers**: `Host`, `Content-Type`, `Content-Length`, etc.\
**Body**: The data sent by the client (e.g., JSON, form data, or binary data).

## 1. **Match the URI to a Location Block**
- Nginx uses the URI (`/api/v1/resource`) to find the appropriate **location block** in its configuration file (`nginx.conf`).
- Example configuration:
```c++
server {
    listen 80;
    server_name example.com;

    location /api/ {
        proxy_pass http://127.0.0.1:5000;
    }
}
```
- In this example:
The URI `/api/v1/resource` matches the `/api/` location block.
Nginx proxies the request to an upstream server running on `127.0.0.1:5000`.

## 2. **Handle the Request Body**
- Nginx reads the body of the `POST` request based on the `Content-Length` header.
- The body can contain:
**JSON**: `{"name": "John Doe", "age": 30}`
**Form Data**: `name=John+Doe&age=30`
**Binary Data**: Files or other non-text data.

## 3. **Process the Request**
- Nginx can process the `POST` request in one of two ways:
1. **Serve Static Content:**
If the `POST` request is meant to upload a file, Nginx can save the file to a specified directory.
Example configuration:
```c++
location /upload/ {
    root /var/www/uploads;
    client_body_temp_path /var/www/uploads/temp;
    client_max_body_size 10M;
}
```
4. **Proxy the Request**:
- Nginx forwards the `POST` request (including the body) to an upstream server for processing.
Example configuration:
```c++
location /api/ {
    proxy_pass http://127.0.0.1:5000;
    proxy_set_header Content-Type $content_type;
    proxy_set_header Content-Length $content_length;
}
```

## 5. **Generate the Response**
If Nginx processes the POST request directly (e.g., file upload), it generates an HTTP response:
**Status Line**: `HTTP/1.1 200 OK`
**Headers**:
`Content-Type`: Based on the response type (e.g., `application/json`).
`Content-Length`: Size of the response body.
**Body**: Confirmation or result of the `POST` request.

## 6. **Send the Response**
Nginx sends the response back to the client over the established connection.

## 7. **Close or Reuse the Connection**
If `Connection: keep-alive` is enabled, Nginx keeps the connection open for reuse.
Otherwise, Nginx closes the connection.

---
# Steps to Handle a DELETE Request
## Request
**Request Line**: `DELETE /api/v1/resource/123 HTTP/1.1`\
**Headers**: `Host`, `Authorization`, etc.\
**Body** (optional): Some `DELETE` requests may include a body with additional data (e.g., authentication tokens or metadata).

## 3. **Match the URI to a Location Block**
Nginx uses the URI (`/api/v1/resource/123`) to find the appropriate **location block** in its configuration file (`nginx.conf`).
Example configuration:
```c++
server {
    listen 80;
    server_name example.com;

    location /api/ {
        proxy_pass http://127.0.0.1:5000;
    }
}
```
In this example:
The URI `/api/v1/resource/123` matches the `/api/` location block.
Nginx proxies the request to an upstream server running on `127.0.0.1:5000`.

## 4. **Proxy the Request**
Nginx forwards the `DELETE` request (including headers and body) to the upstream server for processing.
Example configuration:
```c++
location /api/ {
    proxy_pass http://127.0.0.1:5000;
    proxy_set_header Content-Type $content_type;
    proxy_set_header Content-Length $content_length;
    proxy_set_header Authorization $http_authorization;
}
```

## 5. **Process the Request**
- The upstream server processes the `DELETE` request to remove the specified resource (e.g., a database record or file).
- If the resource is successfully deleted, the upstream server responds with a success message.
- If the resource cannot be deleted (e.g., it does not exist or the client lacks permissions), the upstream server responds with an appropriate error code.

## 6. **Generate the Response**
Nginx forwards the response from the upstream server back to the client.
Example response:
**Status Line**: `HTTP/1.1 200 OK` (if successful) or `HTTP/1.1 404 Not Found` (if the resource does not exist).
**Headers**:
`Content-Type`: Based on the response type (e.g., `application/json`).
`Content-Length`: Size of the response body.
**Body**: Confirmation or error message.

## 7. **Send the Response**
Nginx sends the response back to the client over the established connection.

## 8. **Close or Reuse the Connection**
If `Connection: keep-alive` is enabled, Nginx keeps the connection open for reuse.
Otherwise, Nginx closes the connection.
