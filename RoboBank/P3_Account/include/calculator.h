#ifndef CALCULATOR_H
#define CALCULATOR_H

namespace Calculator
{
    inline long long deposit(long long balance, long long amount)
    {
        return balance + amount;
    }
    inline long long withdrawal(long long balance, long long amount)
    {
        return balance - amount;
    }
    inline long long fee(long long balance, long long fee)
    {
        return balance - fee;
    }
    inline long long interest(long long balance, double apr, int days, int basis)
    {
        // Simple interest: balance * apr * days / basis
        return balance + static_cast<long long>(balance * apr * days / basis + 0.5);
    }
}

#endif // CALCULATOR_H
