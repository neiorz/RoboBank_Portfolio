#include "account.h"
#include <cstring>

Account::Account(const char *id, const AccountSettings &settings, long long opening_balance_cents)
    : id_(id), settings_(settings), balance_cents_(opening_balance_cents), audit_count_(0) {}

const char *Account::id() const { return id_; }
AccountType Account::type() const { return settings_.type; }
double Account::apr() const { return settings_.apr; }
long long Account::balance_cents() const { return balance_cents_; }
int Account::audit_size() const { return audit_count_; }
const TxRecord *Account::audit_data() const { return audit_; }

void Account::deposit(long long amount_cents, long long ts, const char *note)
{
    balance_cents_ = Calculator::deposit(balance_cents_, amount_cents);
    record(TxKind::Deposit, amount_cents, ts, note);
}

void Account::withdraw(long long amount_cents, long long ts, const char *note)
{
    balance_cents_ = Calculator::withdrawal(balance_cents_, amount_cents);
    record(TxKind::Withdrawal, amount_cents, ts, note);
}

void Account::charge_fee(long long fee_cents, long long ts, const char *note)
{
    balance_cents_ = Calculator::fee(balance_cents_, fee_cents);
    record(TxKind::Fee, fee_cents, ts, note);
}

void Account::post_simple_interest(int days, int basis, long long ts, const char *note)
{
    long long interest_amt = Calculator::interest(balance_cents_, settings_.apr, days, basis);
    balance_cents_ = Calculator::deposit(balance_cents_, interest_amt);
    record(TxKind::Interest, interest_amt, ts, note);
}

void Account::apply(const TxRecord &tx)
{
    switch (tx.kind)
    {
    case TxKind::Deposit:
        deposit(tx.amount_cents, tx.timestamp, tx.note);
        break;
    case TxKind::Withdrawal:
        withdraw(tx.amount_cents, tx.timestamp, tx.note);
        break;
    case TxKind::Fee:
        charge_fee(tx.amount_cents, tx.timestamp, tx.note);
        break;
    case TxKind::Interest:
        // if amount is positive ---> post interest 
        if (tx.amount_cents > 0)
            deposit(tx.amount_cents, tx.timestamp, tx.note);
        break;
    }
}

void Account::record(TxKind kind, long long amount, long long ts, const char *note)
{
    if (audit_count_ < kMaxAudit)
    {
        audit_[audit_count_++] = {kind, amount, ts, note};
    }
    else
    {
        // Drop oldest
        for (int i = 1; i < kMaxAudit; ++i)
            audit_[i - 1] = audit_[i];
        audit_[kMaxAudit - 1] = {kind, amount, ts, note};
    }
}
