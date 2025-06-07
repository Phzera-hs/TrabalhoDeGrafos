#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>

#define MAX_USUARIOS 100
#define MAX_FILMES 100
#define NAME_SIZE 100
#define GENRE_SIZE 30

#include "utils.h"
#include "StructsAndCategoriasPadrao.h"
#include "PeopleFunctions.h"
#include "MoviesFunctions.h"
#include "GrafosBFS_DFSFunctions.h"
#include "SugestionsFriends.h"
#include "FriendsListAndExport.h"






// --------------------- FUN��O MENU ---------------------
void menu() {
    setlocale(LC_ALL, "");
    Usuario* listaUsuarios = NULL;
    Filme* listaFilmes = NULL;
    Grafo grafo;
    inicializarGrafo(&grafo);

    carregarUsuarios(&listaUsuarios, "usuarios.txt");
    carregarFilmes(&listaFilmes, "filmes.txt");
    carregarAmizades(&grafo, listaUsuarios, "amizades.txt");

    int maxId = -1;
    Usuario* utemp = listaUsuarios;
    while (utemp != NULL) {
        if (utemp->id > maxId) maxId = utemp->id;
        utemp = utemp->proxUser;
    }
    grafo.numVertices = maxId + 1;

    int opcao = 0;

    while (1) {
        printf("\n=== MENU ===\n");
        printf("1. Listar usu�rios\n");
        printf("2. Adicionar usu�rio\n");
        printf("3. Remover usu�rio\n");
        printf("4. Adicionar filme\n");
        printf("5. Remover filme\n");
        printf("6. Listar filmes\n");
        printf("7. Criar amizade\n");
        printf("8. Listar rede de amizades\n");
        printf("9. Exportar rede de amizades\n");
        printf("10. Listar amigos de um usu�rio\n");
        printf("11. Sugest�o de amizade por g�nero\n");
        printf("12. Sugest�o de filmes\n");
        printf("13. Busca DFS\n");
        printf("14. Busca BFS\n");
        printf("0. Sair\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);
        getchar();

        if (opcao == 0) break;

        int id1, id2;
        char nome[NAME_SIZE];

        switch (opcao) {
            case 1: {
                printf("Usu�rios cadastrados:\n");
                Usuario* utemp = listaUsuarios;
                while (utemp != NULL) {
                    printf("[%d] %s (G�nero favorito: %s)\n", utemp->id, utemp->nome, utemp->generoFav);
                    utemp = utemp->proxUser;
                }
                break;
            }

            case 2: {
                int novoId = 0;
                Usuario* temp = listaUsuarios;
                while (temp != NULL) {
                    if (temp->id >= novoId) {
                        novoId = temp->id + 1;
                    }
                    temp = temp->proxUser;
                }

                printf("Novo ID gerado automaticamente: %d\n", novoId);
                printf("Digite nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;
                
                Usuario* novo = criarUsuario(novoId, nome);
                adicionarUsuario(&listaUsuarios, novo);
                
                if (novoId >= grafo.numVertices) grafo.numVertices = novoId + 1;
                salvarUsuarios(listaUsuarios, "usuarios.txt");
                printf("Usu�rio adicionado e salvo no arquivo.\n");
                break;
            }

            case 3: {
                printf("Digite ID do usu�rio para remover: ");
                scanf("%d", &id1);
                getchar();
                if (removerUsuario(&listaUsuarios, id1)) {
                    printf("Usu�rio removido.\n");
                    No* temp = grafo.adj[id1];
                    while (temp != NULL) {
                        removerAresta(&grafo, id1, temp->destino);
                        temp = grafo.adj[id1];
                    }
                    salvarUsuarios(listaUsuarios, "usuarios.txt");
                    salvarAmizades(&grafo, listaUsuarios, "amizades.txt");
                } else {
                    printf("Usu�rio n�o encontrado.\n");
                }
                break;
            }

            case 4: {
                criarFilmeMenu(&listaFilmes);
                salvarFilmes(listaFilmes, "filmes.txt");
                break;
            }

            case 5: {
                printf("Digite ID do filme para remover: ");
                scanf("%d", &id1);
                getchar();
                if (removerFilme(&listaFilmes, id1)) {
                    printf("Filme removido.\n");
                    salvarFilmes(listaFilmes, "filmes.txt");
                } else {
                    printf("Filme n�o encontrado.\n");
                }
                break;
            }

            case 6: {
                printf("Filmes dispon�veis:\n");
                Filme* ftemp = listaFilmes;
                while (ftemp != NULL) {
                    printf("[%d] %s (G�nero: %s)\n", ftemp->id, ftemp->nome, ftemp->genero);
                    ftemp = ftemp->proxFilme;
                }
                break;
            }

            case 7: {
                printf("Digite ID do usu�rio 1: ");
                scanf("%d", &id1);
                getchar();
                printf("Digite ID do usu�rio 2: ");
                scanf("%d", &id2);
                getchar();

                if (id1 < 0 || id1 >= grafo.numVertices || id2 < 0 || id2 >= grafo.numVertices) {
                    printf("IDs inv�lidos.\n");
                    break;
                }

                Usuario* u1 = buscarUsuarioPorID(listaUsuarios, id1);
                Usuario* u2 = buscarUsuarioPorID(listaUsuarios, id2);

                if (!u1 || !u2) {
                    printf("Um ou ambos os usu�rios n�o existem.\n");
                    break;
                }

                if (id1 == id2) {
                    printf("N�o � poss�vel criar amizade consigo mesmo.\n");
                    break;
                }
                
                if(strcmp(u1->generoFav, u2->generoFav) != 0) {
                    printf("N�o � poss�vel gerar uma amizade entre %s e %s pois n�o gostam da mesma categoria de filme!\n", u1->nome, u2->nome);
                    break;
                }

                if (existeAresta(grafo.adj[id1], id2)) {
                    printf("Amizade j� existe.\n");
                } else {
                    adicionarAresta(&grafo, id1, id2);
                    printf("Amizade criada entre %s e %s.\n", u1->nome, u2->nome);
                    salvarAmizades(&grafo, listaUsuarios, "amizades.txt");
                }
                break;
            }

            case 8: {
                listarRedeAmizades(&grafo, listaUsuarios);
                break;
            }

            case 9: {
                exportarRedeAmizades(&grafo, listaUsuarios, "rede_amizades.txt");
                break;
            }

            case 10: {
                printf("Digite ID do usu�rio: ");
                scanf("%d", &id1);
                getchar();
                listarAmigos(&grafo, listaUsuarios, id1);
                break;
            }

            case 11: {
                printf("Digite ID do usu�rio: ");
                scanf("%d", &id1);
                getchar();
                sugerirAmizadePorGenero(&grafo, listaUsuarios, id1);
                break;
            }

            case 12: {
                printf("Digite ID do usu�rio: ");
                scanf("%d", &id1);
                getchar();
                sugerirFilmes(listaFilmes, listaUsuarios, id1);
                break;
            }

            case 13: {
                printf("Digite ID do usu�rio para iniciar DFS: ");
                scanf("%d", &id1);
                getchar();
                dfs(&grafo, id1, listaUsuarios);
                break;
            }

            case 14: {
                printf("Digite ID do usu�rio para iniciar BFS: ");
                scanf("%d", &id1);
                getchar();
                bfs(&grafo, id1, listaUsuarios);
                break;
            }

            default:
                printf("Op��o inv�lida.\n");
        }
        printf("Pressione ENTER para continuar..."); 
        getchar();
        limparTela();
    }
    
    salvarAmizades(&grafo, listaUsuarios, "amizades.txt");
}

// --------------------- FUN��O PRINCIPAL ---------------------
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    setlocale(LC_ALL, "");
    menu();
    return 0;
}
