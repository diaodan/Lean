#include <iostream>
using namespace std;

struct pack {
    unsigned int f1:1;
    unsigned int f2:9;
    unsigned int f3:30;
};

struct test {
    int vaule;
};

void fun1(int a[5])
{
    char s[]="asada";
    return ;
}

int main(int argc, char **argv)
{
    const int i = 1;
    int j = 2;
    const int *pt = &i;
    const int *pt2 = &i;
    const int **ppt = NULL;
    cout << *pt;
    cout << endl;
    pt = &j;
    cout << *pt;
    cout << endl;
    ppt = &pt;
    ppt = &pt2;
    *ppt = &j;
    j = 10;
    **ppt = 21;

    std::cout << "Come up and C++ me some time";
    std::cout << std::endl;
    std::cout << "You won't regret it !" << std::endl;
    return 0;
}

