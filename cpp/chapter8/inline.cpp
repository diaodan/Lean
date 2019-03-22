#include <iostream>
using namespace std;

inline int square(int x)
{
    return x*x;
}

int main(int argc, char **argv)
{
    int a , b = 10;
    a = square(4);

    int &r_a = a;
     cout << &a;
    cout << endl;
     cout << &r_a;
    cout << endl;
    r_a = b;
    cout << &b;
    cout << endl;
    cout << &r_a;
    cout <<a <<b;
    cout << endl;


    cout << a;
    cout << endl;
    return 0;
}
