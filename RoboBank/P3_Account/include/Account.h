#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "structs.h"
#include "calculator.h"

const int kMaxAudit = 256;

class Account
{
public:
    Account(const char *id, const AccountSettings &settings, long long opening_balance_cents = 0);
    const char *id() const;
    AccountType type() const;
    double apr() const;
    long long balance_cents() const;
    int audit_size() const;
    const TxRecord *audit_data() const;
// operations 
    void deposit(long long amount_cents, long long ts, const char *note = nullptr);
    void withdraw(long long amount_cents, long long ts, const char *note = nullptr);
    void charge_fee(long long fee_cents, long long ts, const char *note = nullptr);
    void post_simple_interest(int days, int basis, long long ts, const char *note = nullptr);
    void apply(const TxRecord &tx);

private:
    void record(TxKind kind, long long amount, long long ts, const char *note);

    const char *id_;
    AccountSettings settings_;
    long long balance_cents_;
    TxRecord audit_[kMaxAudit];
    int audit_count_;
};

#endif // ACCOUNT_H
