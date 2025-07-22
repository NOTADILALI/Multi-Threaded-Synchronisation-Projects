#!/bin/bash

# Output file to store the results
output_file="table1.log"

# Define the Sudoku sizes
sizes=("20" "30" "40" "50" "60" "70" "80" "90" "100")

# Define the programs (excluding SEQ from logging)
programs=("Assgn2SrcTAS-co23btech11012.exe" "Assgn2SrcCAS-co23btech11012.exe" "Assgn2SrcBCAS-co23btech11012.exe")
output_files=("outputTAS.txt" "outputCAS.txt" "outputBCAS.txt")  # Added .txt extensions

# Function to generate a random Sudoku matrix
generate_sudoku() {
    local size=$1
    echo "8 $size 20" > inp.txt  # First line (Threads, Size, 20 as a fixed param)
    for ((i=0; i<size; i++)); do
        for ((j=0; j<size; j++)); do
            echo -n "$((RANDOM % size + 1)) " >> inp.txt
        done
        echo "" >> inp.txt
    done
}

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

    # Generate random Sudoku and store in inp.txt
    generate_sudoku $size

    # Run SEQ program first (but do not log it)
    ./Assgn2SrcSEQ-co23btech11012.exe $size 8 20 inp.txt

    # Run the experiment for each program
    for i in "${!programs[@]}"; do
        program=${programs[$i]}
        out_file=${output_files[$i]}

        echo "Running $program for Sudoku size: $size" >> $output_file

        # Run the experiment 5 times
        for ((iter=1; iter<=5; iter++)); do
            echo "Iteration $iter for $program with Sudoku size $size" >> $output_file
            
            # Run the program (append output instead of overwriting)
            ./$program $size 8 20 inp.txt >> $out_file 2>&1
            
            # Wait for output to be written
            sleep 1

            # Ensure the output file has content before reading
            while ! [ -s "$out_file" ]; do
                sleep 1
            done

            # Log last 5 lines of the output file
            echo "Last 5 lines after iteration $iter for $program:" >> $output_file
            tail -n 5 "$out_file" >> $output_file
            echo "" >> $output_file
        done
    done
done

echo "Experiments completed. Results are stored in $output_file."
