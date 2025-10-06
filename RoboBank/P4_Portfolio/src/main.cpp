#include <iostream>
#include "../include/portfolio.h"

int main()
{
    Portfolio p;

    AccountSettings chk; chk.type = AccountType::Checking; chk.apr = 0.0; chk.fee_flat_cents = 150;
    AccountSettings sav; sav.type = AccountType::Savings; sav.apr = 0.05; sav.fee_flat_cents = 0;

    p.add_account("CHK-001", chk, 0);
    p.add_account("SAV-010", sav, 500000);

    std::cout << "count=" << p.count() << "\n";

    // Apply vector path
    std::vector<TxRecordExt> txs;
    txs.push_back({TxKind::Deposit, 100000, 1, "deposit", "CHK-001"});
    txs.push_back({TxKind::Withdrawal, 25000, 2, "withdraw", "CHK-001"});
    txs.push_back({TxKind::Fee, 1500, 3, "fee", "CHK-001"});
    p.apply_all(txs);
    std::cout << "CHK-001 balance=" << p.balance_of("CHK-001") << " expected 73500\n";

    // Savings interest: 31 days at 5% APR on 500,000
    std::vector<TxRecordExt> interest_tx;
    interest_tx.push_back({TxKind::Interest, 0, 4, "interest", "SAV-010"});
    p.apply_all(interest_tx);
    std::cout << "SAV-010 balance=" << p.balance_of("SAV-010") << "\n";

    // Transfer 30,000 from SAV-010 to CHK-001
    TransferRecord tr{"SAV-010", "CHK-001", 30000, 5, "transfer"};
    if (p.transfer(tr))
        std::cout << "transfer ok\n";
    std::cout << "CHK-001=" << p.balance_of("CHK-001") << " SAV-010=" << p.balance_of("SAV-010") << "\n";

    // Ledger path example (small)
    const int tx_count = 3;
    char tx_account_id[3][MAX_LEN] = {"CHK-001", "CHK-001", "SAV-010"};
    int tx_type[3] = {0, 1, 4};
    int tx_amounts[3] = {100000, 25000, 30000};
    p.apply_from_ledger(tx_account_id, tx_type, tx_amounts, tx_count);
    std::cout << "Final totals exposure=" << p.total_exposure() << "\n";

    return 0;
}
