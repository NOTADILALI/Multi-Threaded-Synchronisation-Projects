#!/bin/bash

# Define the input file
INPUT_FILE="inp.txt"

# Create an output file to store the results
OUTPUT_FILE="experiment3_results.log"
echo "Method, Threads, AvgTimeTaken (ms)" > $OUTPUT_FILE

# Number of threads values to test
threads_values=(1 2 4 8 16 32)

# Loop through each thread value
for threads in "${threads_values[@]}"; do
    # For each method, run the corresponding executable with the input file and thread count
    # Repeat each experiment 5 times to calculate the average time
    for method in "TAS" "CAS" "BCAS" "SEQ"; do
        total_time=0
        
        # Run the experiment 5 times to get average time
        for i in {1..5}; do
            # Execute the corresponding method's .exe file and store the result
            if [[ "$method" == "TAS" ]]; then
                start_time=$(date +%s%3N)
                ./Assgn2SrcTAS-co23btech11012.exe $INPUT_FILE $threads
            elif [[ "$method" == "CAS" ]]; then
                start_time=$(date +%s%3N)
                ./Assgn2SrcCAS-co23btech11012.exe $INPUT_FILE $threads
            elif [[ "$method" == "BCAS" ]]; then
                start_time=$(date +%s%3N)
                ./Assgn2SrcBCAS-co23btech11012.exe $INPUT_FILE $threads
            elif [[ "$method" == "SEQ" ]]; then
                start_time=$(date +%s%3N)
                ./Assgn2SrcSEQ-co23btech11012.exe $INPUT_FILE $threads
            fi

            end_time=$(date +%s%3N)
            elapsed_time=$((end_time - start_time))
            total_time=$((total_time + elapsed_time))
        done
        
        # Calculate average time in milliseconds
        avg_time=$((total_time / 5))
        
        # Log the results to the output file
        echo "$method, $threads, $avg_time" >> $OUTPUT_FILE
    done
done

echo "Experiment 3 results have been written to $OUTPUT_FILE"
