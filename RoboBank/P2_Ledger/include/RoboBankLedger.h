#ifndef ROBO_BANK_LEDGER_H
#define ROBO_BANK_LEDGER_H

const int MAX_ACCOUNTS = 20;
const int MAX_TX = 50;
const int MAX_LEN = 20;

int find_account_index(const char ac_account_id[][MAX_LEN], int ac_count, const char account_id[]);
int get_or_create_account(char ac_account_id[][MAX_LEN], int ac_balance[], int ac_capacity, int &ac_count, const char account_id[]);
void apply_one(char ac_account_id[][MAX_LEN], int ac_balance[], int ac_capacity, int &ac_count, const char account_id[], int tx_type, int amount_cents);
void apply_all(const char tx_account_id[][MAX_LEN], const int tx_type[], const int tx_amount_cents[], int tx_count,
               char ac_account_id[][MAX_LEN], int ac_balance[], int ac_capacity, int &ac_count);
int balance_of(const char ac_account_id[][MAX_LEN], const int ac_balance[], int ac_count, const char account_id[]);
void bank_summary(const int tx_type[], const int tx_amount_cents[], int tx_count,
                  const int ac_balance[], int ac_count,
                  int *out_total_deposits, int *out_total_withdrawals,
                  int *out_total_fees, int *out_total_interest,
                  int *out_net_exposure);

#endif
