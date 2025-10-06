#pragma once

#include <string>

// Portfolio-specific types are in namespace p4 to avoid collisions with P3 types.
namespace p4 {

// Per-account configuration
struct AccountSettings
{
    // Reuse AccountType enum from P3_Account/Enums.h via include in users if needed.
    int type; // use int to avoid direct dependency here (0=Checking,1=Savings)
    double apr; // APR as fractional value (e.g., 0.05 for 5%)
    long long fee_flat_cents;
};

// Transaction kind for portfolio operations (small local enum)
enum TxKind
{
    Deposit = 0,
    Withdrawal = 1,
    Fee = 2,
    Interest = 3,
    TransferIn = 4,
    TransferOut = 5
};

// Transaction record used by Portfolio (C++-style strings)
struct TxRecord
{
    TxKind kind;
    long long amount_cents;
    long long timestamp;
    std::string note;
    std::string account_id; // which account the tx targets
};

// Two-leg transfer record
struct TransferRecord
{
    std::string from_id;
    std::string to_id;
    long long amount_cents;
    long long timestamp;
    std::string note;
};

} // namespace p4
