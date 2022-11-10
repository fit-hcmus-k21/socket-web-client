#include <pthread.h>
#include <bits/stdc++.h>
using namespace std;

// demo how to use pthread functions

void *printArr(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

void printStr(char *str) {
    cout << str << endl;
}

int main() {
    // create 10 threads
    pthread_t threads[10];
    int arr[10];
    for (int i = 0; i < 5; i++) {
        arr[i] = i;
        pthread_create(&threads[i], NULL, (void *(*)(void *))printArr, (void *)arr);
    }
    for (int i = 5; i < 10; i++) {
        char *str = (char *)malloc(100);
        sprintf(str, "thread %d", i);
        pthread_create(&threads[i], NULL, (void *(*)(void *))printStr, (void *)str);
    }
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    cout << "\nDone" << endl;

    return 0;

}