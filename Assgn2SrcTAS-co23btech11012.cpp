#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cmath>

using namespace std;
using namespace chrono;

int N, K, taskInc;
vector<vector<int>> sudoku;
atomic<int> C(0);
atomic<int> invalid_found(0);
atomic_flag lock_flag = ATOMIC_FLAG_INIT;
ofstream output_file;

auto total_start_time = high_resolution_clock::now();
long total_cs_entry_time_ns = 0, total_cs_exit_time_ns = 0;
long worst_cs_entry_time_ns = 0, worst_cs_exit_time_ns = 0;
int cs_entry_count = 0, cs_exit_count = 0;

void tas_lock() {
    auto start = high_resolution_clock::now();
    while (lock_flag.test_and_set(memory_order_acquire)) {
        std::this_thread::yield();  // Allow other threads to run
    }
    auto end = high_resolution_clock::now();
    long entry_time = duration_cast<microseconds>(end - start).count();
    total_cs_entry_time_ns += entry_time;
    worst_cs_entry_time_ns = max(worst_cs_entry_time_ns, entry_time);
    cs_entry_count++;
}

void tas_unlock() {
    auto start = high_resolution_clock::now();
    lock_flag.clear(memory_order_release);
    auto end = high_resolution_clock::now();
    long exit_time = duration_cast<microseconds>(end - start).count();
    total_cs_exit_time_ns += exit_time;
    worst_cs_exit_time_ns = max(worst_cs_exit_time_ns, exit_time);
    cs_exit_count++;
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

void validateSudoku(int thread_id) {
    while (true) {
        tas_lock();
        auto request_time = high_resolution_clock::now();
        output_file << "Thread " << thread_id << " requests to enter CS1 at " << duration_cast<microseconds>(request_time - total_start_time).count() << " microseconds" << endl;
        
        int task_start = C.fetch_add(taskInc, memory_order_relaxed);
        tas_unlock();

        if (task_start >= N || invalid_found.load(memory_order_relaxed)) break;

        for (int task = task_start; task < task_start + taskInc && task < N; task++) {
            auto enter_time = high_resolution_clock::now();
            output_file << "Thread " << thread_id << " enters CS1 at " << duration_cast<microseconds>(enter_time - total_start_time).count() << " microseconds" << endl;
            output_file << "Thread " << thread_id << " grabs row " << task + 1 << " at " << duration_cast<microseconds>(enter_time - total_start_time).count() << " microseconds" << endl;
            bool valid = validateRow(task);
            auto leave_time = high_resolution_clock::now();
            output_file << "Thread " << thread_id << " leaves CS1 at " << duration_cast<microseconds>(leave_time - total_start_time).count() << " microseconds" << endl;
            
            if (!valid) {
                invalid_found.store(1, memory_order_relaxed);
                return;
            }
            output_file << "Thread " << thread_id << " completes checking of row " << task + 1 << " at " << duration_cast<microseconds>(leave_time - total_start_time).count() << " microseconds and finds it as valid." << endl;
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

    output_file.open("outputTAS.txt");
    if (!output_file) {
        cerr << "Error: Could not open outputTAS.txt" << endl;
        return 1;
    }
    total_start_time = high_resolution_clock::now();

    vector<thread> threads;  
    for (int i = 0; i < K; i++) {
        threads.emplace_back(validateSudoku, i + 1);
    }
    for (auto &t : threads) t.join();

    auto total_end_time = high_resolution_clock::now();
    output_file << "Sudoku is " << (invalid_found.load(memory_order_relaxed) ? "Invalid" : "Valid") << "." << endl;
    output_file << "The total time taken is " << duration_cast<microseconds>(total_end_time - total_start_time).count() << " microseconds." << endl;
    if (cs_entry_count > 0) output_file << "Average time taken by a thread to enter the CS is " << total_cs_entry_time_ns / cs_entry_count << " microseconds" << endl;
    if (cs_exit_count > 0) output_file << "Average time taken by a thread to exit the CS is " << total_cs_exit_time_ns / cs_exit_count << " microseconds" << endl;
    output_file << "Worst-case time taken by a thread to enter the CS is " << worst_cs_entry_time_ns << " microseconds" << endl;
    output_file << "Worst-case time taken by a thread to exit the CS is " << worst_cs_exit_time_ns << " microseconds" << endl;

    output_file.close();
    return 0;
}
