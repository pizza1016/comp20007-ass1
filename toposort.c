/* COMP20007 Design of Algorithms
   2016 Semester 1
   Assignment 1 - Topological sorting
   Name: Aaron Lim Zi Yao
   Student number: 709872
   Student email: zil3@student.unimelb.edu.au
   Submission date: 8 April 2016
   Due date: 8 April 2016

   This file contains the topological sorting functions.

   The basic layout of this file including function and parameter names was
   provided. The rest of the code is my own work. Additional functions not
   previously included are labelled and placed at the end of this file.
*/

/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "toposort.h"

/* Representations for temporary and permanent marks in the array of marks: */
#define NO_MARK        0
#define TEMP_MARK      1
#define PERM_MARK      2

/* Numerical constants: */
#define ZERO           0

/* Error messages: */
#define ERR_NO_SOURCES "Error: Graph does not have any sources - is it cyclic?\n"
#define ERR_LEFTOVERS  "Error: Graph has leftover edges - has at least one cycle.\n"
#define ERR_CYCLIC     "Error: Input is not a directed acyclic graph.\n"

/* An array of marks and its corresponding size. */
typedef struct {
    int *marks_array;
    int marks_size;
    int pmarked_count; /* number of vertices permanently visited. */
} marks_t;

/*****************************************************************************/

/* Function prototypes for functions written in another file: */
void    *checked_malloc(size_t size);  /* from graph.c */

/* Function prototypes for additional functions in this file: */
List     dfs_visit(Vertex vertex, Graph graph, List toposorted,
                   marks_t *marks);
marks_t *new_marks(int size);
void     free_marks(marks_t *marks);
List     find_sources(Graph graph);
void     del_edge(Graph graph, int v1, int v2);

/*****************************************************************************/

/* Returns a list of topologically sorted vertices using the DFS method */
List dfs_sort(Graph graph) {
    List toposorted = NULL;  /* the sorted list of vertices */
    int i;

    /* Start an array of size 'order' to store temporary and permanent marks.
    */
    marks_t *marks = new_marks(graph->order);
    
    /* Until all vertices have been visited... */
    while (marks->pmarked_count < graph->order) {
        /* ...brute-force search for a vertex without a mark... */
        for (i = 0; marks->marks_array[i]; i++);
        
        /* ...and pay it a visit. */
        toposorted = dfs_visit(&(graph->vertices[i]), graph, toposorted,
                               marks);
    }
    
    free_marks(marks);
    return toposorted;
}

/*****************************************************************************/

/* Returns a list of topologically sorted vertices using the Kahn method */
List kahn_sort(Graph graph) {
    List toposorted = NULL; /* the sorted list of vertices */
    /* Look for the original sources. */
    List sources = find_sources(graph);
    Vertex vertex, target_vertex;
    List entry, next;
    
    /* Something is obviously wrong if there's nowhere to start. */
    if (sources == NULL) {
        fprintf(stderr, ERR_NO_SOURCES);
        exit(EXIT_FAILURE);
    }
    
    /* Until we starve the world of its water... */
    while (sources) {
        /* Pick a source (first one is easiest) and add it to the end of the
           sorted list since we navigate through the sources first. */
        vertex = (Vertex)pop(&sources);
        insert(vertex, &toposorted);
        
        /* For each outgoing edge from the vertex being processed... */
        for (entry = vertex->out; entry != NULL; entry = next) {
            /* Because I will be directly modifying the vertex's 'out' list
               while I'm going through said list, I need to remember the
               pointers that I need to reuse after the modification. */
            next = entry->next;
            target_vertex = (Vertex)entry->data;
            
            /* Delete the edge pointing from the current vertex to the vertex
               in the list (target). */
            del_edge(graph, vertex->id, target_vertex->id);
            
            /* If the target vertex does not have any more incoming edges, then
               it is a new source. */
            if (target_vertex->in == NULL) {
                insert(target_vertex, &sources);
            }
        }
    }
    
    /* All edges must have been accounted for if it is a directed acyclic
       graph. */
    assert(graph->size >= ZERO);
    if (graph->size != ZERO) {
        fprintf(stderr, ERR_LEFTOVERS);
        exit(EXIT_FAILURE);
    }
    
    return toposorted;
}

/*****************************************************************************/

/* Uses graph to verify vertices are topologically sorted */
bool verify(Graph graph, List vertices) {
    List entry, in_vertex;
    marks_t *marks;
    
    /* If first entry is not a source, then not sorted. */
    if (((Vertex)vertices->data)->in != NULL) {
        return false;
    }
    
    /* Reusing the marks_t struct to mark vertices that we've already seen. */
    marks = new_marks(graph->order);
    marks->marks_array[((Vertex)vertices->data)->id] = PERM_MARK;
    
    /* Go through the list of vertices. */
    for (entry = vertices->next; entry != NULL; entry = entry->next) {

        /* For every vertex (in_vertex) with an incoming edge to the current
           vertex (entry->data): */
        for (in_vertex = ((Vertex)entry->data)->in; in_vertex != NULL;
             in_vertex = in_vertex->next) {

            /* If there exist an in_vertex that we have not previously seen,
               then the list is not sorted. */
            if (marks->marks_array[((Vertex)in_vertex->data)->id] != PERM_MARK) {
                free_marks(marks);
                return false;
            }
        }
        
        /* Hooray! We have visited the vertex. Now to do it again...sigh.... */
        marks->marks_array[((Vertex)entry->data)->id] = PERM_MARK;
    }
    
    /* If all conditions are satisfied, then vertices are sorted. */
    free_marks(marks);
    return true;
}

/*****************************************************************************/

/* Additional functions: */

/* Visit a node and perform sorting operations. */
List dfs_visit(Vertex vertex, Graph graph, List toposorted, marks_t *marks) {
    List pointer;
    
    assert(vertex->id < marks->marks_size);
    
    /* If the vertex being visited has a temporary mark, we must have gone in a
       circle and so a sorted list doesn't exist (assuming that it is a
       directed graph in the first place. */
    if (marks->marks_array[vertex->id] == TEMP_MARK) {
        fprintf(stderr, ERR_CYCLIC);
        exit(EXIT_FAILURE);
    }
    
    /* If the vertex hasn't been visited before... */
    else if (marks->marks_array[vertex->id] == NO_MARK) {
        /* ...temp-visit it and visit every vertex that this vertex has an
           outgoing edge to. */
        marks->marks_array[vertex->id] = TEMP_MARK;
        for (pointer = vertex->out; pointer; pointer = pointer->next) {
            toposorted = dfs_visit(pointer->data, graph, toposorted, marks);
        }
        
        /* Once we finish visiting mark the graph as permanently visited and
           add it to the sorted list. */
        marks->marks_array[vertex->id] = PERM_MARK;
        marks->pmarked_count++;
        toposorted = push(toposorted, vertex);
    }
    
    return toposorted;
}

/*****************************************************************************/

/* mallocs a new marks_t struct with array size 'size', initialise it, and
   returns it's address. */
marks_t *new_marks(int size) {
    int i;
    marks_t *marks;
    marks = (marks_t*)checked_malloc(sizeof(marks_t));
    marks->marks_array = (int *)checked_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        marks->marks_array[i] = NO_MARK;
    }
    marks->marks_size = size;
    marks->pmarked_count = ZERO;
    return marks;
}

/*****************************************************************************/

/* Frees a marks_t struct. */
void free_marks(marks_t *marks) {
    free(marks->marks_array);
    free(marks);
}

/*****************************************************************************/

/* Returns a list of sources in the graph. */
List find_sources(Graph graph) {
    List sources = NULL;
    int i;

    for (i = 0; i < graph->order; i++) {
        /* A vertex is a source when it has no incoming edges. */
        if (graph->vertices[i].in == NULL) {
            sources = push(sources, &(graph->vertices[i]));
        }
    }

    return sources;
}

/*****************************************************************************/

/* Removes the edge going from the vertex with id 'v1' to the vertex with id
   'v2' from the graph. */
void del_edge(Graph graph, int v1, int v2) {
    /* To do that we need to remove the vertex v2 from the out list of v1, and
       the vertex v1 from the in list of v2. */
    assert(del(ptr_eq, &(graph->vertices[v2]), &(graph->vertices[v1].out)));
    assert(del(ptr_eq, &(graph->vertices[v1]), &(graph->vertices[v2].in)));
    graph->size--;
    return;
}
