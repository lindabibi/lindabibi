#include "main.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// Fonction pour créer une nouvelle ligne de données
Data *create_data(int num_columns)
{
    Data *new_data = (Data *)malloc(sizeof(Data));
    if (new_data == NULL)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    new_data->values = (char **)malloc(num_columns * sizeof(char *));
    if (new_data->values == NULL)
    {
        perror("Memory allocation error");
        free(new_data);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_columns; i++)
    {
        new_data->values[i] = NULL;
    }

    return new_data;
}

// Fonction pour libérer la mémoire allouée pour une ligne de données
void free_data(Data *data, int num_columns)
{
    for (int i = 0; i < num_columns; i++)
    {
        free(data->values[i]);
    }
    free(data->values);
    free(data);
}

// Fonction pour libérer la mémoire allouée pour le jeu de données
void free_dataset(Dataset *dataset)
{
    for (int i = 0; i < dataset->num_rows; i++)
    {
        free_data(dataset->data[i], dataset->num_columns);
    }
    free(dataset->data);

    for (int i = 0; i < dataset->num_columns; i++)
    {
        free(dataset->column_names[i]);
    }
    free(dataset->column_names);

    free(dataset);
}

// Fonction pour lire le fichier CSV
Dataset *read_csv(const char *file_path)
{
    Dataset *dataset = (Dataset *)malloc(sizeof(Dataset));
    if (dataset == NULL)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("File open error");
        free(dataset);
        exit(EXIT_FAILURE);
    }

    char line[4096];
    char *token;

    // Lire la première ligne pour obtenir les noms de colonnes
    if (fgets(line, sizeof(line), file) == NULL)
    {
        perror("File read error");
        free(dataset);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int num_columns = 0;
    dataset->column_names = NULL;

    token = strtok(line, ",");
    while (token != NULL)
    {
        dataset->column_names = (char **)realloc(dataset->column_names, (num_columns + 1) * sizeof(char *));
        if (dataset->column_names == NULL)
        {
            perror("Memory allocation error");
            free(dataset);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        dataset->column_names[num_columns] = strdup(token);
        if (dataset->column_names[num_columns] == NULL)
        {
            perror("Memory allocation error");
            free(dataset);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        num_columns++;
        token = strtok(NULL, ",");
    }

    // Lire le reste des lignes pour obtenir les données
    dataset->num_rows = 0;
    dataset->data = NULL;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        dataset->data = (Data **)realloc(dataset->data, (dataset->num_rows + 1) * sizeof(Data *));

        Data *new_data = create_data(num_columns);

        token = strtok(line, ",");
        int column = 0;
        while (token != NULL)
        {
            new_data->values[column] = strdup(token);
            column++;
            token = strtok(NULL, ",");
        }

        dataset->data[dataset->num_rows] = new_data;
        dataset->num_rows++;
    }

    dataset->num_columns = num_columns;

    fclose(file);
    return dataset;
}

void initGraph(struct Graph *graph, int numNodes)
{
    graph->numNodes = numNodes;
    for (int i = 0; i < numNodes; i++)
    {
        graph->nodes[i] = 0;
        for (int j = 0; j < numNodes; j++)
        {
            graph->edges[i][j] = 0;
        }
    }
}

void addNode(struct Graph *graph, int node)
{
    if (node >= 0 && node < graph->numNodes)
    {
        graph->nodes[node] = (struct Node *)malloc(sizeof(struct Node));
        ;
    }
}

void addEdge(struct Graph *graph, int node1, int node2)
{
    if (node1 >= 0 && node1 < graph->numNodes && node2 >= 0 && node2 < graph->numNodes)
    {
        graph->edges[node1][node2] = 1;
        graph->edges[node2][node1] = 1;
    }
}

// Fonction pour ajouter une arête entre deux nœuds si les deux lignes ont une valeur similaire pour la colonne `bt_status`
void ajouter_aretes_similaires(struct Graph *graph, Dataset *dataset)
{
    for (int i = 0; i < dataset->num_rows; i++)
    {
        for (int j = i + 1; j < dataset->num_rows; j++)
        {
            if (strcmp(dataset->data[i]->values[0], dataset->data[j]->values[0]) == 0)
            {
                addEdge(graph, i, j);
            }
        }
    }
}

// Associer chaque nœud à la ligne correspondante dans le jeu de données
void associerLignes(struct Graph *graph, Dataset *dataset)
{
    for (int i = 0; i < graph->numNodes; i++)
    {
        graph->nodes[i]->row = dataset->data[i];
    }
}

// Fonction pour obtenir le nombre de nœuds dans le graphe
int obtenir_nombre_de_noeuds(struct Graph *graph)
{
    return graph->numNodes;
}

// Fonction pour obtenir le nombre d'arêtes dans le graphe
int obtenir_nombre_daretes(struct Graph *graph)
{
    int nombreAretes = 0;
    for (int i = 0; i < graph->numNodes; i++)
    {
        for (int j = 0; j < graph->numNodes; j++)
        {
            nombreAretes += graph->edges[i][j];
        }
    }
    // Nous comptons chaque arête deux fois (une fois pour chaque nœud), donc nous devons diviser par 2
    return nombreAretes / 2;
}

// Fonction pour calculer le degré d'un nœud
int degree(struct Graph *graph, int node)
{
    int degree = 0;
    for (int i = 0; i < graph->numNodes; i++)
    {
        degree += graph->edges[node][i];
    }
    return degree;
}

// Fonction pour calculer la modularité d'une communauté donnée
double modularity(struct Graph *graph, bool *community, int numNodes)
{
    double modularity = 0.0;
    int total_edges = graph->numNodes * (graph->numNodes - 1) / 2; // Nombre total d'arêtes dans le graphe complet

    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            if (community[i] && community[j])
            {
                if (graph->edges[i][j])
                {
                    modularity += 1.0; // Augmenter la modularité si les nœuds sont connectés
                }
            }
            else
            {
                if (!graph->edges[i][j])
                {
                    modularity += 1.0; // Augmenter la modularité si les nœuds ne sont pas connectés
                }
            }
        }
    }

    modularity /= total_edges; // Diviser par le nombre total d'arêtes pour normaliser

    return modularity;
}

// Fonction pour trouver les communautés à partir du jeu de données
void find_communities_from_dataset(Dataset *dataset)
{
    int numNodes = dataset->num_rows;

    bool *community = (bool *)malloc(numNodes * sizeof(bool));

    // Initialiser les communautés à false
    for (int i = 0; i < numNodes; i++)
    {
        community[i] = false;
    }

    int numCommunities = 0;

    // Tableau pour stocker les listes des nœuds dans chaque communauté
    CommunityList *communityLists = (CommunityList *)malloc(numNodes * sizeof(CommunityList));
    for (int i = 0; i < numNodes; i++)
    {
        communityLists[i].head = NULL;
    }

    // Parcourir le jeu de données pour regrouper les nœuds ayant le même bt_status
    for (int i = 0; i < numNodes; i++)
    {
        if (!community[i])
        {
            int currentStatus = atoi(dataset->data[i]->values[0]);

            // Regrouper les nœuds ayant le même bt_status dans une communauté
            for (int j = i; j < numNodes; j++)
            {
                if (currentStatus == atoi(dataset->data[j]->values[0]))
                {
                    community[j] = true;

                    // Ajouter l'index du nœud à la liste de la communauté correspondante
                    NodeIndexListNode *newNode = (NodeIndexListNode *)malloc(sizeof(NodeIndexListNode));
                    newNode->index = j;
                    newNode->next = communityLists[numCommunities].head;
                    communityLists[numCommunities].head = newNode;
                }
            }

            // Une communauté trouvée pour un certain bt_status
            numCommunities++;
        }
    }

    // Afficher les nœuds appartenant aux communautés trouvées
    for (int i = 0; i < numCommunities; i++)
    {
        printf("Communaute %d: ", i + 1);
        NodeIndexListNode *current = communityLists[i].head;
        while (current != NULL)
        {
            printf("%d ", current->index);
            current = current->next;
        }
        printf("\n");
    }

    // Afficher le nombre total de communautés
    printf("Total number of communities: %d\n", numCommunities);

    // Libérer la mémoire allouée
    free(community);
    for (int i = 0; i < numNodes; i++)
    {
        NodeIndexListNode *current = communityLists[i].head;
        while (current != NULL)
        {
            NodeIndexListNode *next = current->next;
            free(current);
            current = next;
        }
    }
    free(communityLists);
}

// Fonction pour créer une matrice de labels à partir des couleurs des nœuds
int **create_labels(int numNodes, int numClasses, int *colors)
{
    int **labels = (int **)malloc(numNodes * sizeof(int *));
    for (int i = 0; i < numNodes; i++)
    {
        labels[i] = (int *)malloc(numClasses * sizeof(int));
        memset(labels[i], 0, numClasses * sizeof(int));
        labels[i][colors[i]] = 1;
    }
    return labels;
}

// Fonction pour créer le dictionnaire des labels de clubs
void create_club_labels(int numNodes, int *colors)
{
    printf("Club Labels:\n");
    for (int i = 0; i < numNodes; i++)
    {
        printf("%d: %d\n", i, colors[i]);
    }
}