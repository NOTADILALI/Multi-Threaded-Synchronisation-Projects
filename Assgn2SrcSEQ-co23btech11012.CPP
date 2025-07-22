#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;
using namespace chrono;

int N, K, taskInc;
vector<vector<int>> sudoku;
ofstream output_file;

auto total_start_time = high_resolution_clock::now();

bool validateRow(int row) {
    vector<bool> seen(N + 1, false);
    for (int i = 0; i < N; i++) {
        int val = sudoku[row][i];
        if (seen[val]) return false;
        seen[val] = true;
    }
    return true;
}

bool validateColumn(int col) {
    vector<bool> seen(N + 1, false);
    for (int i = 0; i < N; i++) {
        int val = sudoku[i][col];
        if (seen[val]) return false;
        seen[val] = true;
    }
    return true;
}

bool validateSubgrid(int startRow, int startCol, int size) {
    vector<bool> seen(N + 1, false);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int val = sudoku[startRow + i][startCol + j];
            if (seen[val]) return false;
            seen[val] = true;
        }
    }
    return true;
}

void validateSudoku() {
    auto request_time = high_resolution_clock::now();
    output_file << "Sequential execution begins at " << duration_cast<microseconds>(request_time - total_start_time).count() << " microseconds" << endl;
    
    for (int row = 0; row < N; row++) {
        auto enter_time = high_resolution_clock::now();
        output_file << "Checking row " << row + 1 << " at " << duration_cast<microseconds>(enter_time - total_start_time).count() << " microseconds" << endl;
        bool valid = validateRow(row);
        auto leave_time = high_resolution_clock::now();
        output_file << "Completed checking row " << row + 1 << " at " << duration_cast<microseconds>(leave_time - total_start_time).count() << " microseconds and it is " << (valid ? "valid." : "invalid.") << endl;

        if (!valid) {
            output_file << "Sudoku is invalid." << endl;
            return;
        }
    }

    for (int col = 0; col < N; col++) {
        auto enter_time = high_resolution_clock::now();
        output_file << "Checking column " << col + 1 << " at " << duration_cast<microseconds>(enter_time - total_start_time).count() << " microseconds" << endl;
        bool valid = validateColumn(col);
        auto leave_time = high_resolution_clock::now();
        output_file << "Completed checking column " << col + 1 << " at " << duration_cast<microseconds>(leave_time - total_start_time).count() << " microseconds and it is " << (valid ? "valid." : "invalid.") << endl;

        if (!valid) {
            output_file << "Sudoku is invalid." << endl;
            return;
        }
    }

    int gridSize = sqrt(N);
    for (int row = 0; row < N; row += gridSize) {
        for (int col = 0; col < N; col += gridSize) {
            auto enter_time = high_resolution_clock::now();
            output_file << "Checking subgrid starting at (" << row + 1 << ", " << col + 1 << ") at " << duration_cast<microseconds>(enter_time - total_start_time).count() << " microseconds" << endl;
            bool valid = validateSubgrid(row, col, gridSize);
            auto leave_time = high_resolution_clock::now();
            output_file << "Completed checking subgrid starting at (" << row + 1 << ", " << col + 1 << ") at " << duration_cast<microseconds>(leave_time - total_start_time).count() << " microseconds and it is " << (valid ? "valid." : "invalid.") << endl;

            if (!valid) {
                output_file << "Sudoku is invalid." << endl;
                return;
            }
        }
    }
}

int main() {
    ifstream inFile("inp.txt");
    if (!inFile) {
        cerr << "Error: Could not open inp.txt" << endl;
        return 1;
    }

    // Skip the first two values from the first line of inp.txt as they are not needed
    inFile >> K >> N >> taskInc;

    sudoku.resize(N, vector<int>(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            inFile >> sudoku[i][j];
        }
    }
    inFile.close();

    output_file.open("outputSEQ.txt");
    if (!output_file) {
        cerr << "Error: Could not open outputSEQ.txt" << endl;
        return 1;
    }

    total_start_time = high_resolution_clock::now();

    validateSudoku();

    auto total_end_time = high_resolution_clock::now();
    output_file << "Sudoku is valid." << endl; // Since no invalid found flag, assume valid if passed all checks
    output_file << "The total time taken is " << duration_cast<microseconds>(total_end_time - total_start_time).count() << " microseconds." << endl;

    output_file.close();
    return 0;
}
