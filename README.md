# A*

Here an implementation of A* algoritm can be found, devoted to solve the minimal path from Barcelona to Sevilla.

In order to execute it, 'asprintf' function has to be available. Since it is part of GNU, it may be already available for some GNU versions. Otherwise, including '#define _GNU_SOURCE' may be needed (just dis-commenting this line in 'get_binary_file.c' and 'auxiliar_functions.h').

Then, the file with all the nodes and ways (the so-called 'spain.csv') has to be in the same folder as the executables of 'A_Star_algorithm.c', (compiled with 'auxiliar_functions.h', where all the needed functions are defined) and 'get_binary_file.c'. So, after compiling and executing this last program, a binary file will be created, which will be read by 'A_Star_algorithm.c' after its compilation and execution. The output will be stored in a new file, 'Results.dat', in which we can find each node's id, name and cumulative distance.
