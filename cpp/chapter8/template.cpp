#include <iostream>

using namespace std;

struct job {
    char name[40];
    double salary;
    int floor;
};

template <typename T>
void Swap(T *a, T *b)
{
    T temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

template <typename T>
void Swap(T *a, T *b, int n)
{
    T temp;
    
    for (int i = 0; i < n; i++) {
        temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}

void Show(int a[], int n)
{
    for (int i = 0; i < n; i++) {
        cout << a[i] <<" ";
    }
    cout << endl;
}

template <> void Swap<job>(job *j1, job *j2)
{
    double t1;
    int t2;
    t1 = j1->salary;
    j1->salary = j2->salary;
    j2->salary = t1;
    t2 = j1->floor;
    j1->floor = j2->floor;
    j2->floor = t2;
    return ;
}

void Show(job *j)
{
    cout << "Name: " << j->name << endl;
    cout << "Salary : " << j->salary << endl;
    cout << " on floor " << j->floor << endl;
}

extern const int finger;
static int count = 20000;
void funct1()
{
    static int count = 10;
    cout << __func__ << endl;
    cout << count << endl;
    count ++;
}

void funct2()
{
    cout << __func__ << endl;
    cout << count << endl;
    count ++;
    cout << "finger : " << finger << endl;
}

int main()
{
    int i = 10, j = 20;
    cout << "Before swap\n";
    cout << "i = " << i << endl;
    cout << "j = " << j << endl;
    Swap(&i, &j);
    cout << "After swap\n";
    cout << "i = " << i << endl;
    cout << "j = " << j << endl;

    float f1 = 10.001, f2 = 20.002;
    cout << "Before swap\n";
    cout << "f1 = " << f1 << endl;
    cout << "f2 = " << f2 << endl;
    Swap(&f1, &f2);
    cout << "After swap\n";
    cout << "f1 = " << f1 << endl;
    cout << "f2 = " << f2 << endl;

    int d1[5] = {1, 2, 3, 4, 5};
    int d2[5] = {10, 20, 30, 40, 50};

    cout << "Before swap " << endl;
    cout << "D1 : ";
    Show(d1, 5);
    cout << endl;
    cout << "D2 : ";
    Show(d2, 5);
    Swap(d1, d2, 5);
    cout << "After swap " << endl;
    cout << "D1 : ";
    Show(d1, 5);
    cout << endl;
    cout << "D2 : ";
    Show(d2, 5);

    struct job sue = {"Sue", 73000.60, 7};
    struct job sidney = {"Sidney", 679000, 10};

    Show(&sue);
    Show(&sidney);
    Swap(&sue, &sidney);
    Show(&sue);
    Show(&sidney);


    funct1();
    funct2();
    funct1();
    funct2();

    return 0;
}

