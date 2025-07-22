#!/bin/bash

# Output file to store the results
output_file="table2.log"

# Fixed Sudoku size and number of threads
sudoku_size=90
num_threads=8

# Task increment values (10 to 50 with step 10)
task_increments=("10" "20" "30" "40" "50")

# Number of repetitions per configuration
repetitions=5

# Define the programs to run
programs=("Assgn2SrcTAS-co23btech11012.exe" "Assgn2SrcCAS-co23btech11012.exe" "Assgn2SrcBCAS-co23btech11012.exe")

# Start the log file
echo "Experiment 2 - Time vs. Task Increment" > $output_file
echo "======================================" >> $output_file
echo "Sudoku Size: $sudoku_size, Num of Threads: $num_threads" >> $output_file
echo "Task Increments: ${task_increments[@]}" >> $output_file
echo "Programs: ${programs[@]}" >> $output_file
echo "" >> $output_file

# Loop over each task increment
for task_inc in "${task_increments[@]}"; do
    echo "Running experiments for Task Increment: $task_inc" >> $output_file
    echo "------------------------------------------------------" >> $output_file

    # Run each program
    for program in "${programs[@]}"; do
        echo "Running $program for Task Increment: $task_inc" >> $output_file

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
            echo "Last 5 lines from $output_file_name for Task Increment $task_inc:" >> $output_file
            tail -n 5 $output_file_name >> $output_file
        else
            echo "Error: $output_file_name not found!" >> $output_file
        fi

        echo "" >> $output_file
    done
done

echo "Experiment 2 completed. Results are stored in $output_file."
