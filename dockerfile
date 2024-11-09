# Use uma imagem base com GCC para compilar o código C
FROM gcc:latest

# Define o diretório de trabalho no contêiner
WORKDIR /app

# Copia o arquivo main.c e a pasta resources para o contêiner
COPY Main.c .
COPY ./resources ./resources

# Compila o código C
RUN gcc -o hortifresh Main.c

# Comando para manter o app rodando em loop
CMD while true; do ./hortifresh; sleep 1; done