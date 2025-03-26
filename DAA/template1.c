#include <stdio.h>
#include <time.h>
void solve(int n) {
    long long total = 0;
    for (int i = 0; i < n; i++) {
        total += i;
    }
}
int main() {
    int n = 1000000;
    clock_t start, end;
    start = clock();
    solve(n);
    end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Execution Time: %f seconds\n", time_taken);
    return 0;
}