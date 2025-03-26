#include<iostream>
using namespace std;

// Egg breaking problem

int main() {
    int input;
    int h = 1;
    cout << "Enter 1 if egg breaks and 0 when egg does not when prompted to do so" << endl;
    while(true) {
        cout << "Does the egg break at " << h << "th floor?" << endl;
        cin >> input;
        if(input == 1) {
            break;
        }
        h *= 2;
    }
    int upper = h;
    int lower = h / 2;
    if(lower == 0) {
        cout << "Highest floor from which we can drop the egg without breaking is " << lower << endl;
        return 0;
    }
    int mid = (upper + lower) / 2;
    while(true) {
        cout << "Does the egg break at " << mid << "th floor?" << endl;
        cin >> input;
        if(input == 1) {
            upper = mid;
        }
        if(input == 0) {
            lower = mid + 1;
        }
        if(lower == upper) {
            cout << "Highest floor from which we can drop the egg without breaking is " << lower << endl;
            return 0;
        }
        mid = (lower + upper) / 2;
    }
}