# Rubra Music — Frontend

Frontend em HTML, CSS e JavaScript integrado ao backend C++/Drogon do projeto ORI-Gerente-de-Músicas.

## Integração utilizada

O site consulta, sem alterar o backend:

- `GET /music/name/{name}`
- `GET /music/singer/{singer}`
- `GET /music/album/{album}`

O `server.py` serve o site na porta `3000` e encaminha `/api/*` para `http://127.0.0.1:5000`. Isso evita CORS sem modificar a API.

## Executar

Com o backend já ligado na porta 5000:

```bash
cd ~/Downloads/ORI-Gerente-de-Musicas-main/frontend
./start-frontend.sh
```

Abra no navegador:

```text
http://localhost:3000
```

## Configuração opcional

Para usar outra porta do frontend:

```bash
FRONTEND_PORT=8080 ./start-frontend.sh
```

Para apontar a outro endereço do backend:

```bash
BACKEND_URL=http://192.168.0.10:5000 ./start-frontend.sh
```
