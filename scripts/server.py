#!/usr/bin/env python3
import http.server
import socketserver

PORT = 8080

class HttpRequestHandler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {
        '': 'application/octet-stream',
        '.manifest': 'text/cache-manifest',
        '.html': 'text/html',
        '.png': 'image/png',
        '.jpg': 'image/jpg',
        '.svg':	'image/svg+xml',
        '.css':	'text/css',
        '.js':'application/x-javascript',
        '.wasm': 'application/wasm',
        '.json': 'application/json',
        '.xml': 'application/xml',
    }

httpd = socketserver.TCPServer(("", PORT), HttpRequestHandler)

try:
    print(f"serving on port {PORT}")
    httpd.serve_forever()
except KeyboardInterrupt:
    pass
