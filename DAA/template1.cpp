#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;
void solve(int n) {
    long long total = 0;
    for (int i = 0; i < n; i++) {
        total += i;
    }
}
int main() {
    int n = 1000000;
    auto start = high_resolution_clock::now();
    solve(n);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Execution Time: " << duration.count() << " microseconds" << endl;
    return 0;
}