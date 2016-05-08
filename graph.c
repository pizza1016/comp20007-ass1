/* COMP20007 Design of Algorithms
   2016 Semester 1
   Assignment 1 - Topological sorting
   Name: Aaron Lim Zi Yao
   Student number: 709872
   Student email: zil3@student.unimelb.edu.au
   Submission date: 8 April 2016
   Due date: 8 April 2016
   
   This file contains the functions for graph manipulation.

   The basic layout of this file including function name and arguments was
   provided. The rest of the code is my own work. Additional functions not
   previously included are labelled and placed at the end of this file.
*/

/*****************************************************************************/

#include <stdlib.h>
#include <assert.h>

#include "graph.h"

/* Numerical constants: */
#define ZERO 0

/*****************************************************************************/

/* Function prototypes for additional functions: */
void *checked_malloc(size_t size);

/*****************************************************************************/

/* Returns a pointer to a new graph with order vertices */
Graph new_graph(int order) {
    Graph graph = (Graph)checked_malloc(sizeof(*graph));

    graph->order = order;
    graph->size = ZERO;

    graph->vertices = (Vertex)checked_malloc(order*sizeof(*(graph->vertices)));
    
    return graph;
}

/*****************************************************************************/

/* Returns whether aim and vertex are pointing to the same location */
bool ptr_eq(void *aim, void *vertex) {
    if (aim == vertex) {
        return true;
    }
    return false;
}

/*****************************************************************************/

/* Returns whether aim and vertex have the same id */
bool id_eq(void *aim, void *vertex) {
    if (((Vertex)aim)->id == ((Vertex)vertex)->id) {
        return true;
    }
    return false;
}

/*****************************************************************************/

/* Add the edge from v1 to v2 to graph */
void add_edge(Graph graph, int v1, int v2) {
    Vertex out_data, in_data;

    /* out_data is a pointer to the target vertex at v2, stored in the source
       vertex at v1. in_data is a pointer to the source vertex at v1, stored in
       the target vertex at v2. */
    out_data = &(graph->vertices[v2]);
    in_data = &(graph->vertices[v1]);
    graph->vertices[v1].out = push(graph->vertices[v1].out, out_data);
    graph->vertices[v2].in = push(graph->vertices[v2].in, in_data);
    graph->size++;
    return;
}

/*****************************************************************************/

/* Free the memory allocated to graph */
void free_graph(Graph graph) {
    int i;

    /* Free every vertex in the graph, including the label and in/out lists. */
    for (i = 0; i < graph->order; i++) {
        free(graph->vertices[i].label);
        if (graph->vertices[i].out) {
            free_list(graph->vertices[i].out);
            graph->vertices[i].out = NULL;
        }
        if (graph->vertices[i].in) {
            free_list(graph->vertices[i].in);
            graph->vertices[i].in = NULL;
        }
    }

    free(graph->vertices);
    graph->vertices = NULL;
    free(graph);
    graph = NULL;
    return;
}

/*****************************************************************************/

/* Additional functions: */

/* Mallocs space and immediately check if spaced is correctly acquired. */
void *checked_malloc(size_t size) {
    void *pointer = malloc(size);
    if (!pointer) {
        fprintf(stderr, "Error: malloc-ing space of size %d bytes failed\n", (int)size);
        exit(EXIT_FAILURE);
    }
    return pointer;
}