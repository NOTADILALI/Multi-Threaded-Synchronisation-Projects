#!/bin/bash

# Output file to store the results
output_file="table3.log"

# Fixed Sudoku size and task increment
sudoku_size=90
task_inc=20

# Thread counts to test
threads=("1" "2" "4" "8" "16" "32")

# Number of repetitions per configuration
repetitions=5

# Define the programs to run
programs=("Assgn2SrcTAS-co23btech11012.exe" "Assgn2SrcCAS-co23btech11012.exe" "Assgn2SrcBCAS-co23btech11012.exe")

# Start the log file
echo "Experiment 3 - Time vs. Number of Threads" > $output_file
echo "=========================================" >> $output_file
echo "Sudoku Size: $sudoku_size, Task Increment: $task_inc" >> $output_file
echo "Thread Counts: ${threads[@]}" >> $output_file
echo "Programs: ${programs[@]}" >> $output_file
echo "" >> $output_file

# Loop over each thread count
for num_threads in "${threads[@]}"; do
    echo "Running experiments for Number of Threads: $num_threads" >> $output_file
    echo "------------------------------------------------------" >> $output_file

    # Run each program
    for program in "${programs[@]}"; do
        echo "Running $program with Threads: $num_threads" >> $output_file

        # Iterate 5 times for each program
        for ((i=1; i<=$repetitions; i++)); do
            # Generate Sudoku before each iteration
            ./generate_sudoku.sh $sudoku_size > inp.txt
            ./$program $num_threads $sudoku_size $task_inc inp.txt >> $output_file 2>&1
        done

        # Log last 5 lines of respective output file
        output_file_name=""
        case $program in
            "Assgn2SrcTAS-co23btech11012.exe") output_file_name="outputTAS.txt" ;;
            "Assgn2SrcCAS-co23btech11012.exe") output_file_name="outputCAS.txt" ;;
            "Assgn2SrcBCAS-co23btech11012.exe") output_file_name="outputBCAS.txt" ;;
        esac

        if [ -f "$output_file_name" ]; then
            echo "Last 5 lines from $output_file_name for Threads $num_threads:" >> $output_file
            tail -n 5 $output_file_name >> $output_file
        else
            echo "Error: $output_file_name not found!" >> $output_file
        fi

        echo "" >> $output_file
    done
done

echo "Experiment 3 completed. Results are stored in $output_file."
