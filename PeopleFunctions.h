#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>


// --------------------- FUNÇÕES DE USUÁRIO ---------------------
void salvarUsuarios(Usuario* lista, const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "w");
    if (!f) {
        printf("Erro ao abrir arquivo %s para escrita\n", nomeArquivo);
        return;
    }
    
    Usuario* atual = lista;
    while (atual != NULL) {
        fprintf(f, "%d;%s;%s\n", atual->id, atual->nome, atual->generoFav);
        atual = atual->proxUser;
    }
    
    fclose(f);
}

void adicionarUsuario(Usuario** lista, Usuario* novo) {
    novo->proxUser = NULL;
    if (*lista == NULL) {
        *lista = novo;
    } else {
        Usuario* atual = *lista;
        while (atual->proxUser != NULL) {
            atual = atual->proxUser;
        }
        atual->proxUser = novo;
    }
}

Usuario* criarUsuarioSemMenu(int id, const char* nome, const char* generoFav) {
    Usuario* u = (Usuario*) malloc(sizeof(Usuario));
    u->id = id;
    strncpy(u->nome, nome, NAME_SIZE);
    
    int valido = 0;
    for (int i = 0; i < 6; i++) {
        if (strcmp(generoFav, CATEGORIAS_FILMES[i]) == 0) {
            valido = 1;
            break;
        }
    }
    
    if (!valido) {
        printf("Gênero inválido: %s. Usando 'Acao' como padrão.\n", generoFav);
        strncpy(u->generoFav, CATEGORIAS_FILMES[0], GENRE_SIZE);
    } else {
        strncpy(u->generoFav, generoFav, GENRE_SIZE);
    }
    
    u->proxUser = NULL;
    return u;
}

Usuario* criarUsuario(int id, const char* nome) {
    Usuario* u = (Usuario*) malloc(sizeof(Usuario));
    u->id = id;
    strncpy(u->nome, nome, NAME_SIZE);
    
    printf("\nEscolha o gênero favorito:\n");
    for (int i = 0; i < 6; i++) {
        printf("%d - %s\n", i+1, CATEGORIAS_FILMES[i]);
    }
    printf("Digite o número da categoria: ");
    
    int opcao;
    scanf("%d", &opcao);
    getchar();
    
    if (opcao < 1 || opcao > 6) {
        printf("Opção inválida! Usando 'Acao' como padrão.\n");
        strncpy(u->generoFav, CATEGORIAS_FILMES[0], GENRE_SIZE);
    } else {
        strncpy(u->generoFav, CATEGORIAS_FILMES[opcao-1], GENRE_SIZE);
    }
    
    u->proxUser = NULL;
    return u;
}

void carregarUsuarios(Usuario** lista, const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return;
    }
    char linha[200];
    while (fgets(linha, sizeof(linha), f)) {
        int id;
        char nome[NAME_SIZE];
        char generoFav[GENRE_SIZE];
        if (sscanf(linha, "%d;%[^;];%[^\n]", &id, nome, generoFav) == 3) {
            Usuario* novo = criarUsuarioSemMenu(id, nome, generoFav);
            adicionarUsuario(lista, novo);
        }
    }
    fclose(f);
}

Usuario* buscarUsuarioPorID(Usuario* lista, int id) {
    Usuario* atual = lista;
    while (atual != NULL) {
        if (atual->id == id) return atual;
        atual = atual->proxUser;
    }
    return NULL;
}

int removerUsuario(Usuario** lista, int id) {
    Usuario* atual = *lista;
    Usuario* ant = NULL;
    while (atual != NULL) {
        if (atual->id == id) {
            if (ant == NULL) *lista = atual->proxUser;
            else ant->proxUser = atual->proxUser;
            free(atual);
            return 1;
        }
        ant = atual;
        atual = atual->proxUser;
    }
    return 0;
}

