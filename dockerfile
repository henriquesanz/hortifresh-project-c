# Use uma imagem base com GCC para compilar o código C
FROM gcc:latest

# Instala dependências necessárias para compilar o ttyd
RUN apt-get update && \
    apt-get install -y \
    cmake \
    git \
    build-essential \
    libjson-c-dev \
    libwebsockets-dev && \
    rm -rf /var/lib/apt/lists/*

# Clona e compila o ttyd
RUN git clone https://github.com/tsl0922/ttyd.git && \
    cd ttyd && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# Define o diretório de trabalho no contêiner
WORKDIR /app

# Copia o arquivo main.c e a pasta resources para o contêiner
COPY Main.c .
COPY ./resources ./resources

# Compila o código C
RUN gcc -o hortifresh Main.c

# Expõe a porta 7681 para acesso ao ttyd
EXPOSE 7681

# Comando para iniciar o ttyd em modo gravável
CMD ["ttyd", "--writable", "-p", "7681", "./hortifresh"]