//
//  auxiliar_functions.h
//  AStar_algorithm
//

#ifndef main_functions_h
#define main_functions_h

#define heuristic modified_Euclidean_distance
#define resize M_PI * 6371000 / 180 // With estimated Earth's radius.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <time.h>


typedef char bool;
enum {false, true};
typedef struct {float g; unsigned parent;} AStarPath;
typedef struct QueueElementstruct {unsigned v; struct QueueElementstruct *seg;} QueueElement;
typedef QueueElement * PriorityQueue;
typedef struct {float f; bool IsOpen;} AStarControlData;
typedef struct {unsigned long id; char *name; double latitude, longitude; unsigned short nsucc; unsigned long *successors;} node;
bool AStar(node *, AStarPath *, unsigned, unsigned, unsigned);


void ExitError(const char *miss, int errcode) {
    fprintf(stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

int binary_search(long int idx, node nodes[], unsigned long total_nodes) {
    /* The 'id' is compared with the one in the middle of the array, so we discard one of the halfs, and so on. */
    int first = 0, last = total_nodes-1, middle = 0.5 * (first + last);;

    while (first <= last) {
        if (nodes[middle].id == idx)
            return middle;
        
        if (nodes[middle].id < idx)
            first = middle + 1;
        
        else
            last = middle - 1;
        
        middle = 0.5 * (first + last);
    }
    return -1;
}

float modified_Euclidean_distance(node *nodes, unsigned previous_vertex, unsigned new_vertex) {
    /* Modified Euclidean distance, which works in ths latitudes and longitudes space. */
    double x = (nodes[previous_vertex].longitude - nodes[new_vertex].longitude), y = nodes[previous_vertex].latitude - nodes[new_vertex].latitude;
    return resize * sqrt(x*x + y*y);
}

float cost(node *nodes, unsigned previous_vertex, unsigned new_vertex) {
    /* Euclidean distance. */
    double x = (nodes[previous_vertex].longitude - nodes[new_vertex].longitude) * cos(0.5 * M_PI / 180 * (nodes[previous_vertex].latitude + nodes[new_vertex].latitude)), y = nodes[previous_vertex].latitude - nodes[new_vertex].latitude;
    return resize * sqrt(x*x + y*y);
}

bool IsEmpty(PriorityQueue Pq) {
    return ((bool) (Pq == NULL));
}

unsigned extract_min(PriorityQueue *Pq) {
    /* Takes the first element (minumum) and displaces the list to the right. */
    PriorityQueue first = *Pq;
    unsigned v = first->v;
    *Pq = (*Pq)->seg;
    free(first);
    return v;
}

bool add_with_priority(unsigned v, PriorityQueue *Pq, AStarControlData * Q) {
    register QueueElement * q;
    QueueElement *aux = (QueueElement *) malloc(sizeof(QueueElement));
    
    if (aux == NULL)
        return false;
    
    aux->v = v;
    float costv = Q[v].f;
    Q[v].IsOpen = true;
    
    if (*Pq == NULL || !(costv > Q[(*Pq)->v].f)) {
        aux->seg = *Pq;
        *Pq = aux;
        return true;
    }
    for (q = *Pq; q->seg && Q[q->seg->v].f < costv; q = q->seg);
    aux->seg = q->seg;
    q->seg = aux;
    return true;
}

void requeue_with_priority(unsigned v, PriorityQueue *Pq, AStarControlData * Q){
    if ((*Pq)->v == v)
        return;
    
    register QueueElement * prepv;
    for (prepv = *Pq; prepv->seg->v != v; prepv = prepv->seg);
    QueueElement * pv = prepv->seg;
    prepv->seg = pv->seg;
    free(pv);
    add_with_priority(v, Pq, Q);
}

bool AStar(node *nodes, AStarPath *PathData, unsigned GrOrder, unsigned node_start, unsigned node_goal) {
    /* Here the main algorithm is performed. By using two different queues, the minimum distance between nodes is obtained. */
    register unsigned i;
    PriorityQueue Open = NULL;
    AStarControlData *Q;
    
    if ((Q = (AStarControlData *) malloc(GrOrder * sizeof(AStarControlData))) == NULL)
        ExitError("when allocating memory for the AStar Control Data vector", 73);
    
    for (i = 0; i < GrOrder; i++) {
        /* All nodes distances set to infinit and no-one is opened yet. */
        PathData[i].g = ULONG_MAX;
        Q[i].IsOpen = false;
    }
    PathData[node_start].g = 0.0;
    PathData[node_start].parent = UINT_MAX;
    Q[node_start].f = heuristic(nodes, node_start, node_goal);
    
    if (!add_with_priority(node_start, &Open, Q))
        return -1;
    
    while (!IsEmpty(Open)) {
        unsigned node_curr;
        
        if ((node_curr = extract_min(&Open)) == node_goal) {
            free(Q);
            return true;
        }
        
        for (i = 0; i < nodes[node_curr].nsucc; i++) {
            unsigned node_succ = nodes[node_curr].successors[i];
            float g_curr_node_succ = PathData[node_curr].g + cost(nodes, node_curr, node_succ);
            
            if (g_curr_node_succ < PathData[node_succ].g) {
                PathData[node_succ].parent = node_curr;
                Q[node_succ].f = g_curr_node_succ + ((PathData[node_succ].g == ULONG_MAX) ? heuristic(nodes, node_succ, node_goal) : (Q[node_succ].f - PathData[node_succ].g));
                PathData[node_succ].g = g_curr_node_succ;
                
                if (!Q[node_succ].IsOpen) {
                    if (!add_with_priority(node_succ, &Open, Q))
                        return -1;
                }
                else
                    requeue_with_priority(node_succ, &Open, Q);
            }
        }
        Q[node_curr].IsOpen = false;
    }
    return false;
}

#endif /* main_functions_h */
