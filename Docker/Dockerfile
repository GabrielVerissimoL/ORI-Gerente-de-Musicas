FROM gcc:14

# Varíaveis ambiente
ENV TZ=America/Sao_Paulo

# diretório base dentro do docker
WORKDIR /bpi

# conteúdo presente no diretório interno
COPY Docker/libs.txt .

# bibliotecas disponíveis no container (docker)
RUN apt-get update 
RUN apt-get install  -y --no-install-recommends $(cat libs.txt)

# porta de acesso ao container
EXPOSE 5000

COPY . .

CMD ["/bin/bash", "-c", "make && chmod +x ./frontend/start-frontend.sh ./bin/projeto ./start-all.sh && ./start-all.sh"]