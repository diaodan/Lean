#include <iostream>
#include <string>

using namespace std;

struct free_throws {
    string name;
    int made;
    int attempts;
    float percent;
};

void display(const free_throws &ft);
void set_pc(free_throws &ft);
free_throws & accumulate(free_throws & target, const free_throws &source);

const free_throws & clone(free_throws &ft)
{
    free_throws *pt = NULL;

    *pt = ft;
    return *pt;
}

void dribble(char *bits)
{
    return ;
}

void dribble(const char *bits)
{
    return ;
}

int main(int argc, char **argv)
{
    free_throws one = {"one", 13, 14};
    free_throws two = {"two", 10, 16};
    free_throws three = {"three", 7, 9};
    free_throws four = {"four", 5, 9};
    free_throws five = {"five", 6, 14};
    free_throws team = {"team", 0, 0};


    const char *p1 = "p1";
    char p2[20] = "p2";

    dribble(p1);
    dribble(p2);

    free_throws dup;
    set_pc(one);
    display(one);
    accumulate(team, one);
    display(team);

    display(accumulate(team, two));
    accumulate(accumulate(team, three), four);
    display(team);

    dup = accumulate(team, five);
    cout << "Displaying team: \n";
    display(team);
    cout << "Displaying dup after assigment: \n";
    display(dup);
    set_pc(four);

    accumulate(dup, five) = four;
    cout << "Displaying dup after ill-advised assigment: \n";
    display(dup);

    const free_throws & jolly = clone(three);
    display(jolly);


    return 0;

}

void display(const free_throws &ft)
{
    cout << "Name: " << ft.name << '\n';
    cout << "Made: " << ft.made << '\t';
    cout << "Attemps: " << ft.attempts << '\t';
    cout << "Percent: " << ft.percent << '\n';
}

void set_pc(free_throws & ft)
{
    if (ft.attempts != 0) {
        ft.percent = 100.0f * float(ft.made) / float(ft.attempts);
    } else {
        ft.percent = 0;
    }
}

free_throws & accumulate(free_throws & target, const free_throws & source)
{
    target.attempts += source.attempts;
    target.made += source.made;
    set_pc(target);
    return target;
}
