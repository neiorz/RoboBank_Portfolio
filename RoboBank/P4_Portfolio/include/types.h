#pragma once

#include <string>

namespace p4 {

struct AccountSettings
{
    int type;
    double apr; 
    long long fee_flat_cents;
};

enum TxKind
{
    Deposit = 0,
    Withdrawal = 1,
    Fee = 2,
    Interest = 3,
    TransferIn = 4,
    TransferOut = 5
};

struct TxRecord
{
    TxKind kind;
    long long amount_cents;
    long long timestamp;
    std::string note;
    std::string account_id; 
};

struct TransferRecord
{
    std::string from_id;
    std::string to_id;
    long long amount_cents;
    long long timestamp;
    std::string note;
};

}
