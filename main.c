#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <float.h>


int main()
{
    // Importation du jeu de données
    char *file_path = "./dataset_fichier.csv";
    Dataset *dataset = read_csv(file_path);
    // Affichage des deux premières lignes du jeu de données
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < dataset->num_columns; j++)
        {
            printf("%s: %s\n", dataset->column_names[j], dataset->data[i]->values[j]);
        }
        printf("\n");
    }
    // Informations sur le jeu de données
    printf("Number of rows: %d\n", dataset->num_rows);
    printf("Number of columns: %d\n\n", dataset->num_columns);
    int numNodes = dataset->num_rows;
    int numCols = dataset->num_columns;
    struct Graph graph;
    initGraph(&graph, numNodes);
    // Ajouter des nœuds au graphe
    for (int i = 0; i < numNodes; i++)
    {
        addNode(&graph, i);
    }
    // Ajouter des arêtes entre les nœuds ayant des valeurs similaires pour la colonne `bt_status`
    ajouter_aretes_similaires(&graph, dataset);
    // Associer chaque nœud à la ligne correspondante dans le jeu de données
    associerLignes(&graph, dataset);
    // Accéder aux valeurs du nœud avec l'index i = 23
    int index_noeud = 23;
    Data *ligne = graph.nodes[index_noeud]->row;
    for (int j = 0; j < dataset->num_columns; j++)
    {
        printf("%s: %s\n", dataset->column_names[j], ligne->values[j]);
    }
    printf("\n");
    // Obtenir le nombre de nœuds et d'arêtes dans le graphe
    int nombreNoeuds = obtenir_nombre_de_noeuds(&graph);
    int nombreAretes = obtenir_nombre_daretes(&graph);
    printf("Nombre de nœuds : %d\n", nombreNoeuds);
    printf("Nombre d'arêtes : %d\n", nombreAretes);
    bool *community = (bool *)malloc(numNodes * sizeof(bool));
    find_communities_from_dataset(dataset);
    // Colorer les nœuds en fonction des valeurs de bt_status
    int *colors = (int *)malloc(numNodes * sizeof(int));
    // Dictionnaire pour stocker les couleurs associées à chaque valeur unique de bt_status
    int *btStatusColors = (int *)malloc(numCols * sizeof(int));
    int numClasses = 0; // Compteur pour le nombre de classes (valeurs uniques de bt_status)
    for (int i = 0; i < numNodes; i++)
    {
        // Récupérer la valeur de bt_status à partir du jeu de données pour ce nœud
        int btStatusValue = atoi(dataset->data[i]->values[0]);
        // Vérifier si cette valeur de bt_status est déjà présente dans le dictionnaire
        int color = -1;
        for (int j = 0; j < numClasses; j++)
        {
            if (btStatusColors[j] == btStatusValue)
            {
                color = j;
                break;
            }
        }
        // Si la valeur de bt_status n'est pas encore présente dans le dictionnaire, l'ajouter avec une nouvelle couleur
        if (color == -1)
        {
            color = numClasses;
            btStatusColors[numClasses] = btStatusValue;
            numClasses++;
        }
        // Attribuer la couleur au nœud
        colors[i] = color;
    }
    // Afficher les couleurs des nœuds
    for (int i = 0; i < numNodes; i++)
    {
        printf("Node %d: Color %d (bt_status = %d)\n", i, colors[i], btStatusColors[colors[i]]);
    }
    int **labels = create_labels(numNodes, numClasses, colors);
    // Afficher les labels
    for (int i = 0; i < numNodes; i++)
    {
        printf("Node %d: ", i);
        for (int j = 0; j < numClasses; j++)
        {
            printf("%d ", labels[i][j]);
        }
        printf("\n");
    }
    // Créer le dictionnaire des labels de clubs
    create_club_labels(numNodes, colors);
    int adjacency_matrix[MAX_NODES][MAX_NODES];

    // Informations sur le jeu de données
    printf("Number of rows: %d\n", dataset->num_rows);
    printf("Number of columns: %d\n\n", dataset->num_columns);
    printf("Number of Class: %d\n\n", numClasses);


    for (int i = 0; i < numNodes; i++)
    {
        free(labels[i]);
    }
    free(labels);
    free(btStatusColors);

    return 0;
}
