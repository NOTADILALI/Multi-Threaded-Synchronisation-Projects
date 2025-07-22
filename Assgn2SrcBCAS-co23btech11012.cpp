#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <mutex>

using namespace std;
using namespace chrono;

atomic<int> counter(0);   // Shared counter for dynamic task allocation
atomic<int> lockVar(0);   // Lock variable for mutual exclusion
int N, K, taskInc;
vector<vector<int>> sudoku;
ofstream outputFile("outputBCAS.txt");
double totalCSentry = 0, totalCSexit = 0, worstCSentry = 0, worstCSexit = 0;
int csEntries = 0, csExits = 0;
mutex logMutex;
bool sudokuValid = true;  // Flag to indicate if Sudoku is valid or invalid

// Get current system time in a formatted string
string getFormattedTime() {
    auto now = system_clock::now();
    time_t now_time = system_clock::to_time_t(now);
    struct tm *timeinfo = localtime(&now_time);
    int hr = timeinfo->tm_hour;
    int min = timeinfo->tm_min;
    int sec = timeinfo->tm_sec;
    int ms = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;
    ostringstream oss;
    oss << setfill('0') << setw(2) << hr << "." << setw(2) << min << "hr";
    return oss.str();
}

// CAS lock for mutual exclusion
void bounded_cas_lock() {
    while (true) {
        int expected = 0;
        if (lockVar.compare_exchange_weak(expected, 1)) {
            return;
        }
    }
}

// CAS unlock
void bounded_cas_unlock() {
    lockVar.store(0);
}

// Validate a row
bool validate_row(int row) {
    vector<bool> seen(N + 1, false);
    for (int j = 0; j < N; j++) {
        if (sudoku[row][j] < 1 || sudoku[row][j] > N || seen[sudoku[row][j]]) return false;
        seen[sudoku[row][j]] = true;
    }
    return true;
}

// Validate a column
bool validate_column(int col) {
    vector<bool> seen(N + 1, false);
    for (int i = 0; i < N; i++) {
        if (sudoku[i][col] < 1 || sudoku[i][col] > N || seen[sudoku[i][col]]) return false;
        seen[sudoku[i][col]] = true;
    }
    return true;
}

// Validate a subgrid (for N = 8, subgrids are 2x2)
bool validate_subgrid(int startRow, int startCol) {
    vector<bool> seen(N + 1, false);
    int gridSize = static_cast<int>(sqrt(N));  // Grid size for subgrid (2 for 8x8 Sudoku)
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int value = sudoku[startRow + i][startCol + j];
            if (value < 1 || value > N || seen[value]) return false;
            seen[value] = true;
        }
    }
    return true;
}

// Worker thread function
void worker(int thread_id) {
    auto thread_start = high_resolution_clock::now();

    while (true) {
        auto request_time = high_resolution_clock::now();
        {
            lock_guard<mutex> logLock(logMutex);
            outputFile << "Thread " << thread_id << " requests to enter CS1 at " << getFormattedTime() << "\n";
        }

        bounded_cas_lock();
        auto cs_entry_time = high_resolution_clock::now();
        double entryTime = duration_cast<microseconds>(cs_entry_time - request_time).count();
        totalCSentry += entryTime;
        worstCSentry = max(worstCSentry, entryTime);
        csEntries++;

        {
            lock_guard<mutex> logLock(logMutex);
            outputFile << "Thread " << thread_id << " enters CS1 at " << getFormattedTime() << "\n";
        }

        int taskIndex = counter.fetch_add(taskInc);  // Dynamic task allocation
        if (taskIndex >= 3 * N || !sudokuValid) {   // Early termination if Sudoku is invalid
            bounded_cas_unlock();
            break;
        }

        // Assign tasks dynamically: rows, columns, subgrids
        for (int i = 0; i < taskInc && (taskIndex + i) < 3 * N; i++) {
            int currTask = taskIndex + i;
            if (currTask < N) {
                // Validate row
                outputFile << "Thread " << thread_id << " grabs row " << currTask + 1 << " at " << getFormattedTime() << "\n";
                bool valid = validate_row(currTask);
                outputFile << "Thread " << thread_id << " completes checking of row " << currTask + 1 << " at " << getFormattedTime() << " and finds it " << (valid ? "valid.\n" : "invalid.\n");
                if (!valid) sudokuValid = false;
            } else if (currTask < 2 * N) {
                // Validate column
                int colIndex = currTask - N;
                outputFile << "Thread " << thread_id << " grabs column " << colIndex + 1 << " at " << getFormattedTime() << "\n";
                bool valid = validate_column(colIndex);
                outputFile << "Thread " << thread_id << " completes checking of column " << colIndex + 1 << " at " << getFormattedTime() << " and finds it " << (valid ? "valid.\n" : "invalid.\n");
                if (!valid) sudokuValid = false;
            } else {
                // Validate subgrid
                int subgridIndex = currTask - 2 * N;
                int gridSize = static_cast<int>(sqrt(N));  // For N=8, gridSize=2 (2x2 subgrids)
                int row = (subgridIndex / gridSize) * gridSize;
                int col = (subgridIndex % gridSize) * gridSize;
                outputFile << "Thread " << thread_id << " grabs subgrid at (" << row + 1 << ", " << col + 1 << ") at " << getFormattedTime() << "\n";
                bool valid = validate_subgrid(row, col);
                outputFile << "Thread " << thread_id << " completes checking of subgrid at (" << row + 1 << ", " << col + 1 << ") at " << getFormattedTime() << " and finds it " << (valid ? "valid.\n" : "invalid.\n");
                if (!valid) sudokuValid = false;
            }
        }

        auto cs_exit_time = high_resolution_clock::now();
        double exitTime = duration_cast<microseconds>(cs_exit_time - cs_entry_time).count();
        totalCSexit += exitTime;
        worstCSexit = max(worstCSexit, exitTime);
        csExits++;

        bounded_cas_unlock();

        {
            lock_guard<mutex> logLock(logMutex);
            outputFile << "Thread " << thread_id << " leaves CS1 at " << getFormattedTime() << "\n";
        }
    }
}

// Main function
int main() {
    ifstream inputFile("inp.txt");
    inputFile >> K >> N >> taskInc;
    sudoku.resize(N, vector<int>(N));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            inputFile >> sudoku[i][j];
    inputFile.close();

    auto start_time = high_resolution_clock::now();
    vector<thread> threads;
    for (int i = 0; i < K; i++)
        threads.emplace_back(worker, i + 1);
    for (auto &t : threads)
        t.join();

    auto end_time = high_resolution_clock::now();

    outputFile << "Sudoku is " << (sudokuValid ? "valid" : "invalid") << ".\n";
    outputFile << "The total time taken is " << duration_cast<microseconds>(end_time - start_time).count() << " microseconds.\n";
    outputFile << "Average time taken by a thread to enter the CS is " << (csEntries > 0 ? totalCSentry / csEntries : 0) << " microseconds\n";
    outputFile << "Average time taken by a thread to exit the CS is " << (csExits > 0 ? totalCSexit / csExits : 0) << " microseconds\n";
    outputFile << "Worst-case time taken by a thread to enter the CS is " << worstCSentry << " microseconds\n";
    outputFile << "Worst-case time taken by a thread to exit the CS is " << worstCSexit << " microseconds\n";

    outputFile.close();
    return 0;
}



