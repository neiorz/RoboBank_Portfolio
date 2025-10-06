#pragma once

inline int apply_deposit(int balance, int amount)
{
    return balance + amount;
}

inline int apply_withdrawal(int balance, int amount)
{
    return balance - amount;
}

inline int apply_fee(int balance, int amount)
{
    return balance - amount;
}

inline int apply_interest(int balance, int amount)
{
    return balance + amount;
}
