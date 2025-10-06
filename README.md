# SW::CPP::ADVANCED::RoboBank Portfolio

**Topics:** Inheritance, Polymorphism, STL Containers  
**Built over:** Calculator (P1), Ledger (P2), Account (P3)

---

## Overview
RoboBank Portfolio manages a collection of accounts and applies batches of transactions across them.

It demonstrates:
- **Inheritance:** a polymorphic account interface with specialized account types.
- **Polymorphism:** virtual methods for posting deposits/withdrawals/fees/interest.
- **STL containers:** `std::unordered_map`, `std::vector`, `std::unique_ptr` for ownership and fast lookup.

**Integration with previous projects:**
- Uses Calculator for all money math (no ad-hoc arithmetic).  
- Consumes Ledger transaction arrays by wrapping them into STL vectors inside Portfolio.  
- Wraps/extends Account (P3) behavior via an interface and derived classes.  
- This module is the “glue” system that ties together the earlier modules into a multi-account, production-like workflow.

---

## Goals
- A polymorphic account interface with two concrete types (Checking, Savings).  
- A Portfolio manager that owns many accounts and routes transactions by `account_id`.  
- Batch application of transactions from Ledger’s parallel arrays, internally converted into STL vectors.  
- Transfers between accounts (two-leg postings).  
- Reports: balances per account, totals, by-type summaries.  
- Reuse Calculator for math; reuse Account (P3) concepts (TxRecord) and behavior.

---

## Data Types (builds on P3)

### Enumerations
- **AccountType:** Checking = 0, Savings = 1  
  Purpose: categorize accounts; used during creation and reporting.

- **TxKind:** Deposit = 0, Withdrawal = 1, Fee = 2, Interest = 3, TransferIn = 4, TransferOut = 5  
  Purpose: classify transactions coming from Ledger and Portfolio operations.

### Structs (extensions of P3)
- **AccountSettings**: per-account configuration  
  Fields: `AccountType type`, `double apr`, `long long fee_flat_cents`

- **TxRecord**: a transaction record compatible with Account (P3) and Portfolio  
  Fields: `TxKind kind`, `long long amount_cents`, `long long timestamp`, `std::string note`, `std::string account_id`  

- **TransferRecord**: two-leg movement of money between accounts  
  Fields: `std::string from_id`, `std::string to_id`, `long long amount_cents`, `long long timestamp`, `std::string note`  

---

## Class Design (Inheritance + Polymorphism)

### Interface (abstract base)
- **class IAccount**: common polymorphic interface for all account types.  
  Holds identity, metadata, state, and operations (Calculator-backed).

### Concrete base (shared implementation)
- **class BaseAccount : public IAccount**  
  Purpose: share common data and default behaviors, still abstract in `type()`.  
  Implements default deposit/withdraw/fee/post_simple_interest/apply using Calculator (P1).  

### Derived accounts (specialization)
- **class CheckingAccount:** checking-specific helpers (e.g., monthly fee).  
- **class SavingsAccount:** savings-specific helpers (e.g., posting interest monthly).  

### Portfolio (STL Containers + Routing)
- **class Portfolio:** owns a heterogeneous set of accounts and applies transactions.  
  Storage:
  - `std::unordered_map<std::string, std::unique_ptr<IAccount>>` for fast lookup and ownership.  
  - `std::vector<TxRecord>` for batches and audit trails.

**Life-cycle:** Add, get, and count accounts.  
**Application paths:**  
- `apply_all(const std::vector<TxRecord>&)` → STL-based path  
- `apply_from_ledger(...)` → wraps Ledger arrays into `std::vector<TxRecord>`  

**Transfers:** two-leg postings (withdraw/deposit).  
**Reports:** balance of one account, total exposure, totals by type, list of account ids.  

**Integration summary:**  
- Calculator (P1): math engine  
- Ledger (P2): Portfolio ingests raw arrays, then uses STL containers  
- Account (P3): upgraded with inheritance  

---

## Function I/O

- **Portfolio::apply_from_ledger(...)**  
  Input: Ledger’s arrays for account ids, transaction types, amounts, and count.  
  Output: none  
  Behavior: wraps raw arrays into `std::vector<TxRecord>` and calls `apply_all(...)`.

- **Portfolio::transfer(tr)**  
  Input: `TransferRecord`  
  Output: bool (false if any account missing)  
  Behavior: two-leg posting.

- **Portfolio::total_exposure()**  
  Output: sum of all balances.

---

## Policies
- Money: integer cents  
- Rounding: Calculator decides  
- APR: validate with `validate_rate`  
- Audit capacity: cap per account, drop oldest if overflow  
- Missing accounts: apply(...) may auto-create or skip, transfer(...) returns false if either side missing  
- Determinism: all applications preserve array/vector order  

---

## Acceptance Examples

1. **Create portfolio and accounts**  
   - Add CHK-001 (Checking, fee=150)  
   - Add SAV-010 (Savings, apr=5%)  
   - Confirm size = 2  

2. **Apply transactions (vector path)**  
   - Deposit 100,000 → CHK-001  
   - Withdraw 25,000 → CHK-001  
   - Fee 1,500 → CHK-001  
   - Result: CHK-001 balance = 73,500  

3. **Savings interest**  
   - Post simple interest for 31 days at 5% APR on 500,000  
   - Result: balance ~502,123  

4. **Transfer between accounts**  
   - Transfer 30,000 from SAV-010 to CHK-001  
   - Result: balances adjusted accordingly  

5. **Apply transactions (Ledger path)**  
   - Provide Ledger-style arrays, call `apply_from_ledger(...)`  
   - Portfolio converts to vector internally and processes  
   - Result: balances match Ledger example  

6. **Totals**  
   - `total_exposure()` returns sum of all balances
  
   
**Author:** Nourhan Ahmed El-Shiekh
