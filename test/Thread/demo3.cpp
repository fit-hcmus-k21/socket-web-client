#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

void *func1 (int n) {
    for (int i = 0; i < n; i++) {
        cout << i << " ";
    }
    return NULL;
}

void *func2 (string *str) {
    for (int i = 0 ; i < str->length(); i++) {
        cout << (*str)[i] << " ";
    }
    return NULL;
}

int main(){
    int n = 22;
    string *s = new string("hello world I'm Thanh Ngan: ponyo 225");
    pthread_t t1, t2;
    pthread_create(&t1, NULL, (void *(*)(void *))func1, (void *)n);
    pthread_create(&t2, NULL, (void *(*)(void *))func2, (void *)s);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    cout << "Done" << endl;
    return 225;

}