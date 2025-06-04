#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>

#define MAX_USUARIOS 100
#define MAX_FILMES 100
#define NAME_SIZE 100
#define GENRE_SIZE 30

// ------------------------- UTIL -----------------------------
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ---------------- STRUCTS ----------------
typedef struct Usuario {
    int id;
    char nome[NAME_SIZE];
    char generoFav[GENRE_SIZE];
    struct Usuario* proxUser;
} Usuario;

typedef struct Filme {
    int id;
    char nome[NAME_SIZE];
    char genero[GENRE_SIZE];
    struct Filme* proxFilme;
} Filme;

typedef struct No {
    int destino;
    struct No* prox;
} No;

typedef struct Grafo {
    No* adj[MAX_USUARIOS];
    int numVertices;
} Grafo;

// --------------------- FUNÇÕES DE USUÁRIO ---------------------

Usuario* criarUsuario(int id, const char* nome, const char* generoFav) {
    Usuario* u = (Usuario*) malloc(sizeof(Usuario));
    u->id = id;
    strncpy(u->nome, nome, NAME_SIZE);
    strncpy(u->generoFav, generoFav, GENRE_SIZE);
    u->proxUser = NULL;
    return u;
}

void adicionarUsuario(Usuario** lista, Usuario* novo) {
    novo->proxUser = *lista;
    *lista = novo;
}

Usuario* buscarUsuarioPorID(Usuario* lista, int id) {
    Usuario* atual = lista;
    while (atual != NULL) {
        if (atual->id == id) return atual;
        atual = atual->proxUser;
    }
    return NULL;
}

Usuario* buscarUsuarioPorNome(Usuario* lista, const char* nome) {
    Usuario* atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) return atual;
        atual = atual->proxUser;
    }
    return NULL;
}

int contarUsuarios(Usuario* lista) {
    int cont = 0;
    Usuario* atual = lista;
    while (atual != NULL) {
        cont++;
        atual = atual->proxUser;
    }
    return cont;
}

// Remove usuário da lista (sem liberar grafo)
int removerUsuario(Usuario** lista, int id) {
    Usuario* atual = *lista;
    Usuario* ant = NULL;
    while (atual != NULL) {
        if (atual->id == id) {
            if (ant == NULL) *lista = atual->proxUser;
            else ant->proxUser = atual->proxUser;
            free(atual);
            return 1; // sucesso
        }
        ant = atual;
        atual = atual->proxUser;
    }
    return 0; // não encontrado
}

// --------------------- FUNÇÕES DE FILME ---------------------

Filme* criarFilme(int id, const char* nome, const char* genero) {
    Filme* f = (Filme*) malloc(sizeof(Filme));
    f->id = id;
    strncpy(f->nome, nome, NAME_SIZE);
    strncpy(f->genero, genero, GENRE_SIZE);
    f->proxFilme = NULL;
    return f;
}

void adicionarFilme(Filme** lista, Filme* novo) {
    novo->proxFilme = *lista;
    *lista = novo;
}

Filme* buscarFilmePorGenero(Filme* lista, const char* genero) {
    Filme* atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->genero, genero) == 0) return atual;
        atual = atual->proxFilme;
    }
    return NULL;
}

// Listar todos filmes por gênero
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

// Evitar duplicação de arestas
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
    // Remover destino da lista de origem
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
    // Remover origem da lista de destino
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

// Reconstruir grafo (útil após remoção de usuário)
void reconstruirGrafo(Grafo* grafo, Usuario* listaUsuarios) {
    inicializarGrafo(grafo);
    Usuario* u1 = listaUsuarios;
    while (u1 != NULL) {
        Usuario* u2 = listaUsuarios;
        while (u2 != NULL) {
            if (u1 != u2 && strcmp(u1->generoFav, u2->generoFav) == 0) {
                adicionarAresta(grafo, u1->id, u2->id);
            }
            u2 = u2->proxUser;
        }
        u1 = u1->proxUser;
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

// --------------------- SUGESTÃO DE AMIZADES ---------------------

// Sugere amigos a partir dos amigos dos amigos (distância 2 no grafo)
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

    printf("Sugestões de amigos para [%d]:\n", userId);
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

// Sugere amizade baseada no gênero favorito (para o usuário)
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
            // Verificar se já são amigos
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

    // Criar vetor para organizar por ID
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

    // Organiza por ID
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

// --------------------- FUNÇÕES DE ARQUIVO (CARREGAR USUÁRIOS E FILMES) ---------------------

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
            Usuario* novo = criarUsuario(id, nome, generoFav);
            adicionarUsuario(lista, novo);
        }
    }
    fclose(f);
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
            Filme* novo = criarFilme(id, nome, genero);
            adicionarFilme(lista, novo);
        }
    }
    fclose(f);
}

// --------------------- MENU ---------------------

void menu() {
    setlocale(LC_ALL, "");
    Usuario* listaUsuarios = NULL;
    Filme* listaFilmes = NULL;
    Grafo grafo;
    inicializarGrafo(&grafo);

    // Carregar dados iniciais
    carregarUsuarios(&listaUsuarios, "usuarios.txt");
    carregarFilmes(&listaFilmes, "filmes.txt");

    // Contar max id e ajustar numVertices
    int maxId = -1;
    Usuario* utemp = listaUsuarios;
    while (utemp != NULL) {
        if (utemp->id > maxId) maxId = utemp->id;
        utemp = utemp->proxUser;
    }
    grafo.numVertices = maxId + 1;

    reconstruirGrafo(&grafo, listaUsuarios);

    int opcao = 0;

    while (1) {
    
        printf("\n=== MENU ===\n");
        printf("1. Listar usuários\n");
        printf("2. Adicionar usuário\n");
        printf("3. Remover usuário\n");
        printf("4. Listar filmes\n");
        printf("5. Criar amizade\n");
        printf("6. Listar rede de amizades\n");
        printf("7. Exportar rede de amizades para arquivo\n");
        printf("8. Listar amigos a partir de ID\n");
        printf("9. Sugestão de amizade por gênero\n");
        printf("10. Sugestão de amigos (amigos dos amigos)\n");
        printf("11. Sugestão de filmes\n");
        printf("12. Busca DFS\n");
        printf("13. Busca BFS\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // consumir \n

        if (opcao == 0) break;

        int id1, id2;
        char nome[NAME_SIZE], genero[GENRE_SIZE];

        switch (opcao) {
        
            case 1: {
                printf("Usuários cadastrados:\n");
                Usuario* utemp = listaUsuarios;
                while (utemp != NULL) {
                    printf("[%d] %s (Gênero favorito: %s)\n", utemp->id, utemp->nome, utemp->generoFav);
                    utemp = utemp->proxUser;
                }
                break;
            }

            case 2: {
                printf("Digite ID do novo usuário: ");
                scanf("%d", &id1);
                getchar();
                if (id1 < 0 || id1 >= MAX_USUARIOS) {
                    printf("ID inválido! Deve estar entre 0 e %d.\n", MAX_USUARIOS - 1);
                    break;
                }
                if (buscarUsuarioPorID(listaUsuarios, id1) != NULL) {
                    printf("ID já existe!\n");
                    break;
                }
                printf("Digite nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0; // remove \n
                printf("Digite gênero favorito: ");
                fgets(genero, sizeof(genero), stdin);
                genero[strcspn(genero, "\n")] = 0;

                Usuario* novo = criarUsuario(id1, nome, genero);
                adicionarUsuario(&listaUsuarios, novo);
                if (id1 >= grafo.numVertices) grafo.numVertices = id1 + 1;
                reconstruirGrafo(&grafo, listaUsuarios);
                printf("Usuário adicionado.\n");
                break;
            }

            case 3: {
                printf("Digite ID do usuário para remover: ");
                scanf("%d", &id1);
                getchar();
                if (removerUsuario(&listaUsuarios, id1)) {
                    printf("Usuário removido.\n");
                    reconstruirGrafo(&grafo, listaUsuarios);
                } else {
                    printf("Usuário não encontrado.\n");
                }
                break;
            }

            case 4: {
                printf("Filmes disponíveis:\n");
                Filme* ftemp = listaFilmes;
                while (ftemp != NULL) {
                    printf("[%d] %s (Gênero: %s)\n", ftemp->id, ftemp->nome, ftemp->genero);
                    ftemp = ftemp->proxFilme;
                }
                break;
            }

            case 5: {
    printf("Digite ID do usuário 1: ");
    scanf("%d", &id1);
    getchar();
    printf("Digite ID do usuário 2: ");
    scanf("%d", &id2);
    getchar();

    if (id1 < 0 || id1 >= grafo.numVertices || id2 < 0 || id2 >= grafo.numVertices) {
        printf("IDs inválidos.\n");
        break;
    }

    Usuario* u1 = buscarUsuarioPorID(listaUsuarios, id1);
    Usuario* u2 = buscarUsuarioPorID(listaUsuarios, id2);

    if (!u1 || !u2) {
        printf("Um ou ambos os usuários não existem.\n");
        break;
    }

    if (id1 == id2) {
        printf("Não é possível criar amizade consigo mesmo.\n");
        break;
    }

    if (existeAresta(grafo.adj[id1], id2)) {
        printf("Amizade já existe.\n");
    } else {
        adicionarAresta(&grafo, id1, id2);
        printf("Amizade criada entre %s e %s.\n", u1->nome, u2->nome);
    }
    break;
}


            case 6: {
                listarRedeAmizades(&grafo, listaUsuarios);
                break;
            }

            case 7: {
                exportarRedeAmizades(&grafo, listaUsuarios, "rede_amizades.txt");
                break;
            }

            case 8: {
                printf("Digite ID do usuário: ");
                scanf("%d", &id1);
                getchar();
                listarAmigos(&grafo, listaUsuarios, id1);
                break;
            }

            case 9: {
                printf("Digite ID do usuário: ");
                scanf("%d", &id1);
                getchar();
                sugerirAmizadePorGenero(&grafo, listaUsuarios, id1);
                break;
            }

            case 10: {
                printf("Digite ID do usuário: ");
                scanf("%d", &id1);
                getchar();
                sugerirAmigos(&grafo, id1, listaUsuarios);
                break;
            }

            case 11: {
                printf("Digite ID do usuário: ");
                scanf("%d", &id1);
                getchar();
                sugerirFilmes(listaFilmes, listaUsuarios, id1);
                break;
            }

            case 12: {
                printf("Digite ID do usuário para iniciar DFS: ");
                scanf("%d", &id1);
                getchar();
                dfs(&grafo, id1, listaUsuarios);
                break;
            }

            case 13: {
                printf("Digite ID do usuário para iniciar BFS: ");
                scanf("%d", &id1);
                getchar();
                bfs(&grafo, id1, listaUsuarios);
                break;
            }

            default:
                printf("Opção inválida.\n");
        }
         printf("Pressione ENTER para continuar..."); 
			getchar();
         limparTela();
        
    }
}


// --------------------- FUNÇÃO PRINCIPAL ---------------------

int main() {

SetConsoleOutputCP(CP_UTF8);
SetConsoleCP(CP_UTF8);

setlocale(LC_ALL, "");
printf("Teste: João, Ação, Café, Informação\n");
    menu();
    return 0;
}

