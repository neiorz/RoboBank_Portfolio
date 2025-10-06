#include "../include/portfolio.h"
#include <algorithm>
#include <iostream>
#include "Account.h"

using namespace std;

// BaseAccount implementation
BaseAccount::BaseAccount(const string &id, const AccountSettings &settings, long long opening_balance_cents)
    : id_(id), settings_(settings), balance_cents_(opening_balance_cents)
{
}

const string &BaseAccount::id() const { return id_; }
long long BaseAccount::balance_cents() const { return balance_cents_; }

void BaseAccount::deposit(long long amount_cents, long long ts, const string &note)
{
    balance_cents_ = Calculator::deposit(balance_cents_, amount_cents);
    // record
    TxRecord r{TxKind::Deposit, amount_cents, ts, note.c_str()};
    audit_.push_back(r);
    if (audit_.size() > 256) audit_.erase(audit_.begin());
}

void BaseAccount::withdraw(long long amount_cents, long long ts, const string &note)
{
    balance_cents_ = Calculator::withdrawal(balance_cents_, amount_cents);
    TxRecord r{TxKind::Withdrawal, amount_cents, ts, note.c_str()};
    audit_.push_back(r);
    if (audit_.size() > 256) audit_.erase(audit_.begin());
}

void BaseAccount::charge_fee(long long fee_cents, long long ts, const string &note)
{
    balance_cents_ = Calculator::fee(balance_cents_, fee_cents);
    TxRecord r{TxKind::Fee, fee_cents, ts, note.c_str()};
    audit_.push_back(r);
    if (audit_.size() > 256) audit_.erase(audit_.begin());
}

void BaseAccount::post_simple_interest(int days, int basis, long long ts, const string &note)
{
    long long interest_amt = Calculator::interest(balance_cents_, settings_.apr, days, basis);
    balance_cents_ = Calculator::deposit(balance_cents_, interest_amt);
    TxRecord r{TxKind::Interest, interest_amt, ts, note.c_str()};
    audit_.push_back(r);
    if (audit_.size() > 256) audit_.erase(audit_.begin());
}

void BaseAccount::apply(const TxRecordExt &tx)
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
        post_simple_interest(0, 365, tx.timestamp, tx.note);
        break;
    default:
        break;
    }
}

std::vector<TxRecord> BaseAccount::audit() const { return audit_; }

// CheckingAccount
CheckingAccount::CheckingAccount(const string &id, const AccountSettings &settings, long long opening_balance_cents)
    : BaseAccount(id, settings, opening_balance_cents)
{
}
AccountType CheckingAccount::type() const { return AccountType::Checking; }
void CheckingAccount::charge_monthly_fee(long long ts)
{
    charge_fee(settings_.fee_flat_cents, ts, "monthly fee");
}

// SavingsAccount
SavingsAccount::SavingsAccount(const string &id, const AccountSettings &settings, long long opening_balance_cents)
    : BaseAccount(id, settings, opening_balance_cents)
{
}
AccountType SavingsAccount::type() const { return AccountType::Savings; }
void SavingsAccount::accrue_interest(int days, int basis, long long ts)
{
    post_simple_interest(days, basis, ts, "accrued interest");
}

// Portfolio
Portfolio::Portfolio() {}

bool Portfolio::add_account(const string &id, const AccountSettings &settings, long long opening_balance_cents)
{
    if (accounts_.find(id) != accounts_.end()) return false;
    if (settings.type == AccountType::Checking)
        accounts_[id] = make_unique<CheckingAccount>(id, settings, opening_balance_cents);
    else
        accounts_[id] = make_unique<SavingsAccount>(id, settings, opening_balance_cents);
    return true;
}

IAccount *Portfolio::get_account(const string &id) const
{
    auto it = accounts_.find(id);
    return (it == accounts_.end()) ? nullptr : it->second.get();
}

size_t Portfolio::count() const { return accounts_.size(); }

void Portfolio::apply_all(const vector<TxRecordExt> &txs, bool auto_create)
{
    for (const auto &t : txs)
    {
        auto it = accounts_.find(t.account_id);
        if (it == accounts_.end())
        {
            if (!auto_create) continue;
            // create with default settings
            AccountSettings s; s.type = AccountType::Checking; s.apr = 0.0; s.fee_flat_cents = 0;
            add_account(t.account_id, s, 0);
            it = accounts_.find(t.account_id);
        }

        IAccount *acc = it->second.get();
        acc->apply(t);
        audit_.push_back(t);
    }
}

void Portfolio::apply_from_ledger(const char tx_account_id[][MAX_LEN], const int tx_type[], const int tx_amount_cents[], int tx_count)
{
    vector<TxRecordExt> v;
    v.reserve(tx_count);
    for (int i = 0; i < tx_count; ++i)
    {
        TxRecordExt tx;
        tx.kind = static_cast<TxKind>(tx_type[i]);
        tx.amount_cents = tx_amount_cents[i];
        tx.timestamp = 0;
        tx.note = "";
        tx.account_id = string(tx_account_id[i]);
        v.push_back(tx);
    }
    apply_all(v, true);
}

bool Portfolio::transfer(const TransferRecord &tr)
{
    auto from = accounts_.find(tr.from_id);
    auto to = accounts_.find(tr.to_id);
    if (from == accounts_.end() || to == accounts_.end()) return false;

    // Withdraw from source
    TxRecordExt out_tx{TxKind::TransferOut, tr.amount_cents, tr.timestamp, tr.note, tr.from_id};
    from->second->apply(out_tx);
    // Deposit to dest
    TxRecordExt in_tx{TxKind::TransferIn, tr.amount_cents, tr.timestamp, tr.note, tr.to_id};
    to->second->apply(in_tx);
    audit_.push_back(out_tx);
    audit_.push_back(in_tx);
    return true;
}

long long Portfolio::balance_of(const string &id) const
{
    auto it = accounts_.find(id);
    return (it == accounts_.end()) ? 0 : it->second->balance_cents();
}

long long Portfolio::total_exposure() const
{
    long long sum = 0;
    for (const auto &p : accounts_) sum += p.second->balance_cents();
    return sum;
}

vector<string> Portfolio::list_ids() const
{
    vector<string> ids;
    ids.reserve(accounts_.size());
    for (const auto &p : accounts_) ids.push_back(p.first);
    return ids;
}

unordered_map<AccountType, long long> Portfolio::totals_by_type() const
{
    unordered_map<AccountType, long long> out;
    for (const auto &p : accounts_)
    {
        out[p.second->type()] += p.second->balance_cents();
    }
    return out;
}
