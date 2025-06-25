# import os
# import http.server
# import socketserver

# class DeleteEnabledCGIHandler(http.server.CGIHTTPRequestHandler):
#     def do_DELETE(self):
#         self.command = "DELETE"
#         self.run_cgi()

# # Optional: serve files from ./public
# os.chdir('/')

# PORT = 8000

# # ✅ Use ThreadingHTTPServer to allow concurrent requests AND provide needed attributes
# Handler = DeleteEnabledCGIHandler
# Handler.cgi_directories = ['/cgi-bin']

# with socketserver.ThreadingTCPServer(("", PORT), Handler) as httpd:
#     httpd.server_name = "localhost"
#     httpd.server_port = PORT
#     print(f"🚀 Serving at http://localhost:{PORT} with DELETE support")
#     httpd.serve_forever()
