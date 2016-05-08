/* COMP20007 Design of Algorithms
   2016 Semester 1
   Assignment 1 - Topological sorting
   Name: Aaron Lim Zi Yao
   Student number: 709872
   Student email: zil3@student.unimelb.edu.au
   Submission date: 8 April 2016
   Due date: 8 April 2016

   This file contains the functions for graph input and output.

   This file contains code that was provided as part of the specification.
   These will be clearly distinguished from the rest of the code. The rest of
   the said code is my own work, barring function and parameter names, and
   given predefined constants.
*/

/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graphio.h"

#define MAX_LINE_LEN    256    /* given constant: max chars in a line */

#define DELIMITERS      " \n"  /* whitespace characters separating 'words' */
#define NULLCHAR        '\0'   /* the null character */

#define ERR_READ_EDGES  "Error: Problem reading a line in list of edges."

/*****************************************************************************/

/* Function prototypes for functions written in another file: */
void *checked_malloc(size_t size);  /* from graph.c */

/*****************************************************************************/

/* Loads the graph from input */
Graph load_graph(char *input) {
    Graph graph;
    FILE *fp;
    int num_vertices, i, from, to;
    char line[MAX_LINE_LEN];
    char *pointer;

    fp = fopen(input, "r");
    assert(fp);

    /* Read the first line for the number of vertices. */
    fgets(line, MAX_LINE_LEN, fp);
    num_vertices = atoi(line);

    graph = new_graph(num_vertices);

    /* Assign the labels and id numbers for each vertex. */
    for (i = 0; i < num_vertices; i++) {
        fgets(line, MAX_LINE_LEN, fp);

        /* Remove the newline character at the end of the label and instead
           terminate the string there. */
        line[strlen(line) - 1] = NULLCHAR;

        graph->vertices[i].id = i;
        graph->vertices[i].label = (char*)checked_malloc(strlen(line) + 1);
        strcpy(graph->vertices[i].label, line);

        /* Initialise list of out and in edges. */
        graph->vertices[i].out = graph->vertices[i].in = NULL;
    }

    /* Read the remaining lines to get the edges. */
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        pointer = line;  

        /* Split the line to get two numbers representing the directed edge. */
        from = atoi(strtok(pointer, DELIMITERS));
        to = atoi(strtok(NULL, DELIMITERS));
        add_edge(graph, from, to);
    }

    /* Notify user if problem reading graph. */
    if (ferror(fp)) {
        fprintf(stderr, ERR_READ_EDGES);
        exit(EXIT_FAILURE);
    }
    else {
        /* At this point we must have reached the end of the file. */
        assert(feof(fp));
    }
    
    fclose(fp);

    return graph;
}

/*****************************************************************************/

/* Prints the graph */
void print_graph(char *output, Graph graph) {
    FILE *fp;
    int i;
    List pointer = NULL;

    fp = fopen(output, "w");
    assert(fp);

    fprintf(fp, "digraph {\n");
    for (i = 0; i < graph->order; i++) {
        if (graph->vertices[i].out) {
            /* First we print the vertex. */
            fprintf(fp, "  %s -> {", graph->vertices[i].label);

            /* Then we print every vertex its outgoing edges are pointing to. */
            pointer = graph->vertices[i].out;
            while (pointer) {
                print_vertex_label(fp, pointer->data);
                pointer = pointer->next;
            }

            fprintf(fp, "}\n");
        }

        /* If there aren't any outgoing edges, then just print vertex as is. */
        else {
            fprintf(fp, "  %s\n", graph->vertices[i].label);
        }
    }

    fprintf(fp, "}\n");

    fclose(fp);

    return;
}

/*****************************************************************************/

/* Prints the destination vertex label surrounded by spaces */
void print_vertex_label(FILE *file, void *vertex) {
    if (vertex) {
        fprintf(file, " %s ", ((Vertex)vertex)->label);
    }
}

/**************** EVERYTHING BELOW THIS LINE IS PROVIDED CODE ****************/

/* Prints the id of a vertex then a newline */
void print_vertex_id(FILE *file, void *vertex) {
    if (vertex)
        fprintf(file, "%d\n", ((Vertex)vertex)->id);
}

/* Returns a sequence of vertices read from file */
List load_vertex_sequence(FILE *file, Graph graph) {
    const char *err_duplicate = "Error: duplicate vertex %d %s\n";
    const char *err_order = "Error: graph order %d, loaded %d vertices\n";
    List list = NULL;
    int id;

    while(fscanf(file, "%d\n", &id) == 1) {
        assert(id >= 0);
        assert(id < graph->order);

        if (!insert_if(id_eq, graph->vertices + id, &list)) {
            fprintf(stderr, err_duplicate, id, graph->vertices[id].label);
            exit(EXIT_FAILURE);
        }
    }

    if (len(list) != graph->order) {
        fprintf(stderr, err_order, graph->order, len(list));
        exit(EXIT_FAILURE);
    }

    return list;
}
