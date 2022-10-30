#include <bits/stdc++.h>
using namespace std;

class Student {
public:
    Student();
    Student(string name, int age, int score);
    ~Student();
    string getName();
    int getAge();
    int getScore();
    void setName(string name);
    void setAge(int age);
    void setScore(int score);
    void show();
private:
    string name;
    int age;
    int score;
};