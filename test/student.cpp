#include "student.h"

Student::Student()
{
    name = " ";
    age = 0;
    score = 0;
}

Student::Student(string name, int age, int score)
{
    this->name = name;
    this->age = age;
    this->score = score;
}

Student::~Student()
{
}

string Student::getName()
{
    return name;
}

void Student::show()
{
    cout << "Name: " << name << endl;
    cout << "Age: " << age << endl;
    cout << "Score: " << score << endl;
}
