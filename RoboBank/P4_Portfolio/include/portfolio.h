#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "types.h"
#include "Enums.h" // for AccountType (from P3_Account/include)
#include "RoboBankLedger.h"

class IAccount
{
public:
    virtual ~IAccount() = default;
    virtual const std::string &id() const = 0;
    virtual AccountType type() const = 0;
    virtual long long balance_cents() const = 0;
    virtual void deposit(long long amount_cents, long long ts, const std::string &note) = 0;
    virtual void withdraw(long long amount_cents, long long ts, const std::string &note) = 0;
    virtual void charge_fee(long long fee_cents, long long ts, const std::string &note) = 0;
    virtual void post_simple_interest(int days, int basis, long long ts, const std::string &note) = 0;
    virtual void apply(const p4::TxRecord &tx) = 0;
    virtual std::vector<p4::TxRecord> audit() const = 0;
};

class BaseAccount : public IAccount
{
public:
    BaseAccount(const std::string &id, const p4::AccountSettings &settings, long long opening_balance_cents = 0);
    const std::string &id() const override;
    virtual AccountType type() const = 0; // keep abstract for derived
    long long balance_cents() const override;
    void deposit(long long amount_cents, long long ts, const std::string &note) override;
    void withdraw(long long amount_cents, long long ts, const std::string &note) override;
    void charge_fee(long long fee_cents, long long ts, const std::string &note) override;
    void post_simple_interest(int days, int basis, long long ts, const std::string &note) override;
    void apply(const p4::TxRecord &tx) override;
    std::vector<p4::TxRecord> audit() const override;

protected:
    std::string id_;
    p4::AccountSettings settings_;
    long long balance_cents_;
    std::vector<p4::TxRecord> audit_;
};

class CheckingAccount : public BaseAccount
{
public:
    CheckingAccount(const std::string &id, const p4::AccountSettings &settings, long long opening_balance_cents = 0);
    AccountType type() const override;
    // additional helpers
    void charge_monthly_fee(long long ts);
};

class SavingsAccount : public BaseAccount
{
public:
    SavingsAccount(const std::string &id, const p4::AccountSettings &settings, long long opening_balance_cents = 0);
    AccountType type() const override;
    // additional helpers
    void accrue_interest(int days, int basis, long long ts);
};

class Portfolio
{
public:
    Portfolio();
    bool add_account(const std::string &id, const p4::AccountSettings &settings, long long opening_balance_cents = 0);
    IAccount *get_account(const std::string &id) const;
    size_t count() const;
    void apply_all(const std::vector<p4::TxRecord> &txs, bool auto_create = true);
    void apply_from_ledger(const char tx_account_id[][MAX_LEN], const int tx_type[], const int tx_amount_cents[], int tx_count);
    bool transfer(const p4::TransferRecord &tr);
    long long balance_of(const std::string &id) const;
    long long total_exposure() const;
    std::vector<std::string> list_ids() const;
    std::unordered_map<AccountType, long long> totals_by_type() const;

private:
    std::unordered_map<std::string, std::unique_ptr<IAccount>> accounts_;
    std::vector<p4::TxRecord> audit_; // portfolio level audit
};
