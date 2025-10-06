// Structs for account settings and transaction record
#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"

struct AccountSettings
{
    AccountType type;
    double apr; 
    long long fee_flat_cents;
};

struct TxRecord
{
    TxKind kind;
    long long amount_cents;
    long long timestamp;
    const char *note;
};

#endif // STRUCTS_H
