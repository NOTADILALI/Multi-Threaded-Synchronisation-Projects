#README

Overview:
This code is written in C++ and relies on the pthread library, so a Linux environment with 
g++ compiler is required to compile and run the program. 
The code also leverages the C++ atomic library to perform atomic operations, 
ensuring thread safety when dealing with shared resources.

Requirements:
Compiler: C++ (g++) compiler
Libraries: pthread.h, C++ atomic library
Environment: Linux

Instructions:

1. To execute seperate method

Input File Format (inp.txt):
The input file inp.txt must be formatted as follows:

First line:
Contains three values: K (number of threads), N (dimension of the Sudoku grid),
 and taskInc (task increment).

Subsequent lines:
The Sudoku puzzle itself in an N x N matrix format.

Output File Format (output.txt):
The output file output.txt contains the following:

Timestamps (in nanoseconds):
Indicates when each thread requests to enter the critical section, when it enters, and when it exits.

Validation Details:
Lists the specific dimensions (row, column, or subgrid) checked by each thread, including indices and validation results.

Overall Result:
The final validity of the Sudoku puzzle, along with the total time taken (in nanoseconds).

Critical Section Timings:
The average and worst-case times for entering and exiting the critical section for each thread.

Commands:

./Assgn2SrcTAS-co23btech11012  - for tas method 
./Assgn2SrcCAS-co23btech11012  - for cas method
./Assgn2SrcBCAS-co23btech11012 - for bounded cas method
./Assgn2SrcSEQ-co23btech11012  - for sequential method

2. To execute any experiment

./experiment1.sh - for variable sudoku size
./experiment2.sh - for variable TaskInc
./experiment3.sh - for variable number of threads

