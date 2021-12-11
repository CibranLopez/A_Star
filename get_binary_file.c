//
//  main.c
//  get_binary_file_Spain
//
//  Created by Cibrán on 16/10/2021.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define input_file_name "spain.csv"
#define output_file_name "Spain_binary_file.bin"


typedef enum {false, true} bool;
typedef struct {unsigned long id; char *name; double latitude, longitude; unsigned short nsucc; unsigned long *successors;} node;


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

void ExitError(const char *miss, int errcode) {
    fprintf(stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

int check_name(char *data_string, char *new_name) {
    /* Checks that a name is not in the string yet. */
    char *token = strsep(&data_string, ";");
    while (token != NULL) {
        if (!strcmp(token, new_name))
            return 0;
        token = strsep(&data_string, ";");
    }
    return 1;
}

void add_name(char *new_name, node nodes[], int position) {
    /* Adds the new name to the string whenever it is feasable. */
    if (strlen(new_name)) {
        if (strlen(nodes[position].name)) {
            if (check_name(nodes[position].name, new_name)) {
                if ((nodes[position].name = realloc(nodes[position].name, (strlen(nodes[position].name) + strlen(new_name) + 2) * sizeof(char))) == NULL)
                    ExitError("Error in the memory allocation of names\n", 5);
                strcat(nodes[position].name, ";");
                strcat(nodes[position].name, new_name);
            }
        }
        else
            asprintf(&nodes[position].name, "%s", new_name);
    }
}


/*
    We store all the data following its shape in the file. It is neccessary to know positions of id, latitude, longitude, name, two-ways clause and the starting point of successors, as well as the number of tokens in the nodes, and of initial comments, nodes and ways.
    Relations are skipped.
    Everything is finally stored in a binary file (with the smae structure as the one then loades).
 */

int main() {
    clock_t start;
    start = clock();
    bool two_ways;
    char *token, *auxiliar_token, *data_string = NULL;
    int new_position, actual_position;
    unsigned int i, k, actual_name, new_name;
    unsigned long N_tokens_nodes = 11UL, N_comments = 3UL, N_nodes = 23895681UL, N_ways = 1417363UL, N_total_successors = 0UL, N_total_characters = 0UL;
    
    size_t buffer_size = 0;
    
    FILE *catalunya_data;
    if ((catalunya_data = fopen(input_file_name, "r")) == NULL)
        ExitError("The input data file cannot be opened", 1);
    
    /* Opening the file. */
    if (catalunya_data == NULL)
        ExitError("\nWe cannot acces to the nodes data file\n", 2);
    
    /* Reserving memory for the nodes. */
    node *nodes;
    if ((nodes = (node *) malloc(N_nodes * sizeof(node))) == NULL)
        ExitError("Error in the memory allocation of nodes\n", 3);
    
    /* Skipping comments. */
    for (i = 0; i < N_comments; i++)
        getline(&data_string, &buffer_size, catalunya_data);
    
    printf("--- Loading the data ---\n");
    
    /* Loading nodes. */
    for (i = 0; i < N_nodes; i++) {
        getline(&data_string, &buffer_size, catalunya_data);
        nodes[i].nsucc = 0;
        
        /* Initializing the names. */
        asprintf(&nodes[i].name, "%s", "");
        
        /* Initializing the successors. */
        if ((nodes[i].successors = (unsigned long *) malloc(sizeof(unsigned long))) == NULL)
            ExitError("Error in the memory allocation of successors\n", 4);
        
        for (k = 0; k < N_tokens_nodes; k++) {
            token = strsep(&data_string, "|");
            
            if (k == 1)
                nodes[i].id = atoi(token);
            
            if (k == 9)
                nodes[i].latitude = atof(token);
            
            if (k == 10)
                nodes[i].longitude = atof(token);
        }
    }
    
    /* Loading ways. */
    for (i = 0; i < N_ways; i++) {
        getline(&data_string, &buffer_size, catalunya_data);
        two_ways = false;
        
        for (k = 0; k < 8; k++) {
            token = strsep(&data_string, "|");
            
            if (k == 1)
                auxiliar_token = strsep(&data_string, "|");
            
            if (k == 6 && strlen(token) == 0)
                two_ways = true;
        }
        
        /* Trying to load the first node. */
        if ((token = strsep(&data_string, "|")) == NULL)
            continue;
        
        actual_name = atoi(token);
                    
        while (token != NULL) {
            if ((actual_position = binary_search(actual_name, nodes, N_nodes)) >= 0)
                break; // So the objective is fulfilled.
            
            if ((token = strsep(&data_string, "|")) == NULL)
                continue;
            
            actual_name = atoi(token);
        }
        
        /* Inserting the first node's name. */
        add_name(auxiliar_token, nodes, actual_position);
        
        /* Different approachs depending on the orientation of the way. */
        if (two_ways)
            while (token != NULL) {
                /* Trying to load the next node. Else, it continues in the while loop. */
                if ((token = strsep(&data_string, "|")) == NULL )
                    continue;
                
                new_name = atoi(token);
                                                        
                if ((new_position = binary_search(new_name, nodes, N_nodes)) < 0)
                    continue;
                
                if ((nodes[actual_position].successors = realloc(nodes[actual_position].successors, (nodes[actual_position].nsucc+1) * sizeof(unsigned long))) == NULL)
                    ExitError("Error in the memory allocation of successors\n", 5);
                
                if ((nodes[new_position].successors = realloc(nodes[new_position].successors, (nodes[new_position].nsucc+1) * sizeof(unsigned long))) == NULL)
                    ExitError("Error in the memory allocation of successors\n", 6);
                
                /* Inserting the next node's name. */
                add_name(auxiliar_token, nodes, new_position);
                
                nodes[actual_position].successors[nodes[actual_position].nsucc] = new_position;
                nodes[new_position].successors[nodes[new_position].nsucc] = actual_position;
                
                nodes[actual_position].nsucc++;
                nodes[new_position].nsucc++;
                
                actual_position = new_position;
            }
        
        else
            while (token != NULL) {
                /* Trying to load the next node. Else, it continues in the while loop. */
                if ((token = strsep(&data_string, "|")) == NULL )
                    continue;
                
                new_name = atoi(token);
                                    
                if ((new_position = binary_search(new_name, nodes, N_nodes)) < 0)
                    continue;
                
                if ((nodes[actual_position].successors = (unsigned long *) realloc(nodes[actual_position].successors, (nodes[actual_position].nsucc+1) * sizeof(unsigned long))) == NULL)
                    ExitError("Error in the memory allocation of successors\n", 7);
                
                /* Inserting the next node's name. */
                add_name(auxiliar_token, nodes, new_position);
                
                nodes[actual_position].successors[nodes[actual_position].nsucc] = new_position;
                nodes[actual_position].nsucc++;
                
                actual_position = new_position;
            }
    }
    
    fclose(catalunya_data);
    
    printf("--- Writing the data ---\n");
    
    FILE *binary_data;
    if ((binary_data = fopen(output_file_name, "wb")) == NULL)
        ExitError("The output binary data file cannot be opened", 8);
    
    /* Computing the total number of successors. */
    for (i = 0; i < N_nodes; i++) {
        N_total_successors += nodes[i].nsucc;
        if (strlen(nodes[i].name))
            N_total_characters += strlen(nodes[i].name);
    }
    N_total_characters += N_nodes; // We are having 'N_nodes' times '|'.
    
    /* Reserving memory for the nodes, successsors and names. */
    if (fwrite(&N_nodes, sizeof(unsigned long), 1, binary_data) + fwrite(&N_total_successors, sizeof(unsigned long), 1, binary_data) + fwrite(&N_total_characters, sizeof(unsigned long), 1, binary_data) != 3)
        ExitError("When initializing the output binary data file", 9);
    
    /* Writing all nodes. */
    if (fwrite(nodes, sizeof(node), N_nodes, binary_data) != N_nodes)
        ExitError("When writing nodes to the output binary data file", 10);
    
    /* Writing sucessors in blocks. */
    for (i = 0; i < N_nodes; i++)
        if (nodes[i].nsucc)
            if (fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, binary_data) != nodes[i].nsucc)
                ExitError("When writing edges to the output binary data file", 11);
    
    /* Writing names in blocks. */
    for (i = 0; i < N_nodes; i++)
        if (fwrite(nodes[i].name, sizeof(char), strlen(nodes[i].name), binary_data) + fwrite("|", sizeof(char), 1, binary_data) != strlen(nodes[i].name)+1)
            ExitError("When writing names to the output binary data file", 11);
    
    fclose(binary_data);
    printf("--- Finished in %.2f seconds ---\n", (float) (clock() - start) / CLOCKS_PER_SEC);
}
