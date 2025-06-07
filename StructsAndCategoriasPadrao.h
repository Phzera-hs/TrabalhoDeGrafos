#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>

const char* CATEGORIAS_FILMES[6] = {
    "Acao",
    "Comedia",
    "Drama",
    "Aventura",
    "Terror",
    "Romance"
};

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

