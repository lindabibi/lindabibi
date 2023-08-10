#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h> 

#define MAX_NODES 11647
#define MAX_EPOCHS 15000 
#define ES_STEPS 50

// Définir la constante PI
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Structure pour stocker les données d'une ligne
typedef struct
{
    char **values;
} Data;

// Structure pour stocker le jeu de données
typedef struct
{
    char **column_names;
    int num_rows;
    int num_columns;
    Data **data;
} Dataset;

struct Node
{
    Data *row; // Pour stocker la ligne de données associée au nœud
};

struct Graph
{
    int numNodes;
    struct Node *nodes[MAX_NODES];
    int edges[MAX_NODES][MAX_NODES];
};

// Définir la structure pour un nœud dans la liste chaînée des index
typedef struct NodeIndexListNode
{
    int index;
    struct NodeIndexListNode *next;
} NodeIndexListNode;

// Définir la structure pour une liste chaînée de nœuds
typedef struct CommunityList
{
    NodeIndexListNode *head;
} CommunityList;


// Fonction pour lire le fichier CSV
Dataset *read_csv(const char *file_path);
void initGraph(struct Graph *graph, int numNodes);
void addNode(struct Graph *graph, int node);
void addEdge(struct Graph *graph, int node1, int node2);
void ajouter_aretes_similaires(struct Graph *graph, Dataset *dataset);
void associerLignes(struct Graph *graph, Dataset *dataset);
int obtenir_nombre_de_noeuds(struct Graph *graph);
int obtenir_nombre_daretes(struct Graph *graph);
int degree(struct Graph *graph, int node);
double modularity(struct Graph *graph, bool *community, int numNodes);
void find_communities_from_dataset(Dataset *dataset);
int **create_labels(int numNodes, int numClasses, int *colors);
void create_club_labels(int numNodes, int *colors);