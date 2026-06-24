# Rubra Music — Frontend

Frontend em HTML, CSS e JavaScript integrado ao backend C++/Drogon na porta 5000.

## Funcionalidades

- Pesquisa por música, artista e álbum.
- Resultados separados em fileiras.
- Ao clicar em um artista, abre uma página interna com todas as músicas retornadas para ele.
- Botão **Voltar aos resultados** e suporte à tecla `Esc`.
- Proxy local para evitar bloqueio de CORS.

## Executar

```bash
chmod +x start-frontend.sh
./start-frontend.sh
```

Acesse `http://localhost:3000`.

O backend C++ precisa estar executando em `http://localhost:5000`.
