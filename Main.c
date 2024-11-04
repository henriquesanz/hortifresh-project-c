#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_PRODUTOS 200
#define MAX_STRING_LENGTH 50
#define MAX_LINE_LENGTH 200

//TODO: Incluir Exclusão de Produtos

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

Produto produtos[MAX_PRODUTOS];
Venda vendas[MAX_PRODUTOS];
static char pathProdutosCsv[] = "../resources/produtos.csv";
int totalProdutos = 0;
int numVendas = 0;

void lerArquivo(const char *nomeArquivo);
void cadastrarProduto();
void exibirProdutos();
void exibirRelatorioVendas();
void realizarCompra();
void aguardarAcao();
void menuPrincipal();
void carregarProdutosInicio(int *totalProdutos);
void lerProdutos(FILE *file, Produto produtos[], int *totalProdutos);
void salvarProdutoNoCSV(Produto produto);
void atualizarCSV();

int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    lerArquivo("../resources/banner.txt");
    aguardarAcao();
    carregarProdutosInicio(&totalProdutos);
    menuPrincipal();

    return 0;
}

void menuPrincipal(){
    int opcao;

    do {
        system("cls");
        printf("\nSistema de Gerenciamento Hortifruti\n");
        printf("1. Cadastrar Produto\n");
        printf("2. Exibir Produtos\n");
        printf("3. Realizar Compra\n");
        printf("4. Relatório de Vendas\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        system("cls");

        switch (opcao) {
            case 1:
                cadastrarProduto();
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
                exibirRelatorioVendas();
                aguardarAcao();
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opção Inválida!\n");
        }
    } while (opcao != 0);
}

void cadastrarProduto() {
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
        printf("Limite de registros atingido.\n");
    }
}

void exibirProdutos() {
    printf("\nProdutos cadastrados:\n");
    for (int i = 0; i < totalProdutos; i++) {
        printf("Id: %d | Nome: %s | Preço: R$ %.2f | Quantidade: %d\n", produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
    }
}

void realizarCompra() {
    int opcao, id, quantidade, i;
    char nome[50];
    float total = 0.0;

    while (1) {
        printf("Escolha uma opção:\n");
        printf("1. Selecionar produto por ID\n");
        printf("2. Selecionar produto por nome\n");
        printf("3. Finalizar compra\n");
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
                        printf("Produto encontrado: %s\n", produtos[i].nome);
                        printf("Preço do Produto: %.2f\n", produtos[i].preco);
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
                            numVendas++;
                        } else {
                            printf("Quantidade indisponível.\n");
                        }
                        break;
                    }
                }
                if (i == totalProdutos) {
                    printf("Produto não encontrado.\n");
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
                            numVendas++;
                        } else {
                            printf("Quantidade indisponível.\n");
                        }
                        break;
                    }
                }
                if (i == totalProdutos) {
                    printf("Produto não encontrado.\n");
                }
                break;
            default:
                printf("Opção inválida.\n");
                break;
        }
    }

    printf("Valor total da compra: R$%.2f\n", total);

    atualizarCSV();
}

void exibirRelatorioVendas() {
    printf("Relatório de Vendas:\n");
    printf("ID Produto | Nome Produto | Preço Unitário | Quantidade Vendida | Valor Total\n");
    for (int i = 0; i < numVendas; i++) {
        printf("%d | %s | %.2f | %d | %.2f\n", vendas[i].idProduto, vendas[i].nomeProduto, vendas[i].precoUnitario, vendas[i].quantidadeVendida, vendas[i].valorTotal);
    }
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
        printf("%s", line);
    }

    fclose(file);
}

void aguardarAcao(){
    printf("\n\nPressione qualquer tecla para continuar...\n");
    while (getchar() != '\n');
    getchar();
}

void carregarProdutosInicio(int *totalProdutos){
    FILE *file = fopen(pathProdutosCsv, "r");
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
    FILE *file = fopen(pathProdutosCsv, "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "%d;%s;%.2f;%d\n", produto.id, produto.nome, produto.preco, produto.quantidade);

    fclose(file);
}

void atualizarCSV() {
    FILE *arquivo = fopen(pathProdutosCsv, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(arquivo, "\n");

    for (int i = 0; i < totalProdutos; i++) {
        fprintf(arquivo, "%d;%s;%.2f;%d\n", produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
    }

    fclose(arquivo);
}