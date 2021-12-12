# A*

Here a implementation of A* algoritm can be found. It is executed in 'A_Star_algorithm.c', with all needed functions and constants defined in 'auxiliar_functions.h'. As well, a program to convert the initial file ('spain.csv') into the required structure (a binary file will be created) is attached under the name 'get_binary_file.c'. This will be the input of the main algorithm.

In order to execute these programs, 'asprintf' function has to be available. Since it is part of GNU, it may be already available for some GNU versions. Otherwise, including '#define _GNU_SOURCE' may be needed (just dis-commenting this line in 'get_binary_file.c' and 'auxiliar_functions.h').

The resulting path from Barcelona to Sevilla is included under the name 'Results.dat', which is generated each time the A* algorithm is executed.
