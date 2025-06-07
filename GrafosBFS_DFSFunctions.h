#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>


// --------------------- FUNÇÕES DE GRAFO ---------------------
void inicializarGrafo(Grafo* grafo) {
    for (int i = 0; i < MAX_USUARIOS; i++) {
        grafo->adj[i] = NULL;
    }
    grafo->numVertices = 0;
}

No* criarNo(int destino) {
    No* novo = (No*) malloc(sizeof(No));
    novo->destino = destino;
    novo->prox = NULL;
    return novo;
}

int existeAresta(No* lista, int destino) {
    No* atual = lista;
    while (atual != NULL) {
        if (atual->destino == destino) return 1;
        atual = atual->prox;
    }
    return 0;
}

void adicionarAresta(Grafo* grafo, int origem, int destino) {
    if (origem < 0 || destino < 0 || origem >= MAX_USUARIOS || destino >= MAX_USUARIOS) return;

    if (!existeAresta(grafo->adj[origem], destino)) {
        No* novo = criarNo(destino);
        novo->prox = grafo->adj[origem];
        grafo->adj[origem] = novo;
    }

    if (!existeAresta(grafo->adj[destino], origem)) {
        No* novo = criarNo(origem);
        novo->prox = grafo->adj[destino];
        grafo->adj[destino] = novo;
    }
}

void removerAresta(Grafo* grafo, int origem, int destino) {
    No **ptr = &grafo->adj[origem];
    while (*ptr) {
        if ((*ptr)->destino == destino) {
            No* temp = *ptr;
            *ptr = (*ptr)->prox;
            free(temp);
            break;
        }
        ptr = &((*ptr)->prox);
    }
    ptr = &grafo->adj[destino];
    while (*ptr) {
        if ((*ptr)->destino == origem) {
            No* temp = *ptr;
            *ptr = (*ptr)->prox;
            free(temp);
            break;
        }
        ptr = &((*ptr)->prox);
    }
}

// --------------------- BUSCA DFS E BFS ---------------------
void dfsUtil(Grafo* grafo, int v, int visitado[], Usuario* listaUsuarios) {
    visitado[v] = 1;
    Usuario* u = buscarUsuarioPorID(listaUsuarios, v);
    if (u) printf("[%d] %s ", v, u->nome);

    No* temp = grafo->adj[v];
    while (temp) {
        if (!visitado[temp->destino]) {
            dfsUtil(grafo, temp->destino, visitado, listaUsuarios);
        }
        temp = temp->prox;
    }
}

void dfs(Grafo* grafo, int start, Usuario* listaUsuarios) {
    int visitado[MAX_USUARIOS] = {0};
    printf("DFS a partir de [%d]: ", start);
    dfsUtil(grafo, start, visitado, listaUsuarios);
    printf("\n");
}

void bfs(Grafo* grafo, int start, Usuario* listaUsuarios) {
    int visitado[MAX_USUARIOS] = {0};
    int fila[MAX_USUARIOS];
    int front = 0, rear = 0;

    visitado[start] = 1;
    fila[rear++] = start;

    printf("BFS a partir de [%d]: ", start);

    while (front < rear) {
        int v = fila[front++];
        Usuario* u = buscarUsuarioPorID(listaUsuarios, v);
        if (u) printf("[%d] %s ", v, u->nome);

        No* temp = grafo->adj[v];
        while (temp) {
            if (!visitado[temp->destino]) {
                visitado[temp->destino] = 1;
                fila[rear++] = temp->destino;
            }
            temp = temp->prox;
        }
    }
    printf("\n");
}
