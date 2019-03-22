#include <stdio.h>
#include "stock.h"

int main(int argc, char **argv)
{
    Stock fluffy_the_cat("NanoSmart", 0, 0);
    Stock test;
    test.show();

    /*
    fluffy_the_cat.acquire("NanoSmart", 20, 12.50);
    fluffy_the_cat.show();
    fluffy_the_cat.buy(15, 18.125);
    fluffy_the_cat.show();
    fluffy_the_cat.sell(400, 20.00);
    fluffy_the_cat.show();
    fluffy_the_cat.buy(300000, 40.125);
    fluffy_the_cat.show();
    fluffy_the_cat.sell(300000, 80.125);
    fluffy_the_cat.show();
    */
    printf("new object\n");
    test = Stock();
    printf("new object\n");

    const Stock land = Stock("land", 0, 0);
    land.show();

    return 0;
}
