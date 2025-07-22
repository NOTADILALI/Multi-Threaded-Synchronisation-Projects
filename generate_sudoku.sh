#!/bin/bash

# Generate a Sudoku grid of size N x N
size=$1
n=$(($size / 2))  # n is the square root of N (for sub-grids)

# Initialize an empty Sudoku grid
grid=()

# Function to generate a random Sudoku grid
generate_grid() {
    for ((i=0; i<$size; i++)); do
        for ((j=0; j<$size; j++)); do
            # Random number between 1 and N (inclusive)
            grid[$i,$j]=$((1 + RANDOM % $size))
        done
    done
}

# Function to save the Sudoku grid to inp.txt
save_grid_to_file() {
    for ((i=0; i<$size; i++)); do
        row=""
        for ((j=0; j<$size; j++)); do
            row="${row}${grid[$i,$j]} "
        done
        echo "$row" >> inp.txt
    done
}

# Generate and save the grid to inp.txt
generate_grid
save_grid_to_file
