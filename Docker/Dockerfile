FROM gcc:14

# Varíaveis ambiente
ENV TZ=America/Sao_Paulo

# diretório base dentro do docker
WORKDIR bpi/

# conteúdo presente no diretório interno
COPY ../libs.txt .

# bibliotecas disponíveis no container (docker)
RUN sudo apt-get update 
RUN sudo apt-get install  -y --no-install-recommends < ../libs.txt

# porta de acesso ao container
EXPOSE 5000

# executa o código da API
RUN ["./APIexe"]
