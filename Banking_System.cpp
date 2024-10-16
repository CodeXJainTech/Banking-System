#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <string>
#include <algorithm>
#include <map>
#include <queue>
#include <memory>

using namespace std;

class BankSystem {
public:
    class BankAccount {
    public:
        class Transaction {
        public:
            unsigned long long int RecieverAccountNumber = 0;
            unsigned long long int SourceAccountNumber = 0;
            unsigned long long int TransferAmount = 0;
            string TransactionType = "";
            string TransactionTime = "";

            Transaction(unsigned long long int RecieverAccountNumber,
                        unsigned long long int SourceAccountNumber,
                        unsigned long long int TransferAmount,
                        string TransactionType)
            {
                this->RecieverAccountNumber = RecieverAccountNumber;
                this->SourceAccountNumber = SourceAccountNumber;
                this->TransferAmount = TransferAmount;
                char buffer[80];
                time_t rawtime;
                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
                this->TransactionTime = buffer;
                this->TransactionType = TransactionType;
            }
        };
    queue<Transaction *> Transactions;
    public:
        unsigned long long int AccountNumber;
        unsigned long long int AccountBalance;
        string HolderName;
        string Password;
        time_t CreationTime;

        BankAccount(unsigned long long int AccountNumber,
                    unsigned long long int AccountBalance,
                    string HolderName,
                    string Password) : AccountNumber(AccountNumber), AccountBalance(AccountBalance), HolderName(HolderName), Password(Password), CreationTime(time(0)) {
        }

        void Deposit() {
            unsigned long long int Amount;
            cout << "Please enter the amount you would like to deposit: ";
            cin >> Amount;
            cout << "Depositing: " << Amount << endl;
            this->AccountBalance += Amount;
            cout << "Deposit Successful." << endl;
            cout << "Final balance: " << this->AccountBalance << endl;
            Transaction* temp = new Transaction(AccountNumber, AccountNumber, Amount, "Withdrawl");
            this->Transactions.push(temp);
        }

        void Withdraw() {
            unsigned long long int Amount;
            cout << "Please enter the amount you would like to withdraw: ";
            cin >> Amount;
            if (this->AccountBalance >= Amount) {
                this->AccountBalance -= Amount;
                cout << "Withdrawal Successful." << endl;
                cout << "Final balance: " << this->AccountBalance << endl;
                this->Transactions.push(new Transaction(AccountNumber, AccountNumber, Amount, "Deposit"));
            } else {
                cout << "Insufficient account balance." << endl;
            }
        }

        void Transfer(BankAccount& receiver) {
            unsigned long long int Amount;
            unsigned long long int RecieverNumber;
            cout << "Please enter the amount you would like to transfer: ";
            cin >> Amount;

            if (Amount > this->AccountBalance) {
                cout << "Insufficient balance to perform this action." << endl;
                return;
            }

            char confirmation;
            cout << "Please confirm the transaction. You would like to transfer amount: " << Amount << " to account number: " << receiver.AccountNumber << endl
                 << "Y/N" << endl;
            cin >> confirmation;
            if (confirmation != 'Y') {
                cout << "Canceling the transaction.\n";
            } else {
                this->AccountBalance -= Amount;
                receiver.AccountBalance += Amount;
                cout << "Transaction Successful." << endl;
                cout << "Your balance after transaction: " << this->AccountBalance << endl;
                Transactions.push(new Transaction(RecieverNumber, AccountNumber, Amount, "Transaction"));
            }
        }

        void ChangePassword() {
            string newPassword;
            string confirmPassword;

            cout << "Enter a new password for your bank account: ";
            cin >> newPassword;
            cout << "Confirm your new bank account password: ";
            cin >> confirmPassword;

            if (newPassword == confirmPassword) {
                this->Password = newPassword;
                cout << "Password changed successfully." << endl;
            } else {
                cout << "The new passwords didn't match. Please try again." << endl;
            }    
        }

        void DisplayAccountBalance() {
            if (AccountBalance) {
                cout << "Your account balance is: " << AccountBalance << endl;
            } else {
                cout << "You are not logged in." << endl;
            }
        }
    };

    map<unsigned long long int, unique_ptr<BankAccount>> Accounts;
    BankAccount* CURRENT = nullptr;
    const string accountsFileName = "accounts.txt";

    void SaveAccountsToFile() {
        ofstream outFile("accounts.txt");
        if (!outFile) {
            cerr << "Error opening accounts file for writing." << endl;
            return;
        }

        for (const auto &acc : Accounts) {
            if (acc.second->AccountNumber == 0) {
                continue;
            }
            outFile << acc.second->AccountNumber << " "
                   << acc.second->AccountBalance << " "
                   << acc.second->HolderName << " "
                   << acc.second->Password << " "
                   << acc.second->CreationTime << endl;
        }
        outFile.close();
    }

    void LoadAccountsFromFile() {
        ifstream inFile("accounts.txt");
        if (!inFile) {
            cerr << "Error opening accounts file for reading." << endl;
            return;
        }

        unsigned long long int accountNum;
        unsigned long long int accountBal;
        string name;
        string password;
        time_t creationTime;
        while (inFile >> accountNum >> accountBal >> name >> password >> creationTime) {
            Accounts[accountNum] = make_unique<BankAccount>(accountNum, accountBal, name, password);
            Accounts[accountNum]->CreationTime = creationTime;
        }
        inFile.close();
    }

    void CreateAccount() {
        string name;
        string pass1;
        string pass2;
        unsigned long long int accountBal = 0;

        cout << "Enter your name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter a password for your bank account: ";
        cin >> pass1;
        cout << "Confirm your bank account password: ";
        cin >> pass2;

        if (pass1 != pass2) {
            cout << "The passwords didn't match. Please try again." << endl;
            cout << "\n\n\n";
            return;
        }

        cout << "Please enter your starting deposit: ";
        cin >> accountBal;
        unsigned long long int accountNum = rand() % 10000000000000000;
        
        while (accountNum < 1000000000000000) {
            accountNum <<= 1;
        }

        Accounts[accountNum] = make_unique<BankAccount>(accountNum, accountBal, name, pass1);
        Accounts[accountNum]->CreationTime = time(0);
        cout << "Your bank account has been created successfully.\nYour account number is : " << accountNum << endl;

        SaveAccountsToFile();
    }

    void Login() {
        LoadAccountsFromFile();
        BankAccount *currentAcc = nullptr;
        unsigned long long int accno;
        string pass;
        cout << "Please enter your account number: ";
        cin >> accno;
        cout << "Please enter your password: ";
        cin >> pass;

        auto it = Accounts.find(accno);
        if (it != Accounts.end()) {
            currentAcc = it->second.get();
            if (currentAcc->Password == pass) {
                cout << "You have been logged in successfully." << endl;
                CURRENT = currentAcc;
            } else {
                cout << "The entered password is incorrect. Please try again." << endl;
            }
        } else {
            cout << "Enter a valid account number." << endl;
        }
    }

    void DeleteAccount(unsigned long long int accno, const string& password) {
    auto it = Accounts.find(accno);
        if (it != Accounts.end()) {
            if (it->second->Password == password) {
                cout << "Account found." << endl;
                char confirmation;
                cout << "Are you sure you want to delete this account? (Y/N): ";
                cin >> confirmation;
                if (confirmation == 'Y' || confirmation == 'y') {
                    Accounts.erase(it);
                    cout << "Account deleted successfully." << endl;
                    SaveAccountsToFile();
                } else {
                    cout << "Account deletion canceled." << endl;
                }
            } else {
                cout << "Incorrect password for the account." << endl;
            }
        } else {
            cout << "Account not found." << endl;
        }
    }

    void DisplayAllAccounts() {
    LoadAccountsFromFile();
      string password;
      cout<<"The BankManger Password: ";
      cin >> password;
        if(password == "Secret"){
          cout << "List of all accounts:" << endl;
        for (const auto &acc : Accounts) {
            cout << "Account number: " << acc.first << endl;
            cout << "Account holder: " << acc.second->HolderName << endl;
            cout << "Account balance: " << acc.second->AccountBalance << endl;
            cout << "Creation time: " << ctime(&acc.second->CreationTime);
            cout << "\n----------------------------------\n";
        }
        }
        else{
          cout << "Password incorrect.";
        }
    }

    ~BankSystem() {
        for (auto &acc : Accounts) {
            acc.second.release();
        }
        Accounts.clear();
    }
};

int main() {
    BankSystem *SYSTEM = new BankSystem();
    srand(time(NULL));
    

    bool Running = true;

    while (Running) {
        cout << "Welcome to the bank. What would you like to do today?" << endl;
        cout << "1. Enter 1 to create a bank account" << endl;
        cout << "2. Enter 2 to log into an existing bank account" << endl;
        cout << "3. Enter 3 to delete an existing bank account" << endl;
        cout << "4. Enter 4 to display all accounts" << endl;
        cout << "5. Enter 5 to exit" << endl;

        int action;
        cin >> action;

        switch (action) {
            case 1: {
                SYSTEM->CreateAccount();
                SYSTEM->SaveAccountsToFile();
                break;
            }
            case 2: {
                SYSTEM->Login();
                break;
            }
            case 3: {
                unsigned long long int accno;
                string password;
                cout << "Enter the account number you want to delete: ";
                cin >> accno;
                cout << "Enter the password of the account: ";
                cin >> password;
                SYSTEM->DeleteAccount(accno, password);
                SYSTEM->SaveAccountsToFile();
                break;
            }
            case 4: {
                SYSTEM->DisplayAllAccounts();
                break;
            }
            case 5: {
                cout << "It was a pleasure to serve you." << endl;
                Running = false;
                break;
            }
            default: {
                cout << "Invalid option. Please try again." << endl;
                break;
            }
        }

        while (SYSTEM->CURRENT) {
            cout << "Account number: " << SYSTEM->CURRENT->AccountNumber << endl;
            cout << "Account holder: " << SYSTEM->CURRENT->HolderName << endl;
            cout << "\n\n";

            cout << "What would you like to do?" << endl;
            cout << "1. Enter 1 to deposit money into your bank account" << endl;
            cout << "2. Enter 2 to withdraw money from your bank account" << endl;
            cout << "3. Enter 3 to transfer money from your bank account" << endl;
            cout << "4. Enter 4 to display account balance" << endl;
            cout << "5. Enter 5 to change password" << endl;
            cout << "6. Enter 6 to log out" << endl;

            int accountaction;
            cin >> accountaction;
            switch (accountaction) {
                case 1: {
                    SYSTEM->CURRENT->Deposit();
                    SYSTEM->SaveAccountsToFile();
                    break;
                }
                case 2: {
                    SYSTEM->CURRENT->Withdraw();
                    SYSTEM->SaveAccountsToFile();
                    break;
                }
                case 3: {
                    unsigned long long int receiverAccountNumber;
                    cout << "Please enter the account number you want to transfer funds to: ";
                    cin >> receiverAccountNumber;
                    
                    auto it = SYSTEM->Accounts.find(receiverAccountNumber);
                    if (it != SYSTEM->Accounts.end()) {
                        SYSTEM->CURRENT->Transfer(*(it->second)); // Pass the BankAccount object
                        SYSTEM->SaveAccountsToFile();
                    } else {
                        cout << "Receiver account not found." << endl;
                    }
                    break;
                }
                case 4 : {
                SYSTEM->CURRENT->DisplayAccountBalance();
                break;
                }
                case 5: {
                SYSTEM->CURRENT->ChangePassword();
                SYSTEM->SaveAccountsToFile();
                break;
                }
                case 6: {
                    SYSTEM->CURRENT = nullptr;
                    break;
                }
                default: {
                    cout << "Invalid option. Please try again." << endl;
                    break;
                }
            }
        }
    }
    delete SYSTEM;
    return 0;
}