#include <cstring>
#include "RoboBankLedger.h"
#include "LedgerCalculator.h"

int find_account_index(const char ac_account_id[][MAX_LEN], int ac_count, const char account_id[])
{
    for (int i = 0; i < ac_count; i++)
    {
        if (strcmp(ac_account_id[i], account_id) == 0)
        {
            return i;
        }
    }
    return -1;
}

int get_or_create_account(char ac_account_id[][MAX_LEN], int ac_balance[], int ac_capacity, int &ac_count, const char account_id[])
{
    int idx = find_account_index(ac_account_id, ac_count, account_id);
    if (idx != -1)
        return idx;

    if (ac_count < ac_capacity)
    {
        strcpy(ac_account_id[ac_count], account_id);
        ac_balance[ac_count] = 0;
        return ac_count++;
    }
    return -1; // no space
}

void apply_one(char ac_account_id[][MAX_LEN], int ac_balance[], int ac_capacity, int &ac_count,
               const char account_id[], int tx_type, int amount_cents)
{
    int idx = get_or_create_account(ac_account_id, ac_balance, ac_capacity, ac_count, account_id);
    if (idx == -1)
        return; // no space

    switch (tx_type)
    {
    case 0: // Deposit
    case 3: // Interest
    case 4: // TransferIn
        ac_balance[idx] = apply_deposit(ac_balance[idx], amount_cents);
        break;
    case 1: // Withdrawal
    case 5: // TransferOut
        ac_balance[idx] = apply_withdrawal(ac_balance[idx], amount_cents);
        break;
    case 2: // Fee
        ac_balance[idx] = apply_fee(ac_balance[idx], amount_cents);
        break;
    }
}

void apply_all(const char tx_account_id[][MAX_LEN], const int tx_type[], const int tx_amount_cents[], int tx_count,
               char ac_account_id[][MAX_LEN], int ac_balance[], int ac_capacity, int &ac_count)
{
    for (int i = 0; i < tx_count; i++)
    {
        apply_one(ac_account_id, ac_balance, ac_capacity, ac_count, tx_account_id[i], tx_type[i], tx_amount_cents[i]);
    }
}

int balance_of(const char ac_account_id[][MAX_LEN], const int ac_balance[], int ac_count, const char account_id[])
{
    int idx = find_account_index(ac_account_id, ac_count, account_id);
    return (idx != -1) ? ac_balance[idx] : 0;
}

void bank_summary(const int tx_type[], const int tx_amount_cents[], int tx_count,
                  const int ac_balance[], int ac_count,
                  int *out_total_deposits, int *out_total_withdrawals,
                  int *out_total_fees, int *out_total_interest,
                  int *out_net_exposure)
{

    *out_total_deposits = *out_total_withdrawals = *out_total_fees = *out_total_interest = *out_net_exposure = 0;

    for (int i = 0; i < tx_count; i++)
    {
        switch (tx_type[i])
        {
        case 0:
        case 4:
            *out_total_deposits += tx_amount_cents[i];
            break;
        case 1:
        case 5:
            *out_total_withdrawals += tx_amount_cents[i];
            break;
        case 2:
            *out_total_fees += tx_amount_cents[i];
            break;
        case 3:
            *out_total_interest += tx_amount_cents[i];
            break;
        }
    }

    for (int i = 0; i < ac_count; i++)
    {
        *out_net_exposure += ac_balance[i];
    }
}
