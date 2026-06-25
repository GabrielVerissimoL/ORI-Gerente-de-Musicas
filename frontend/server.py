#!/usr/bin/env python3
"""Servidor estático com proxy para a API C++ na porta 5000.

Não altera o backend. O navegador acessa /api no mesmo domínio do frontend,
e este servidor encaminha a requisição para o Drogon.
"""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

FRONTEND_HOST = os.getenv("FRONTEND_HOST", "0.0.0.0")
FRONTEND_PORT = int(os.getenv("FRONTEND_PORT", "3000"))
BACKEND_URL = os.getenv("BACKEND_URL", "http://127.0.0.1:5000").rstrip("/")
ROOT = Path(__file__).resolve().parent


class FrontendHandler(SimpleHTTPRequestHandler):
    server_version = "RubraFrontend/1.0"

    def end_headers(self) -> None:
        if not self.path.startswith("/api/"):
            self.send_header("Cache-Control", "no-cache")
        super().end_headers()

    def do_GET(self) -> None:  # noqa: N802 - nome exigido por BaseHTTPRequestHandler
        if self.path.startswith("/api/"):
            self._proxy_request("GET")
            return
        super().do_GET()

    def do_POST(self) -> None:  # noqa: N802
        if self.path.startswith("/api/"):
            self._proxy_request("POST")
            return
        self.send_response(404)
        self.end_headers()

    def _proxy_request(self, method: str) -> None:
        backend_path = self.path[len("/api") :]
        target = f"{BACKEND_URL}{backend_path}"

        # Lê o corpo da requisição para POST
        data = None
        headers = {"Accept": "application/json", "User-Agent": "RubraFrontend/1.0"}
        if method == "POST":
            content_length = int(self.headers.get("Content-Length", 0))
            data = self.rfile.read(content_length) if content_length > 0 else None
            content_type = self.headers.get("Content-Type", "application/json")
            headers["Content-Type"] = content_type

        request = urllib.request.Request(
            target,
            data=data,
            headers=headers,
            method=method,
        )

        try:
            with urllib.request.urlopen(request, timeout=25) as response:
                body = response.read()
                self.send_response(response.status)
                self.send_header("Content-Type", response.headers.get("Content-Type", "application/json"))
                self.send_header("Content-Length", str(len(body)))
                self.send_header("Cache-Control", "no-store")
                self.end_headers()
                self.wfile.write(body)
        except urllib.error.HTTPError as error:
            body = error.read()
            self.send_response(error.code)
            self.send_header("Content-Type", error.headers.get("Content-Type", "application/json"))
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
        except (urllib.error.URLError, TimeoutError, ConnectionError) as error:
            body = json.dumps(
                {
                    "Erro": "Backend indisponível",
                    "Detalhe": f"Não foi possível conectar em {BACKEND_URL}: {error}",
                },
                ensure_ascii=False,
            ).encode("utf-8")
            self.send_response(502)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)

    def log_message(self, fmt: str, *args: object) -> None:
        sys.stdout.write(f"[FRONTEND] {self.address_string()} - {fmt % args}\n")


def main() -> None:
    os.chdir(ROOT)
    server = ThreadingHTTPServer((FRONTEND_HOST, FRONTEND_PORT), FrontendHandler)
    print("=" * 64)
    print("RUBRA MUSIC - FRONTEND")
    print(f"Site:    http://localhost:{FRONTEND_PORT}")
    print(f"Backend: {BACKEND_URL}")
    print("Para encerrar, pressione Ctrl+C.")
    print("=" * 64)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n[FRONTEND] Servidor encerrado.")
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
