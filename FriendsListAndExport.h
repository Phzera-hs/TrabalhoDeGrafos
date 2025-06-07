#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>



// --------------------- LISTAGEM DE AMIGOS ---------------------
void listarAmigos(Grafo* grafo, Usuario* listaUsuarios, int userId) {
    Usuario* u = buscarUsuarioPorID(listaUsuarios, userId);
    if (!u) {
        printf("Usuário não encontrado.\n");
        return;
    }
    printf("Amigos de [%d] %s:\n", userId, u->nome);
    No* temp = grafo->adj[userId];
    if (!temp) {
        printf("Nenhum amigo.\n");
        return;
    }
    while (temp) {
        Usuario* amigo = buscarUsuarioPorID(listaUsuarios, temp->destino);
        if (amigo) {
            printf("- [%d] %s\n", amigo->id, amigo->nome);
        }
        temp = temp->prox;
    }
}

// --------------------- LISTAR REDE DE AMIZADES ---------------------
void listarRedeAmizades(Grafo* grafo, Usuario* listaUsuarios) {
    printf("===== REDE DE AMIZADES =====\n");
    Usuario* u = listaUsuarios;

    Usuario* usuariosPorID[MAX_USUARIOS] = {NULL};
    int maxID = -1;
    while (u != NULL) {
        usuariosPorID[u->id] = u;
        if (u->id > maxID) maxID = u->id;
        u = u->proxUser;
    }

    for (int i = 0; i <= maxID; i++) {
        if (usuariosPorID[i] != NULL) {
            printf("[%d] %s:  ", i, usuariosPorID[i]->nome);
            No* temp = grafo->adj[i];
            while (temp != NULL) {
                Usuario* amigo = usuariosPorID[temp->destino];
                if (amigo) {
                    printf("[%d] %s ? ", amigo->id, amigo->nome);
                }
                temp = temp->prox;
            }
            printf("\n");
        }
    }
    printf("============================\n");
}

// --------------------- EXPORTAR REDE DE AMIZADES ---------------------
void exportarRedeAmizades(Grafo* grafo, Usuario* listaUsuarios, const char* arquivo) {
    FILE* f = fopen(arquivo, "w");
    if (!f) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return;
    }

    fprintf(f, "======================\n");
    fprintf(f, "PESSOA         |     AMIGOS\n");

    Usuario* usuariosPorID[MAX_USUARIOS] = {NULL};
    int maxID = -1;
    Usuario* u = listaUsuarios;
    while (u != NULL) {
        usuariosPorID[u->id] = u;
        if (u->id > maxID) maxID = u->id;
        u = u->proxUser;
    }

    for (int i = 0; i <= maxID; i++) {
        if (usuariosPorID[i] != NULL) {
            fprintf(f, "%-15s | ", usuariosPorID[i]->nome);

            No* temp = grafo->adj[i];
            int primeiro = 1;
            while (temp != NULL) {
                Usuario* amigo = usuariosPorID[temp->destino];
                if (amigo) {
                    if (!primeiro) fprintf(f, ", ");
                    fprintf(f, "%s", amigo->nome);
                    primeiro = 0;
                }
                temp = temp->prox;
            }
            fprintf(f, "\n");
        }
    }
    fprintf(f, "======================\n");

    fclose(f);
    printf("Rede de amizades exportada para %s\n", arquivo);
}

void salvarAmizades(Grafo* grafo, Usuario* listaUsuarios, const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "w");
    if (!f) {
        printf("Erro ao abrir arquivo %s para escrita\n", nomeArquivo);
        return;
    }
    
    Usuario* usuariosPorID[MAX_USUARIOS] = {NULL};
    Usuario* atual = listaUsuarios;
    while (atual != NULL) {
        usuariosPorID[atual->id] = atual;
        atual = atual->proxUser;
    }

    for (int i = 0; i < grafo->numVertices; i++) {
        if (usuariosPorID[i] != NULL) {
            No* temp = grafo->adj[i];
            while (temp != NULL) {
                if (i < temp->destino && usuariosPorID[temp->destino] != NULL) {
                    fprintf(f, "%d;%d\n", i, temp->destino);
                }
                temp = temp->prox;
            }
        }
    }
    
    fclose(f);
}

void carregarAmizades(Grafo* grafo, Usuario* listaUsuarios, const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "r");
    if (!f) {
        return;
    }
    
    int id1, id2;
    while (fscanf(f, "%d;%d\n", &id1, &id2) == 2) {
        if (buscarUsuarioPorID(listaUsuarios, id1) && buscarUsuarioPorID(listaUsuarios, id2)) {
            adicionarAresta(grafo, id1, id2);
        }
    }
    
    fclose(f);
}
