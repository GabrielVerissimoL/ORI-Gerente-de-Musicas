# 🎵 ORI — Gerente de Músicas

Sistema de gerenciamento de músicas desenvolvido para a disciplina de **Organização e Recuperação da Informação (ORI)**. Implementa um banco de dados próprio com **Árvore B** como índice primário, **índices secundários invertidos** e persistência em **arquivo binário de tamanho fixo**, exposto via **API REST** (Drogon/C++) e consumido por um **frontend web** moderno chamado **Rubra Music**.

---

## 🏗 Arquitetura

```
Navegador ◄──► Frontend Python (proxy :3000) ──► API REST Drogon (:5000)
                                                       │
                                        ┌──────────────┼──────────────┐
                                   Árvore B       Índices Secund.   Arquivo Binário
                                  (busca ID)     (nome, artista,   (MusicRecord
                                                  álbum, gênero)    tamanho fixo)
```

1. O **conversor** transforma o CSV em arquivo binário com registros fixos.
2. Ao iniciar, a API carrega tudo na RAM (Árvore B + índices secundários).
3. O **frontend** consome a API via proxy Python, evitando CORS.

---

## 🌳 Estruturas de Dados

### Árvore B (Índice Primário)
- Ordem 4, grau mínimo `t = 2`. Chave: objeto `music` ordenado por `id`.
- Operações: inserção com split, busca por ID, remoção com merge/empréstimo.

### Índices Secundários Invertidos
Quatro `std::map<string, vector<int>>` mapeando nome, artista, álbum e gênero (em minúsculas) para listas de IDs. Busca por prefixo via `lower_bound`.

### Registro em Disco (`MusicRecord`)
Struct de tamanho fixo com campos `char[]` e acesso direto via RRN: `fseek(file, rrn * sizeof(MusicRecord), SEEK_SET)`.

---

## 🛠 Tecnologias

| Tecnologia | Uso |
|------------|-----|
| **C++** | Backend: API, Árvore B, persistência binária |
| **Drogon** | Framework HTTP C++ assíncrono para a API REST |
| **JsonCpp** | Serialização JSON no backend |
| **Python 3** | Servidor frontend (proxy reverso + arquivos estáticos) |
| **JavaScript / HTML5 / CSS3** | Interface web (SPA vanilla, dark theme) |
| **Make** | Automação de build |
| **Docker** | Containerização do backend |

---

## 📁 Estrutura do Projeto

```
├── B-treeClass/btree.hpp          # Árvore B (header-only)
├── MusicClass/music.hpp|.cpp      # Classe music + struct MusicRecord
├── databaseClass/database.hpp|.cpp # Banco: índices, carga do disco, CRUD
├── B-treeAPI/
│   ├── api.cpp                    # API REST (main do servidor Drogon)
│   ├── conversor.cpp              # Conversor CSV → binário
│   ├── testador.cpp               # Testador interativo (CLI)
│   └── tcc_ceds_music.csv         # Dataset (~28 mil músicas)
├── frontend/
│   ├── index.html / styles.css / app.js  # Interface Rubra Music
│   └── server.py                  # Proxy Python (:3000 → :5000)
├── Docker/                        # Dockerfile + docker-compose
└── makefile                       # Build automatizado
```

---

## 🚀 Como Executar

### 1. Converter CSV → Binário *(apenas uma vez)*

```bash
g++ -Wall -Wextra -g3 B-treeAPI/conversor.cpp MusicClass/music.cpp databaseClass/database.cpp -o B-treeAPI/conversor
cd B-treeAPI && ./conversor && cd ..
```

### 2. Compilar e Rodar a API

```bash
make run      # Compila e executa (porta 5000)
# ou: make / make clean / make re
```

### 3. Iniciar o Frontend

```bash
cd frontend && python3 server.py
```

Acesse **`http://localhost:3000`** — o proxy encaminha `/api/*` para a porta 5000.

### 4. Testador via Terminal *(opcional)*

```bash
g++ -Wall -Wextra -g3 B-treeAPI/testador.cpp MusicClass/music.cpp databaseClass/database.cpp -o B-treeAPI/testador
cd B-treeAPI && ./testador
```

---

## 🔌 Endpoints da API

| Método | Rota | Descrição |
|--------|------|-----------|
| `GET` | `/music/{id}` | Busca por ID (Árvore B) |
| `GET` | `/music/name/{name}` | Busca por nome (prefixo) |
| `GET` | `/music/singer/{singer}` | Busca por artista (prefixo) |
| `GET` | `/music/album/{album}` | Busca por álbum (prefixo) |
| `POST` | `/music` | Adiciona uma nova música |

**Exemplo:**
```bash
curl http://localhost:5000/music/singer/beatles
```

---

## 🎨 Frontend — Rubra Music

Interface dark theme moderna, sem frameworks JS (vanilla). Responsiva para desktop, tablet e mobile.

- 🔍 Busca simultânea por nome, artista e álbum
- 👤 Cards de artistas com página de discografia
- 💿 Cards de álbuns clicáveis
- 🎵 Lista de músicas com duração, gênero e link externo
- 🔄 Indicador de conexão com o backend
- ⌨️ Atalho `Esc` para voltar aos resultados

---

## ✨ Funcionalidades

- Indexação primária com **Árvore B de ordem 4**
- Indexação secundária com **mapas invertidos** (busca por prefixo)
- Persistência em disco com **registros de tamanho fixo** e acesso direto via RRN
- Conversão automática de **CSV → binário** (~28 mil músicas)
- **API REST** com Drogon (busca + inserção)
- **Frontend web** responsivo com proxy reverso Python
- **Testador CLI** interativo
- **Docker** para containerização
