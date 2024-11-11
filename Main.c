#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define VERMELHO "\033[31m"
#define VERDE "\033[32m"
#define AMARELO "\033[33m"
#define AZUL "\033[34m"
#define NEGRITO "\033[1m"
#define RESET "\033[0m"

#ifdef _WIN32
    #define RESOURCE_PATH "../resources/"
    #define CLEAR "cls"
#else
    #define RESOURCE_PATH "./resources/"
    #define CLEAR "clear"
#endif

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>

    void disableEcho() {
        struct termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    void enableEcho() {
        struct termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }
#endif

void configurarUTF8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void analisarSenha(char *senha) {
    int i = 0;
    char ch;
    printf("Digite a senha: ");

#ifdef _WIN32
    while (1) {
        ch = getch();

        if (ch == 13) {  // Enter
            break;
        } else if (ch == 8) {  // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            senha[i] = ch;
            i++;
            printf("*");
        }
    }
#else

    while ((ch = getchar()) != '\n' && ch != EOF);

    disableEcho();

    while (1) {
        ch = getchar();

        if (ch == '\n' || ch == '\r') {
            break;
        } else if (ch == 127 || ch == 8) { 
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            senha[i] = ch;
            i++;
            printf("*");
        }
    }

    enableEcho();
#endif

    senha[i] = '\0';
    printf("\n");
}

#define MAX_PRODUTOS 200
#define MAX_STRING_LENGTH 50
#define MAX_LINE_LENGTH 200

typedef struct {
    int id;
    char nome[50];
    float preco;
    int quantidade;
} Produto;

typedef struct {
    int idProduto;
    char nomeProduto[50];
    float precoUnitario;
    int quantidadeVendida;
    float valorTotal;
} Venda;

typedef enum { FUNCIONARIO, GERENTE, ADMIN } Perfil;

Produto produtos[MAX_PRODUTOS];
Venda vendas[MAX_PRODUTOS];
int totalProdutos = 0;
int numVendas = 0;

Perfil menuLogin();
void lerArquivo(const char *nomeArquivo);
void cadastrarProduto(Perfil perfil);
void exibirProdutos();
void exibirRelatorioVendas(Perfil perfil);
void realizarCompra();
void aguardarAcao();
void menuPrincipal(Perfil perfil);
void carregarProdutosInicio(int *totalProdutos);
void lerProdutos(FILE *file, Produto produtos[], int *totalProdutos);
void salvarProdutoNoCSV(Produto produto);
void atualizarCSV();
void codificarDecodificar(char *str, char chave);
int validarLogin(const char *login, const char *senha, Perfil *perfil);
void excluirUsuario(const char *login);
void exibirPerfil(Perfil perfil);
void cadastrarUsuario();
void gerenciarUsuarios(Perfil perfil);
void registrarUsuario(const char *login, const char *senha, Perfil perfil);
void analisarSenha(char *senha);
char* resourceFile(char *nome);
void atualizarProdutosEmEstoque(Perfil perfil);
void excluirProduto(Perfil perfil);

int main() {
    configurarUTF8();

    Perfil perfil;
    
    carregarProdutosInicio(&totalProdutos);
    perfil = menuLogin();

    menuPrincipal(perfil);

    return 0;
}

void menuPrincipal(Perfil perfil){
    int opcao;

    do {
        system(CLEAR);
        printf("\n%s%sSistema de Gerenciamento Hortifruti%s\n", NEGRITO, VERDE, RESET);
        exibirPerfil(perfil);
        printf("%s%s1.%s Cadastrar Produto\n", NEGRITO, VERMELHO, RESET);
        printf("%s%s2.%s Exibir Produtos\n", NEGRITO, AZUL, RESET);
        printf("%s%s3.%s Realizar Compra\n", NEGRITO, AZUL, RESET);
        printf("%s%s4.%s Relatório de Vendas\n", NEGRITO, VERMELHO, RESET);
        printf("%s%s5.%s Gerenciar Usuários\n", NEGRITO, VERMELHO, RESET);
        printf("%s%s6.%s Atualizar Produtos em Estoque\n", NEGRITO, AZUL, RESET);
        printf("%s%s7.%s Excluir Produtos\n", NEGRITO, VERMELHO, RESET);
        printf("%s%s0.%s Sair\n", NEGRITO, AZUL, RESET);
        printf("%sEscolha uma opção: %s", NEGRITO, RESET);
        scanf("%d", &opcao);

        system(CLEAR);

        switch (opcao) {
            case 1:
                cadastrarProduto(perfil);
                aguardarAcao();
                break;
            case 2:
                exibirProdutos();
                aguardarAcao();
                break;
            case 3:
                realizarCompra();
                aguardarAcao();
                break;
            case 4:
                exibirRelatorioVendas(perfil);
                aguardarAcao();
                break;
            case 5:
                gerenciarUsuarios(perfil);
                aguardarAcao();
                break;
            case 6:
                atualizarProdutosEmEstoque(perfil);
                aguardarAcao();
                break;
            case 7:
                excluirProduto(perfil);
                aguardarAcao();
                break;
            case 0:
                printf("%sEncerrando...%s\n", VERMELHO, RESET);
                break;
            default:
                printf("Opção Inválida!\n");
        }
    } while (opcao != 0);
}

Perfil menuLogin(){
    Perfil perfil;
    char login[50], senha[50];

    int validUser;
    do
    {
        system(CLEAR);
        lerArquivo(resourceFile("banner.txt"));
        printf("\n\nDigite o login: ");
        scanf("%s", login);
        analisarSenha(senha);

        validUser = validarLogin(login, senha, &perfil);
        switch (validUser) {
            case 0:
                system(CLEAR);
                printf("%s%sLogin ou senha inválidos.%s\n", VERMELHO, NEGRITO, RESET);
                break;
            case 1:
                system(CLEAR);
                printf("%s%sLogin bem-sucedido!%s\n", NEGRITO, VERDE, RESET);
                exibirPerfil(perfil);
                aguardarAcao();
                break;
            case 2:
                system(CLEAR);
                printf("%s%sNenhum usuário foi cadastrado ainda!%s\n", NEGRITO, VERMELHO, RESET);
                aguardarAcao();
                cadastrarUsuario();
                break;
            default:
                printf("%s%sLogin ou senha inválidos.%s\n", NEGRITO, VERMELHO, RESET);
                break;
        }
    } while (validUser != 1);
    return perfil;
}

void gerenciarUsuarios(Perfil perfil) {
    int escolha;
    char login[50];

    switch (perfil) {
        case FUNCIONARIO:
            printf("%sPerfil com Acesso Insuficiente (FUNCIONARIO)! Somente Perfis ADMIN possuem acesso para Gerenciar Usuarios!%s\n", VERMELHO, RESET);
            return;
        case GERENTE:
            printf("%sPerfil com Acesso Insuficiente (GERENTE)! Somente Perfis ADMIN possuem acesso para Gerenciar Usuarios!%s\n", VERMELHO, RESET);
            return;
        case ADMIN:
            break;
    }

    printf("1. Registrar novo usuário\n");
    printf("2. Excluir usuário\n");
    printf("Escolha uma opção: ");
    scanf("%d", &escolha);

    switch (escolha) {
        case 1:
            cadastrarUsuario();
            break;
        case 2:
            printf("Digite o login do usuário a ser excluído: ");
            scanf("%s", login);
            excluirUsuario(login);
            break;
        default:
            printf("%sOpção inválida!%s\n", VERMELHO, RESET);
    }
}

void cadastrarUsuario(){
    char login[50], senha[50];
    system(CLEAR);
    printf("%sCadastro de Novo Usuário: %s\n", NEGRITO, RESET);
    printf("Digite o login: ");
    scanf("%s", login);
    analisarSenha(senha);

    int escolhaPerfil;
    do
    {
        printf("\nEscolha o perfil (0: FUNCIONARIO, 1: GERENTE, 2: ADMIN): ");
        scanf("%d", &escolhaPerfil);
        if (escolhaPerfil < 0 || escolhaPerfil > 2) {
            printf("Perfil inválido. Usuário não registrado.\n");
        }
    } while (escolhaPerfil < 0 || escolhaPerfil > 2);

    registrarUsuario(login, senha, (Perfil)escolhaPerfil);
}

void cadastrarProduto(Perfil perfil) {

    switch (perfil) {
        case FUNCIONARIO:
            printf("%sPerfil com Acesso Insuficiente (FUNCIONARIO)! Somente Perfis GERENTE ou ADMIN possuem acesso para Gerenciar Usuarios!%s\n", VERMELHO, RESET);
            return;
        case GERENTE:
            break;
        case ADMIN:
            break;
    }

    if (totalProdutos < MAX_PRODUTOS) {
        produtos[totalProdutos].id = totalProdutos + 1;

        getchar();

        printf("Digite o nome: ");
        fgets(produtos[totalProdutos].nome, sizeof(produtos[totalProdutos].nome), stdin);
        produtos[totalProdutos].nome[strcspn(produtos[totalProdutos].nome, "\n")] = '\0';

        printf("Digite o preco: ");
        scanf("%f", &produtos[totalProdutos].preco);
        getchar(); 

        printf("Digite a quantidade: ");
        scanf("%d", &produtos[totalProdutos].quantidade);
        getchar(); 

        salvarProdutoNoCSV(produtos[totalProdutos]);

        totalProdutos++;
    } else {
        printf("%sLimite de registros atingido.%s\n", VERMELHO, RESET);
    }
}

void exibirProdutos() {
    printf("\n%sProdutos cadastrados:%s\n", NEGRITO, RESET);
    for (int i = 0; i < totalProdutos; i++) {
        printf("%s%sId:%s %d %s%s| Nome:%s %s %s%s| Preço: %sR$ %.2f %s%s| Quantidade:%s %d\n", NEGRITO, AZUL,RESET, produtos[i].id, NEGRITO, AZUL, RESET, produtos[i].nome, NEGRITO, AZUL, RESET, produtos[i].preco, NEGRITO, AZUL, RESET, produtos[i].quantidade);
    }
}

void realizarCompra() {
    int opcao, id, quantidade, i;
    char nome[50];
    float total = 0.0;

    Venda prodVendidos[MAX_PRODUTOS];
    int numV = 0;

    while (1) {
        printf("%sEscolha uma opção:%s\n", NEGRITO, RESET);
        printf("1. Selecionar produto por ID\n");
        printf("2. Selecionar produto por nome\n");
        printf("3. Finalizar compra\n");
        printf("4. Cancelar Compra\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        if (opcao == 3) {
            break;
        }

        if(opcao == 4){
            return;
        }

        switch (opcao) {
            case 1:
                printf("Digite o ID do produto: ");
                scanf("%d", &id);
                if(id == 0){
                    printf("%sProduto não encontrado.%s\n", VERMELHO, RESET);
                    break;
                }
                for (i = 0; i < totalProdutos; i++) {
                    if (produtos[i].id == id) {
                        printf("%sProduto encontrado: %s%s\n", NEGRITO, RESET, produtos[i].nome);
                        printf("%sPreço do Produto: %s%.2f\n", NEGRITO, RESET, produtos[i].preco);
                        printf("%sDigite a quantidade: %s", NEGRITO, RESET);
                        scanf("%d", &quantidade);
                        if (quantidade <= produtos[i].quantidade) {
                            produtos[i].quantidade -= quantidade;
                            total += produtos[i].preco * quantidade;

                            vendas[numVendas].idProduto = produtos[i].id;
                            strcpy(vendas[numVendas].nomeProduto, produtos[i].nome);
                            vendas[numVendas].precoUnitario = produtos[i].preco;
                            vendas[numVendas].quantidadeVendida = quantidade;
                            vendas[numVendas].valorTotal = produtos[i].preco * quantidade;
                            prodVendidos[numV] = vendas[numVendas];
                            numV++;
                            numVendas++;
                        } else {
                            printf("%sQuantidade indisponível.%s\n", VERMELHO, RESET);
                        }
                        break;
                    }
                }
                if (i == totalProdutos) {
                    printf("%sProduto não encontrado.%s\n", VERMELHO, RESET);
                }
                break;
            case 2:
                printf("Digite o nome do produto: ");
                scanf("%s", nome);
                for (i = 0; i < totalProdutos; i++) {
                    if (strcmp(produtos[i].nome, nome) == 0) {
                        printf("Produto encontrado: %s\n", produtos[i].nome);
                        printf("Digite a quantidade: ");
                        scanf("%d", &quantidade);
                        if (quantidade <= produtos[i].quantidade) {
                            produtos[i].quantidade -= quantidade;
                            total += produtos[i].preco * quantidade;

                            vendas[numVendas].idProduto = produtos[i].id;
                            strcpy(vendas[numVendas].nomeProduto, produtos[i].nome);
                            vendas[numVendas].precoUnitario = produtos[i].preco;
                            vendas[numVendas].quantidadeVendida = quantidade;
                            vendas[numVendas].valorTotal = produtos[i].preco * quantidade;
                            prodVendidos[numV] = vendas[numVendas];
                            numV++;
                            numVendas++;
                        } else {
                            printf("%sQuantidade indisponível.%s\n", VERMELHO, RESET);
                        }
                        break;
                    }
                }
                if (i == totalProdutos) {
                    printf("%sProduto não encontrado.\n%s", VERMELHO, RESET);
                }
                break;
            default:
                printf("%s%sOpção inválida.%s\n", VERMELHO, NEGRITO, RESET);
                break;
        }
    }

    system(CLEAR);
    printf("%sRegistro de Compra: %s\n", NEGRITO, RESET);
    printf("%sID Produto | Nome Produto | Preço Unitário | Quantidade Vendida | Valor Total%s\n", NEGRITO, RESET);
    for(int x = 0; x <= numV; x++){
        if(prodVendidos[x].idProduto != 0){
        printf("%d, %s, %.2f, %d, %.2f\n", prodVendidos[x].idProduto, prodVendidos[x].nomeProduto, prodVendidos[x].precoUnitario, prodVendidos[x].quantidadeVendida, prodVendidos[x].valorTotal);
        }
    }
    printf("\n\n%sValor total da compra: %sR$%.2f\n", NEGRITO, RESET, total);

    atualizarCSV();
}

void atualizarProdutosEmEstoque(Perfil perfil) {

    switch (perfil) {
        case FUNCIONARIO:
            printf("%sPerfil com Acesso Insuficiente (FUNCIONARIO)! Somente Perfis GERENTE ou ADMIN possuem acesso para Gerenciar Usuarios!%s\n", VERMELHO, RESET);
            return;
        case GERENTE:
            break;
        case ADMIN:
            break;
    }

    int opcao, id, i;
    char nome[50];

    while (1) {
        printf("%sEscolha uma opção:%s\n", NEGRITO, RESET);
        printf("1. Selecionar produto por ID\n");
        printf("2. Selecionar produto por nome\n");
        printf("3. Finalizar Atualização\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        if (opcao == 3) {
            break;
        }

        switch (opcao) {
            case 1:
                printf("Digite o ID do produto: ");
                scanf("%d", &id);
                for (i = 0; i < totalProdutos; i++) {
                    if (produtos[i].id == id) {
                        printf("%sProduto encontrado: %s%s\n", NEGRITO, RESET, produtos[i].nome);
                        printf("%sPreço do Produto: %s%.2f\n", NEGRITO, RESET, produtos[i].preco);
                        printf("%sQuantidade Registrada: %s%i\n", NEGRITO, RESET, produtos[i].quantidade);
                        printf("Digite o novo nome do Produto: ");
                        scanf("%s", produtos[i].nome);
                        printf("Digite o novo preço do Produto: ");
                        scanf("%f", &produtos[i].preco);
                        printf("Digite o nova quantidade do Produto: ");
                        scanf("%i", &produtos[i].quantidade);
                        break;
                    }
                }
                if (i == totalProdutos) {
                    printf("%sProduto não encontrado.%s\n", VERMELHO, RESET);
                }
                break;
            case 2:
                printf("Digite o nome do produto: ");
                scanf("%s", nome);
                for (i = 0; i < totalProdutos; i++) {
                    if (strcmp(produtos[i].nome, nome) == 0) {
                        printf("%sProduto encontrado: %s%s\n", NEGRITO, RESET, produtos[i].nome);
                        printf("%sPreço do Produto: %s%.2f\n", NEGRITO, RESET, produtos[i].preco);
                        printf("%sQuantidade Registrada: %s%i\n", NEGRITO, RESET, produtos[i].quantidade);
                        printf("Digite o novo nome do Produto: ");
                        scanf("%s", produtos[i].nome);
                        printf("Digite o novo preço do Produto: ");
                        scanf("%f", &produtos[i].preco);
                        printf("Digite o nova quantidade do Produto: ");
                        scanf("%i", &produtos[i].quantidade);
                        break;
                    }
                }
                if (i == totalProdutos) {
                    printf("%sProduto não encontrado.%s\n", VERMELHO, RESET);
                }
                break;
            default:
                printf("%sOpção inválida.%s\n", VERMELHO, RESET);
                break;
        }
    }

    system(CLEAR);

    printf("Produtos Atualizados!");

    atualizarCSV();
}

void excluirProduto(Perfil perfil) {

    switch (perfil) {
        case FUNCIONARIO:
            printf("Perfil com Acesso Insuficiente (FUNCIONARIO)! Somente Perfis GERENTE ou ADMIN possuem acesso para Gerenciar Usuarios!\n");
            return;
        case GERENTE:
            break;
        case ADMIN:
            break;
    }

    int opcao, id, i, j;
    char nome[50];

    printf("%sEscolha uma opção:%s\n", NEGRITO, RESET);
    printf("1. Selecionar produto por ID\n");
    printf("2. Selecionar produto por nome\n");
    printf("Opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
        case 1:
            printf("Digite o ID do produto: ");
            scanf("%d", &id);
            if(id == 0){
                printf("ID Inválido!");
                break;
            }

            for (i = 0; i < totalProdutos; i++) {
                if (produtos[i].id == id) {
                    printf("%sProduto encontrado: %s%s\n", NEGRITO, RESET, produtos[i].nome);
                    printf("%sPreço do Produto: %s%.2f\n", NEGRITO, RESET, produtos[i].preco);
                    printf("%sQuantidade Registrada: %s%i\n", NEGRITO, RESET, produtos[i].quantidade);
                    for (j = i; j < totalProdutos - 1; j++) {
                        produtos[j] = produtos[j + 1];
                    }
                    (totalProdutos)--;
                    printf("%s%sProduto com id %d excluído com sucesso!%s\n", NEGRITO, VERMELHO, id, RESET);
                    return;
                }
            }

            printf("%s%sProduto com id %d não encontrado.%s\n", NEGRITO, VERMELHO, id, RESET);
        case 2:
            printf("Digite o nome do produto: ");
            scanf("%s", nome);
            for (i = 0; i < totalProdutos; i++) {
                if (strcmp(produtos[i].nome, nome) == 0) {
                    printf("%sProduto encontrado: %s%s\n", NEGRITO, RESET, produtos[i].nome);
                    printf("%sPreço do Produto: %s%.2f\n", NEGRITO, RESET, produtos[i].preco);
                    printf("%sQuantidade Registrada: %s%i\n", NEGRITO, RESET, produtos[i].quantidade);
                    for (j = i; j < totalProdutos - 1; j++) {
                        produtos[j] = produtos[j + 1];
                    }
                    (totalProdutos)--;
                    printf("%s%sProduto com nome %s excluído com sucesso!%s\n", NEGRITO, VERMELHO, nome, RESET);
                    return;
                }
            }
            if (i == totalProdutos) {
                printf("%sProduto não encontrado.%s\n", VERMELHO, RESET);
            }
            break;
        default:
            printf("%sOpção inválida.%s\n", VERMELHO, RESET);
            break;
    }

    system(CLEAR);

    atualizarCSV();
}

void exibirRelatorioVendas(Perfil perfil) {
    switch (perfil) {
        case FUNCIONARIO:
            printf("%s%sPerfil com Acesso Insuficiente (FUNCIONARIO)! Somente Perfis GERENTE ou ADMIN possuem acesso para Gerenciar Usuarios!%s\n", NEGRITO, VERMELHO, RESET);
            return;
        case GERENTE:
            break;
        case ADMIN:
            break;
    }

    float valorTotalVendas = 0;

    printf("%sRelatório de Vendas:%s\n", NEGRITO, RESET);
    printf("%sID Produto | Nome Produto | Preço Unitário | Quantidade Vendida | Valor Total%s\n", NEGRITO, RESET);
    for (int i = 0; i < numVendas; i++) {
        printf("%d | %s | %.2f | %d | %.2f\n", vendas[i].idProduto, vendas[i].nomeProduto, vendas[i].precoUnitario, vendas[i].quantidadeVendida, vendas[i].valorTotal);
        valorTotalVendas += vendas[i].valorTotal;
    }
    printf("\n\n%sO valor total das vendas foi: %sR$%.2f", NEGRITO, RESET, valorTotalVendas);
}

void lerArquivo(const char *nomeArquivo) {
    FILE *file;
    char line[256];

    file = fopen(nomeArquivo, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        printf("%s%s%s%s", NEGRITO, VERDE, line, RESET);
    }

    fclose(file);
}

void aguardarAcao(){
    printf("\n\nPressione qualquer tecla para continuar...\n");
    while (getchar() != '\n');
    getchar();
}

void carregarProdutosInicio(int *totalProdutos){
    FILE *file = fopen(resourceFile("produtos.csv"), "r");
    if (file != NULL) {
        lerProdutos(file, produtos, totalProdutos);
        fclose(file);
    }
}

void lerProdutos(FILE *file, Produto produtos[], int *totalProdutos) {
    char line[MAX_STRING_LENGTH * 4];
    *totalProdutos = 0;

    // Ignorar a primeira linha
    if (fgets(line, sizeof(line), file) == NULL) {
        return; // Se o arquivo estiver vazio ou ocorrer um erro, sair da função
    }

    while (fgets(line, sizeof(line), file) && *totalProdutos < MAX_PRODUTOS) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
            sscanf(line, "%d;%49[^;];%f;%d",
                &produtos[*totalProdutos].id, produtos[*totalProdutos].nome,
                &produtos[*totalProdutos].preco, &produtos[*totalProdutos].quantidade);
            (*totalProdutos)++;
        }
    }
}

void salvarProdutoNoCSV(Produto produto) {
    FILE *file = fopen(resourceFile("produtos.csv"), "a");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "%d;%s;%.2f;%d\n", produto.id, produto.nome, produto.preco, produto.quantidade);

    fclose(file);
}

void atualizarCSV() {
    FILE *arquivo = fopen(resourceFile("produtos.csv"), "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(arquivo, "\n");

    for (int i = 0; i < totalProdutos; i++) {
        fprintf(arquivo, "%d;%s;%.2f;%d\n", produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
    }

    fclose(arquivo);
}

void codificarDecodificar(char *str, char chave) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] ^= chave; 
    }
}

int validarLogin(const char *login, const char *senha, Perfil *perfil) {
    FILE *arquivo = fopen(resourceFile("usuarios.dat"), "r");
    if (arquivo == NULL) {
        return 2;
    }

    char storedLogin[50], storedSenha[50];
    int storedPerfil;
    char chave = 123;

    while (fscanf(arquivo, "%s %s %d", storedLogin, storedSenha, &storedPerfil) != EOF) {
        codificarDecodificar(storedSenha, chave);

        if (strcmp(storedLogin, login) == 0 && strcmp(storedSenha, senha) == 0) {
            *perfil = (Perfil)storedPerfil;
            fclose(arquivo);
            return 1; 
        }
    }

    fclose(arquivo);
    return 0; 
}

void registrarUsuario(const char *login, const char *senha, Perfil perfil) {
    FILE *arquivo = fopen(resourceFile("usuarios.dat"), "a");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para registro!\n");
        return;
    }

    char chave = 123;
    char senhaCodificada[50];
    strcpy(senhaCodificada, senha);

    codificarDecodificar(senhaCodificada, chave);

    fprintf(arquivo, "%s %s %d\n", login, senhaCodificada, perfil);
    fclose(arquivo);

    printf("%s%sUsuário registrado com sucesso!%s\n", VERDE, NEGRITO, RESET);
}

void excluirUsuario(const char *login) {
    FILE *arquivo = fopen(resourceFile("usuarios.dat"), "r");
    FILE *temp = fopen(resourceFile("temp.dat"), "w");

    if (arquivo == NULL || temp == NULL) {
        perror("Erro ao abrir os arquivos!\n");
        return;
    }

    char storedLogin[50], storedSenha[50];
    int storedPerfil;

    int encontrado = 0;
    while (fscanf(arquivo, "%s %s %d", storedLogin, storedSenha, &storedPerfil) != EOF) {
        if (strcmp(storedLogin, login) == 0) {
            encontrado = 1;
            continue; // Ignora a escrita do usuário a ser excluído
        }
        fprintf(temp, "%s %s %d\n", storedLogin, storedSenha, storedPerfil); // Escreve os outros usuários
    }

    fclose(arquivo);
    fclose(temp);

    if (encontrado) {
        if (remove(resourceFile("usuarios.dat")) == 0) {
            if (rename(resourceFile("temp.dat"), resourceFile("usuarios.dat")) == 0) {
                printf("%s%sUsuário %s excluído com sucesso!%s\n", VERMELHO, NEGRITO, login, RESET);
            } else {
                perror("Erro ao renomear o arquivo temporário.\n");
            }
        } else {
            perror("Erro ao remover o arquivo original.\n");
        }
    } else {
        remove(resourceFile("temp.dat"));
        printf("%s%sUsuário não encontrado!%s\n", VERMELHO, NEGRITO, RESET);
    }
}


char* resourceFile(char *nome) {
    char *fullPath = malloc(strlen(RESOURCE_PATH) + strlen(nome) + 1);

    if (fullPath == NULL) {
        perror("Erro na alocação de memória.\n");
        return NULL; 
    }

    strcpy(fullPath, RESOURCE_PATH);

    strcat(fullPath, nome);

    return fullPath; 
}

void exibirPerfil(Perfil perfil) {
    switch (perfil) {
        case FUNCIONARIO:
            printf("%sPerfil: %sFUNCIONARIO\n", NEGRITO, RESET);
            break;
        case GERENTE:
            printf("%sPerfil: %sGERENTE\n", NEGRITO, RESET);
            break;
        case ADMIN:
            printf("%sPerfil: %sADMIN\n", NEGRITO, RESET);
            break;
        default:
            printf("%sPerfil desconhecido%s\n", NEGRITO, RESET);
    }
}