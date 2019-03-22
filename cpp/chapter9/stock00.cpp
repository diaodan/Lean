#include <stdio.h>
#include "stock.h"

Stock::~Stock()
{
    printf("Destructor %x\n", this);
}

Stock::Stock()
{
    printf("Default constructor %x\n", this);
    company = "No name";
    shares = 0;
    share_val = 0;
    total_val = 0.0;
}

Stock::Stock(const string &co, long n, double pr)
{
    company = co;
    printf("Constructor %x\n", this);
    if (n < 0) {
        printf("Number of shares sold can't be negative. "
               " %s shares set to 0.\n", company.c_str());
        shares = 0;
    } else {
        shares = n;
    }
    share_val = pr;
    set_tot();
}

void Stock::acquire(const string &co, long n, double pr)
{
    company = co;

    if (n < 0) {
        printf("Number of shares can't be negative: "
                "%s shares set to 0.\n", company.c_str());
        shares = 0;
    } else {
        shares = n;
    }

    share_val = pr;
    set_tot();

    return ;
}

void Stock::buy(long num, double price)
{
    if (num < 0) {
        printf("Number of shares purchased can't be negative. "
               "Transaction is aborted.\n");
    } else {
        shares += num;
        share_val = price;
        set_tot();
    }
}

void Stock::sell(long num, double price)
{
    if (num < 0) {
        printf("Number of shares sold can't be negative. "
               "Transaction is aborted.\n");
    } else if (num > shares) {
        printf("You can't sell more than you have! "
               "Transaction is aborted.\n");
    } else {
        shares -= num;
        share_val = price;
        set_tot();
    }
}

void Stock::update(double price)
{
    share_val = price;
    set_tot();
}

void Stock::show() const
{
    printf("Company: %s, Shares: %ld, Shares price: %f, Total Worth: %f\n",
                    company.c_str(), shares, share_val, total_val);
}



