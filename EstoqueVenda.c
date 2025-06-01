#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC 100
#define MAX_ITENS 10

typedef struct {
    int id;
    char descricao[MAX_DESC];
    float preco;
    int quantidade;
} Produto;

typedef struct {
    int id;
    int idsProdutos[MAX_ITENS];
    int qtdProdutos;
} Venda;


void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void cadastrarProduto() {
    FILE *f = fopen("produto.dat", "ab");
    if (!f) {
        perror("Erro ao abrir produto.dat");
        return;
    }

    Produto p;
    printf("ID do produto: ");
    scanf("%d", &p.id);
    limparBuffer();
    printf("Descrição: ");
    fgets(p.descricao, MAX_DESC, stdin);
    p.descricao[strcspn(p.descricao, "\n")] = '\0';
    printf("Preço: ");
    scanf("%f", &p.preco);
    printf("Quantidade em estoque: ");
    scanf("%d", &p.quantidade);

    fwrite(&p, sizeof(Produto), 1, f);
    fclose(f);
    printf("Produto cadastrado com sucesso!\n");
}

void listarProdutos() {
    FILE *f = fopen("produto.dat", "rb");
    if (!f) {
        printf("Nenhum produto encontrado.\n");
        return;
    }

    Produto p;
    printf("\n=== Lista de Produtos ===\n");
    while (fread(&p, sizeof(Produto), 1, f)) {
        printf("ID: %d | %s | Preço: R$%.2f | Estoque: %d\n",
               p.id, p.descricao, p.preco, p.quantidade);
    }

    fclose(f);
}

void atualizarProduto() {
    FILE *f = fopen("produto.dat", "rb+");
    if (!f) {
        perror("Erro ao abrir produto.dat");
        return;
    }

    int id;
    printf("ID do produto a atualizar: ");
    scanf("%d", &id);
    limparBuffer();

    Produto p;
    while (fread(&p, sizeof(Produto), 1, f)) {
        if (p.id == id) {
            fseek(f, -sizeof(Produto), SEEK_CUR);

            printf("Nova descrição: ");
            fgets(p.descricao, MAX_DESC, stdin);
            p.descricao[strcspn(p.descricao, "\n")] = '\0';
            printf("Novo preço: ");
            scanf("%f", &p.preco);
            printf("Nova quantidade: ");
            scanf("%d", &p.quantidade);

            fwrite(&p, sizeof(Produto), 1, f);
            printf("Produto atualizado com sucesso.\n");
            fclose(f);
            return;
        }
    }

    printf("Produto não encontrado.\n");
    fclose(f);
}

void removerProduto() {
    int id;
    printf("ID do produto a remover: ");
    scanf("%d", &id);

    FILE *f = fopen("produto.dat", "rb");
    FILE *tmp = fopen("temp_produto.dat", "wb");

    if (!f || !tmp) {
        perror("Erro ao abrir arquivos");
        return;
    }

    Produto p;
    int encontrado = 0;

    while (fread(&p, sizeof(Produto), 1, f)) {
        if (p.id != id) {
            fwrite(&p, sizeof(Produto), 1, tmp);
        } else {
            encontrado = 1;
        }
    }

    fclose(f);
    fclose(tmp);

    if (encontrado) {
        remove("produto.dat");
        rename("temp_produto.dat", "produto.dat");
        printf("Produto removido com sucesso.\n");
    } else {
        remove("temp_produto.dat");
        printf("Produto com ID %d não encontrado.\n", id);
    }
}


void cadastrarVenda() {
    FILE *fVenda = fopen("venda.dat", "ab");
    if (!fVenda) {
        perror("Erro ao abrir venda.dat");
        return;
    }

    Venda v;
    printf("ID da venda: ");
    scanf("%d", &v.id);
    printf("Quantidade de produtos na venda (máx %d): ", MAX_ITENS);
    scanf("%d", &v.qtdProdutos);
    if (v.qtdProdutos > MAX_ITENS) v.qtdProdutos = MAX_ITENS;

    for (int i = 0; i < v.qtdProdutos; i++) {
        printf("ID do produto %d: ", i + 1);
        scanf("%d", &v.idsProdutos[i]);
    }

    fwrite(&v, sizeof(Venda), 1, fVenda);
    fclose(fVenda);
    printf("Venda registrada com sucesso!\n");
}

void listarVendasComProdutos() {
    FILE *fVenda = fopen("venda.dat", "rb");
    FILE *fProd = fopen("produto.dat", "rb");

    if (!fVenda || !fProd) {
        printf("Erro ao abrir arquivos.\n");
        if (fVenda) fclose(fVenda);
        if (fProd) fclose(fProd);
        return;
    }

    Venda v;
    Produto p;

    printf("\n=== Relatório de Vendas ===\n");
    while (fread(&v, sizeof(Venda), 1, fVenda)) {
        printf("Venda ID: %d\n", v.id);
        printf("Produtos:\n");

        for (int i = 0; i < v.qtdProdutos; i++) {
            rewind(fProd);
            int encontrado = 0;
            while (fread(&p, sizeof(Produto), 1, fProd)) {
                if (p.id == v.idsProdutos[i]) {
                    printf("  - %s (R$%.2f)\n", p.descricao, p.preco);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                printf("  - Produto com ID %d não encontrado.\n", v.idsProdutos[i]);
            }
        }
        printf("\n");
    }

    fclose(fVenda);
    fclose(fProd);
}

void removerVenda() {
    int id;
    printf("ID da venda a remover: ");
    scanf("%d", &id);

    FILE *f = fopen("venda.dat", "rb");
    FILE *tmp = fopen("temp_venda.dat", "wb");

    if (!f || !tmp) {
        perror("Erro ao abrir arquivos.");
        if (f) fclose(f);
        if (tmp) fclose(tmp);
        return;
    }

    Venda v;
    int encontrado = 0;

    while (fread(&v, sizeof(Venda), 1, f)) {
        if (v.id != id) {
            fwrite(&v, sizeof(Venda), 1, tmp);
        } else {
            encontrado = 1;
        }
    }

    fclose(f);
    fclose(tmp);

    if (encontrado) {
        remove("venda.dat");
        rename("temp_venda.dat", "venda.dat");
        printf("Venda removida com sucesso.\n");
    } else {
        remove("temp_venda.dat");
        printf("Venda com ID %d não encontrada.\n", id);
    }
}


void menu() {
    int op;

    do {
        printf("\n==== MENU ====\n");
        printf("1. Cadastrar Produto\n");
        printf("2. Cadastrar Venda\n");
        printf("3. Listar Produtos\n");
        printf("4. Relatório de Vendas\n");
        printf("5. Atualizar Produto\n");
        printf("6. Remover Produto\n");
        printf("7. Remover Venda\n"); 
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);
        limparBuffer();

        switch (op) {
            case 1: cadastrarProduto(); break;
            case 2: cadastrarVenda(); break;
            case 3: listarProdutos(); break;
            case 4: listarVendasComProdutos(); break;
            case 5: atualizarProduto(); break;
            case 6: removerProduto(); break;
            case 7: removerVenda(); break;  
            case 0: printf("Saindo...\n"); break;
            default: printf("Opção inválida!\n");
        }

    } while (op != 0);
}

int main() {
    menu();
    return 0;
}
