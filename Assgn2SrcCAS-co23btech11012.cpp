#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <mutex> // Mutex for thread-safe logging

using namespace std;

int N, K, taskInc;
vector<vector<int>> sudoku;
atomic<int> C(0);
atomic<int> invalid_found(0);
atomic<int> cas_lock_var(0);
ofstream output_file;
chrono::time_point<chrono::high_resolution_clock> total_start_time, total_end_time;
long total_cs_entry_time_ns = 0, total_cs_exit_time_ns = 0;
long worst_cs_entry_time_ns = 0, worst_cs_exit_time_ns = 0;
int cs_entry_count = 0, cs_exit_count = 0;
mutex output_mutex; // Mutex for controlling access to output_file

// Get current system time in a formatted string
string getFormattedTime() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    struct tm *timeinfo = localtime(&now_time);
    int hr = timeinfo->tm_hour;
    int min = timeinfo->tm_min;
    ostringstream oss;
    oss << setfill('0') << setw(2) << hr << "." << setw(2) << min << "hr";
    return oss.str();
}

void cas_lock() {
    int expected = 0;
    while (!cas_lock_var.compare_exchange_strong(expected, 1, memory_order_acquire)) {
        expected = 0;
        std::this_thread::yield();
    }
}

void cas_unlock() {
    cas_lock_var.store(0, memory_order_release);
}

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

void validateSudoku(int thread_id) {
    auto start_time = chrono::high_resolution_clock::now();
    
    // Logging entry request
    {
        lock_guard<mutex> guard(output_mutex);
        output_file << "Thread " << thread_id + 1 << " requests to enter CS1 at " << getFormattedTime() << endl;
    }
    
    while (true) {
        cas_lock();
        auto cs_entry_time = chrono::high_resolution_clock::now();
        total_cs_entry_time_ns += chrono::duration_cast<chrono::nanoseconds>(cs_entry_time - start_time).count();
        worst_cs_entry_time_ns = max(worst_cs_entry_time_ns, total_cs_entry_time_ns);
        cs_entry_count++;
        
        // Logging thread entry to CS
        {
            lock_guard<mutex> guard(output_mutex);
            output_file << "Thread " << thread_id + 1 << " enters CS1 at " << getFormattedTime() << endl;
        }

        int task_start = C.fetch_add(taskInc, memory_order_relaxed);
        cas_unlock();
        
        auto cs_exit_time = chrono::high_resolution_clock::now();
        total_cs_exit_time_ns += chrono::duration_cast<chrono::nanoseconds>(cs_exit_time - cs_entry_time).count();
        worst_cs_exit_time_ns = max(worst_cs_exit_time_ns, total_cs_exit_time_ns);
        cs_exit_count++;
        
        // Logging thread exit from CS
        {
            lock_guard<mutex> guard(output_mutex);
            output_file << "Thread " << thread_id + 1 << " leaves CS1 at " << getFormattedTime() << endl;
        }
        
        if (task_start >= 3 * N || invalid_found.load(memory_order_relaxed)) break;
        
        for (int task = task_start; task < task_start + taskInc && task < 3 * N; task++) {
            bool valid = false;
            if (task < N) valid = validateRow(task);
            else if (task < 2 * N) valid = validateColumn(task - N);
            else {
                int gridSize = sqrt(N);
                int row = ((task - 2 * N) / gridSize) * gridSize;
                int col = ((task - 2 * N) % gridSize) * gridSize;
                valid = validateSubgrid(row, col, gridSize);
            }

            // Logging task completion
            {
                lock_guard<mutex> guard(output_mutex);
                output_file << "Thread " << thread_id + 1 << " completes checking of task " << task + 1 << " and finds it as " << (valid ? "valid." : "invalid.") << endl;
            }

            if (!valid) {
                invalid_found.store(1, memory_order_relaxed);
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
    inFile >> K >> N >> taskInc;
    sudoku.resize(N, vector<int>(N));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            inFile >> sudoku[i][j];
        }
    }
    inFile.close();

    output_file.open("outputCAS.txt", ios::out);  // Open in text mode
    if (!output_file) {
        cerr << "Error: Could not open outputCAS.txt" << endl;
        return 1;
    }
    total_start_time = chrono::high_resolution_clock::now();

    vector<std::thread> threads;
    for (int i = 0; i < K; i++) {
        threads.emplace_back(validateSudoku, i);
    }
    for (auto &t : threads) t.join();

    total_end_time = chrono::high_resolution_clock::now();
    
    // Logging final results
    {
        lock_guard<mutex> guard(output_mutex);
        output_file << "Sudoku is " << (invalid_found.load() ? "invalid." : "valid.") << endl;
        output_file << "The total time taken is " << chrono::duration_cast<chrono::microseconds>(total_end_time - total_start_time).count() << " microseconds." << endl;
        output_file << "Average time taken by a thread to enter the CS is " << (cs_entry_count > 0 ? total_cs_entry_time_ns / cs_entry_count / 1000 : 0) << " microseconds" << endl;
        output_file << "Average time taken by a thread to exit the CS is " << (cs_exit_count > 0 ? total_cs_exit_time_ns / cs_exit_count / 1000 : 0) << " microseconds" << endl;
        output_file << "Worst-case time taken by a thread to enter the CS is " << worst_cs_entry_time_ns / 1000 << " microseconds" << endl;
        output_file << "Worst-case time taken by a thread to exit the CS is " << worst_cs_exit_time_ns / 1000 << " microseconds" << endl;
        output_file.flush();  // Ensure all data is written to the file
    }

    output_file.close();
    return 0;
}
