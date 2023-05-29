//Grant Davis
//CS 446 
//Homework #1
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

vector<int> factors;

void function(int start, int end, int n);

int main(int argc, char **argv){
    int i, n, p, half, sum = 0;
    if (argc >= 2){
        n = stoi(argv[1]);
        p = stoi(argv[2]);
    }
    half = n / 2;
    int parts = half / p;
    int count = 0;
    thread th[p];
    
    for(i = 0; i < p - 1; i++){
        th[i] = thread(function, count * parts + 1, (count + 1) * parts, n);
        count++;
    }
    th[p-1] = thread(function, count * parts + 1, half, n);
    for(i = 0; i < p; i++){
        th[i].join();
    }
    for(i = 0; i < factors.size(); i++){
        sum += factors[i];
    }
    if(sum == n){
        cout << n << " is perfect number\n";
    }
    else{
        cout << n << " is not perfect number\n";
    }
return 0;
}

void function(int start, int end, int n){
    int i, s = 0;
    for(i = start; i <= end; i++){
        if(n % i == 0){
            factors.push_back(i);
        }
    }
}