#!/bin/bash

# Experiment 2: Time vs Task Increment (taskInc from 10 to 50)
# Running validation for each method (TAS, CAS, BCAS, SEQ) and logging the results

# Fixed parameters
sudoku_size=90
num_threads=8
sudoku_file="inp.txt"
output_file="experiment2_results.log"
methods=("TAS" "CAS" "BCAS" "SEQ")
exe_files=("Assgn2SrcTAS-co23btech11012.exe" "Assgn2SrcCAS-co23btech11012.exe" "Assgn2SrcBCAS-co23btech11012.exe" "Assgn2SrcSEQ-co23btech11012.exe")

# Create or clear the output file
echo "Method, TaskInc, AvgTimeTaken (ms)" > $output_file

# Loop over taskInc values from 10 to 50 (in increments of 10)
for taskInc in {10..50..10}
do
    # Run the experiment for each method
    for i in "${!methods[@]}"
    do
        method=${methods[$i]}
        exe_file=${exe_files[$i]}
        
        total_time=0

        # Run the experiment 5 times for each taskInc and method
        for j in {1..5}
        do
            # Prepare the input file with the current taskInc value
            echo "$num_threads $sudoku_size $taskInc" > $sudoku_file
            
            # Use the existing generate_sudoku.sh to generate a random Sudoku
            ./generate_sudoku.sh $sudoku_size >> $sudoku_file  # This will append the generated Sudoku to inp.txt

            # Run the program and measure the execution time in milliseconds
            start_time=$(date +%s%3N)
            ./$exe_file $sudoku_file
            end_time=$(date +%s%3N)

            # Calculate elapsed time in milliseconds
            elapsed_time=$((($end_time - $start_time)))
            
            # Add the time to the total time
            total_time=$((total_time + elapsed_time))
        done

        # Calculate the average time
        avg_time=$((total_time / 5))

        # Log the method, taskInc, and average time taken in ms
        echo "$method, $taskInc, $avg_time" >> $output_file
    done
done

echo "Experiment 2 completed. Results saved to $output_file."
