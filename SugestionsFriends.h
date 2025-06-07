#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>


// --------------------- SUGESTÃO DE AMIZADES ---------------------
void sugerirAmigos(Grafo* grafo, int userId, Usuario* listaUsuarios) {
    int visitado[MAX_USUARIOS] = {0};
    int nivel[MAX_USUARIOS] = {0};
    int fila[MAX_USUARIOS];
    int front = 0, rear = 0;

    visitado[userId] = 1;
    nivel[userId] = 0;
    fila[rear++] = userId;

    while (front < rear) {
        int atual = fila[front++];
        No* temp = grafo->adj[atual];
        while (temp) {
            if (!visitado[temp->destino]) {
                visitado[temp->destino] = 1;
                nivel[temp->destino] = nivel[atual] + 1;
                fila[rear++] = temp->destino;
            }
            temp = temp->prox;
        }
    }

    Usuario* usuario = buscarUsuarioPorID(listaUsuarios, userId);
    if (usuario) {
        printf("Sugestões de amigos para [%s]:\n", usuario->nome);
    } else {
        printf("Sugestões de amigos para [ID %d]:\n", userId);
    }

    int encontrou = 0;
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (nivel[i] == 2) {
            Usuario* u = buscarUsuarioPorID(listaUsuarios, i);
            if (u) {
                printf("- [%d] %s\n", u->id, u->nome);
                encontrou = 1;
            }
        }
    }
    if (!encontrou) printf("Nenhuma sugestão encontrada.\n");
}

void sugerirAmizadePorGenero(Grafo* grafo, Usuario* listaUsuarios, int userId) {
    Usuario* u = buscarUsuarioPorID(listaUsuarios, userId);
    if (!u) {
        printf("Usuário não encontrado.\n");
        return;
    }
    printf("Sugestões de amizade para %s (gênero favorito: %s):\n", u->nome, u->generoFav);

    Usuario* atual = listaUsuarios;
    int encontrou = 0;

    while (atual != NULL) {
        if (atual->id != userId && strcmp(atual->generoFav, u->generoFav) == 0) {
            if (!existeAresta(grafo->adj[userId], atual->id)) {
                printf("- [%d] %s\n", atual->id, atual->nome);
                encontrou = 1;
            }
        }
        atual = atual->proxUser;
    }
    if (!encontrou) printf("Nenhuma sugestão encontrada.\n");
}

// --------------------- SUGESTÃO DE FILMES ---------------------
void sugerirFilmes(Filme* listaFilmes, Usuario* listaUsuarios, int userId) {
    Usuario* u = buscarUsuarioPorID(listaUsuarios, userId);
    if (!u) {
        printf("Usuário não encontrado.\n");
        return;
    }

    printf("Filmes sugeridos para %s (gênero favorito: %s):\n", u->nome, u->generoFav);
    listarFilmesPorGenero(listaFilmes, u->generoFav);
}
