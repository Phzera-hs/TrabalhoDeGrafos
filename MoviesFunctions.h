#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
// --------------------- FUNÇÕES DE FILME ---------------------
void salvarFilmes(Filme* lista, const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "w");
    if (!f) {
        printf("Erro ao abrir arquivo %s para escrita\n", nomeArquivo);
        return;
    }
    
    Filme* atual = lista;
    while (atual != NULL) {
        fprintf(f, "%d;%s;%s\n", atual->id, atual->nome, atual->genero);
        atual = atual->proxFilme;
    }
    
    fclose(f);
}

void adicionarFilme(Filme** lista, Filme* novo) {
    novo->proxFilme = NULL; 
    if (*lista == NULL) {
        *lista = novo;
    } else {
        Filme* atual = *lista;
        while (atual->proxFilme != NULL) {
            atual = atual->proxFilme;
        }
        atual->proxFilme = novo;
    }
}

Filme* criarFilmeSemMenu(int id, const char* nome, const char* genero) {
    Filme* f = (Filme*) malloc(sizeof(Filme));
    f->id = id;
    strncpy(f->nome, nome, NAME_SIZE);
    
    int valido = 0;
    for (int i = 0; i < 6; i++) {
        if (strcmp(genero, CATEGORIAS_FILMES[i]) == 0) {
            valido = 1;
            break;
        }
    }
    
    if (!valido) {
        printf("Gênero inválido: %s. Usando 'Acao' como padrão.\n", genero);
        strncpy(f->genero, CATEGORIAS_FILMES[0], GENRE_SIZE);
    } else {
        strncpy(f->genero, genero, GENRE_SIZE);
    }
    
    f->proxFilme = NULL;
    return f;
}

Filme* criarFilme(int id, const char* nome) {
    Filme* f = (Filme*) malloc(sizeof(Filme));
    f->id = id;
    strncpy(f->nome, nome, NAME_SIZE);
    
    printf("\nEscolha o gênero do filme:\n");
    for (int i = 0; i < 6; i++) {
        printf("%d - %s\n", i+1, CATEGORIAS_FILMES[i]);
    }
    printf("Digite o número da categoria: ");
    
    int opcao;
    scanf("%d", &opcao);
    getchar();
    
    if (opcao < 1 || opcao > 6) {
        printf("Opção inválida! Usando 'Acao' como padrão.\n");
        strncpy(f->genero, CATEGORIAS_FILMES[0], GENRE_SIZE);
    } else {
        strncpy(f->genero, CATEGORIAS_FILMES[opcao-1], GENRE_SIZE);
    }
    
    f->proxFilme = NULL;
    return f;
}

void carregarFilmes(Filme** lista, const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return;
    }
    char linha[200];
    while (fgets(linha, sizeof(linha), f)) {
        int id;
        char nome[NAME_SIZE];
        char genero[GENRE_SIZE];
        if (sscanf(linha, "%d;%[^;];%[^\n]", &id, nome, genero) == 3) {
            Filme* novo = criarFilmeSemMenu(id, nome, genero);
            adicionarFilme(lista, novo);
        }
    }
    fclose(f);
}

int removerFilme(Filme** lista, int id) {
    Filme* atual = *lista;
    Filme* ant = NULL;
    while (atual != NULL) {
        if (atual->id == id) {
            if (ant == NULL) *lista = atual->proxFilme;
            else ant->proxFilme = atual->proxFilme;
            free(atual);
            return 1;
        }
        ant = atual;
        atual = atual->proxFilme;
    }
    return 0;
}

void criarFilmeMenu(Filme** listaFilmes) {
    int novoId = 0;
    Filme* temp = *listaFilmes;
    while (temp != NULL) {
        if (temp->id >= novoId) {
            novoId = temp->id + 1;
        }
        temp = temp->proxFilme;
    }

    printf("\n=== CRIAR NOVO FILME ===\n");
    printf("ID gerado automaticamente: %d\n", novoId);
    
    char nome[NAME_SIZE];
    printf("Digite o nome do filme: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';
    
    Filme* novo = criarFilme(novoId, nome);
    adicionarFilme(listaFilmes, novo);
    
    printf("\nFilme criado com sucesso!\n");
    printf("[%d] %s (%s)\n", novo->id, novo->nome, novo->genero);
}

void listarFilmesPorGenero(Filme* lista, const char* genero) {
    Filme* atual = lista;
    int count = 0;
    printf("Filmes do gênero '%s':\n", genero);
    while (atual != NULL) {
        if (strcmp(atual->genero, genero) == 0) {
            printf("- %s\n", atual->nome);
            count++;
        }
        atual = atual->proxFilme;
    }
    if (count == 0) {
        printf("Nenhum filme encontrado para este gênero.\n");
    }
}

