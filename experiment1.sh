#!/bin/bash

# Output file to store the results
output_file="experiment1_results.log"

# Define the Sudoku sizes
sizes=("20" "30" "40" "50" "60" "70" "80" "90" "100")

# Number of repetitions for averaging the results
repetitions=5

# Define the programs
programs=("Assgn2SrcTAS-co23btech11012.exe" "Assgn2SrcCAS-co23btech11012.exe" "Assgn2SrcBCAS-co23btech11012.exe" "Assgn2SrcSEQ-co23btech11012.exe")

# Start the log file
echo "Experiment 1 - Sudoku Validation" > $output_file
echo "=================================" >> $output_file
echo "Sudoku sizes: ${sizes[@]}" >> $output_file
echo "Programs: ${programs[@]}" >> $output_file
echo "" >> $output_file

# Loop over each Sudoku size
for size in "${sizes[@]}"; do
    echo "Running experiments for Sudoku size: $size" >> $output_file
    echo "------------------------------------------------------" >> $output_file

    # Run the experiment for each program
    for program in "${programs[@]}"; do
        echo "Running $program for Sudoku size: $size" >> $output_file
        total_time=0

        # Run the experiment 'repetitions' times for each program
        for ((i=1; i<=$repetitions; i++)); do
            start_time=$(date +%s%3N)
            ./$program $size 8 20 input.txt >> $output_file 2>&1
            end_time=$(date +%s%3N)
            run_time=$((end_time - start_time))
            total_time=$((total_time + run_time))
        done

        # Calculate the average time for this program
        avg_time=$((total_time / repetitions))
        echo "Average time for $program with Sudoku size $size: $avg_time ms" >> $output_file
        echo "" >> $output_file
    done
done

echo "Experiments completed. Results are stored in $output_file."
