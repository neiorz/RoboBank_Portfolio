// Enumerations for account type and transaction kind
#ifndef ENUMS_H
#define ENUMS_H

enum AccountType
{
    Checking = 0,
    Savings = 1
};

enum TxKind
{
    Deposit = 0,
    Withdrawal = 1,
    Fee = 2,
    Interest = 3
    , TransferIn = 4,
    TransferOut = 5
};

#endif // ENUMS_H
