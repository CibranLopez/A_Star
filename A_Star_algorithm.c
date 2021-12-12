//
//  main.c
//  AStar_algorithm
//


#include "auxiliar_functions.h"

#define input_file_name "Spain_binary_file.bin"
#define output_file_name "Results.dat"


/*
    Here the AStar's algorithm is implemented. It goes iteratively to the nearest neighbour, and if the distance through the node plus the expeted one to the goal is smaller than the other options, it saves it as a new edge of the sapanning tree. Once it finishes one brach, it goes through the previous non-expanded node.
    All needed functions, libraries and constants are defined in 'auxiliar_functions.h'.
*/

int main() {
    clock_t start, initialization, process;
    start = clock();
    register unsigned int i;
    unsigned long N_nodes, N_total_successors, N_total_characters, *allsuccessors;
    char *allnames;
    
    FILE *fin;
    
    if ((fin = fopen(input_file_name, "r")) == NULL)
        ExitError("The data file does not exist or cannot be opened", 1);
    
    /* Initial needed parameters. */
    if (fread(&N_nodes, sizeof(unsigned long), 1, fin) + fread(&N_total_successors, sizeof(unsigned long), 1, fin) + fread(&N_total_characters, sizeof(unsigned long), 1, fin) != 3)
        ExitError("When reading the header of the binary data file", 2);
        
    /* Reserving memory for the nodes, successsors and names. */
    node *nodes;
    if ((nodes = (node *) malloc(N_nodes * sizeof(node))) == NULL)
        ExitError("When allocating memory for the nodes vector", 3);
    
    if ((allsuccessors = (unsigned long *) malloc(N_total_successors * sizeof(unsigned long))) == NULL)
        ExitError("When allocating memory for the edges vector", 4);
    
    if ((allnames = (char *) malloc(N_total_characters * sizeof(char))) == NULL)
        ExitError("When allocating memory for the edges vector", 4);
    
    /* Reading all data from file. */
    if (fread(nodes, sizeof(node), N_nodes, fin) != N_nodes)
        ExitError("When reading nodes from the binary data file", 5);
    
    if (fread(allsuccessors, sizeof(unsigned long), N_total_successors, fin) != N_total_successors)
        ExitError("When reading sucessors from the binary data file", 6);
    
    if (fread(allnames, sizeof(char), N_total_characters, fin) != N_total_characters)
        ExitError("When reading names from the binary data file", 6);
    fclose(fin);
    
    /* Setting pointers to successors. */
    for (i = 0; i < N_nodes; i++)
        if (nodes[i].nsucc) {
            nodes[i].successors = allsuccessors;
            allsuccessors += nodes[i].nsucc;
        }
    
    /* Setting pointers to names. */
    for (i = 0; i < N_nodes; i++)
        asprintf(&nodes[i].name, "%s", strsep(&allnames, "|"));
    
    AStarPath *PathData;
    if ((PathData = (AStarPath *) malloc(N_nodes * sizeof(AStarPath))) == NULL)
        ExitError("Error in memory allocation of the path data.", 3);
    
    /* Here starting and goal nodes are defined. It is both possible to define them by their 'id' and their coordinates. */
    unsigned node_start_id = 240949599U, node_goal_id = 195977239U;
    
    unsigned node_start = binary_search(node_start_id, nodes, N_nodes);
    unsigned node_goal = binary_search(node_goal_id, nodes, N_nodes);
        
    printf("Starting point: %lf, %lf\n", nodes[node_start].latitude, nodes[node_start].longitude);
    printf("Ending point: %lf, %lf\n", nodes[node_goal].latitude, nodes[node_goal].longitude);
    
    initialization = clock();
    
    /* Perform of the main algorithm. */
    bool r = AStar(nodes, PathData, N_nodes, node_start, node_goal);
    
    process = clock();
    
    if (r == -1)
        ExitError("In allocating memory for the OPEN list in AStar", 7);
    else
        if (!r)
            ExitError("No solution found in AStar", 8);
    
    register unsigned v = node_goal, pv = PathData[v].parent, ppv;
    PathData[node_goal].parent = UINT_MAX;
    
    while (v != node_start) {
        /* Reconstructing the path backwards. */
        ppv = PathData[pv].parent;
        PathData[pv].parent = v;
        v = pv;
        pv = ppv;
    }
    
    /* Saving the resulting ids, distances and names. */
    FILE *resulting_data;
    if ((resulting_data = fopen(output_file_name, "w")) == NULL)
        ExitError("The output binary data file cannot be opened", 8);
   
    fprintf(resulting_data, "  Node  id  | Distance | Name\n  --------  | -------- | ---- \n %10.lu |      0   | %s\n", nodes[node_start].id, nodes[node_start].name);
        
    for (v = PathData[node_start].parent; v != UINT_MAX; v = PathData[v].parent)
        fprintf(resulting_data, " %10.lu | %8.2f | %s\n", nodes[v].id, PathData[v].g, nodes[v].name);
    
    /* Printing the elapsed times during execution. */
    printf("Inizialization: %.2f s; Process: %.2f s; Saving: %.2f s\n", (float) (initialization - start) / CLOCKS_PER_SEC, (float) (process - initialization) / CLOCKS_PER_SEC, (float) (clock() - start) / CLOCKS_PER_SEC);
}
