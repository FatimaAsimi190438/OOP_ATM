//fatima asim
//19i0943
//CS(f)
//Final Project
//ATM
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include<cstdio>

using namespace std;

class Keypad {
public:
    int getInput(){
        int inp = 1;
        do {

        cin>>inp;
        if (inp < 0 ){
            cout<<"\nERROR : Invalid Input : Enter again (0-9) :";
        }

        }while (inp<0);
        system("clear");
        return inp;
    }

};

class Screen {
    public:
    void display(string s){
        cout<<s<<endl;

    }

    void displayNum(double num){
        cout<<num;
    }

    void show( string s){
        cout<<s<<" ";
    }

    void cls(){
        system("clear");
    }
};

class CashDispenser {
    
    const int initCount = 500;
    int billCount ;
    public:
        
        CashDispenser(){
            billCount = initCount;
        }

        void dispenseCash(double amount){
            int billsRequired = ( (int)amount)/20;
            billCount = billsRequired;
        }

        bool isSufficientBalanceAvailable(double amount){
            int billsRequired =  (amount)/20;
            return ( billCount >= billsRequired ) ;
        }

        int getBillCount() const {
            return billCount;
        }

        CashDispenser operator=(const CashDispenser &cd){
            this->billCount = cd.getBillCount() ;
            return *this;
        }
};



class DepositSlot {
public:
    bool isEnvelopRecieved(){
        return true;
    }
};

class Account {

    int accountNumber;
    int pin;
    double balance;
    public:

    Account(){
        accountNumber = 0;
        pin = 0;
        balance = 0;
    }

    Account (int AccountNumber , int PIN , double amount){
        accountNumber = AccountNumber;
        pin = PIN;
        if (amount>(-1))
            balance = amount;
    }

    void setAccount (int AccountNumber , int PIN , double amount){
        accountNumber = AccountNumber;
        pin = PIN;
        if (amount>(-1))
            balance = amount;
    }

    
    bool validatePIN(int pin){
            return (this->pin == pin);

    }

    double getBalance(){
        return balance;
    }
    
    
    void credit(double amount){
        if (amount>=0)
        balance+= amount;    
    }

    void debit(double amount){
        if (amount <= balance )
        balance-=amount ;
    
    }

    int getAccountNumber(){
        return accountNumber;
    }
 
};


class BankDatabase{
    Account* accounts;
    int totalAccounts;

    public:
    BankDatabase(){
            ifstream inFile;
            inFile.open("db.txt");

            // Check for error
            if (inFile.fail()) {
                cerr << "error  in database" << endl;
                exit(1);
            }
            int numOfAccounts;
            inFile >> numOfAccounts ;
            totalAccounts = numOfAccounts ;
            
            accounts = new Account[numOfAccounts];

            int pin,accountNumber,balance ;

            // Read file till you reach the end and check for matchs
            int index = 0 ;
            while (inFile >> accountNumber >> pin >> balance) {
            accounts[index].setAccount(accountNumber,pin,balance);
            index++;
            }

    }

    Account* getAccount(int acountNumber){
        for (int i = 0 ; i < totalAccounts ; i++){
            if (accounts[i].getAccountNumber() == acountNumber)
                return &accounts[i];
        }
        return NULL;
    }


    bool authenticateUser(int accountNum , int pin){
        Account* userAccount = getAccount( accountNum );
        if ( userAccount != nullptr )
            return userAccount->validatePIN( pin ); 
        else
            return false;
    }

    double getBalance(int accountNum){
        Account* userAccount = getAccount( accountNum );
        return userAccount->getBalance();
    }

    void credit(int accountNum , double amount ){
        Account* userAccount = getAccount( accountNum );
        userAccount->credit(amount);
    }

    void debit(int accountNum , double amount ){
        Account* userAccount = getAccount( accountNum );
        userAccount->debit(amount);
    }

};



class Transaction {
protected:
    int accountNumber;
    Screen* tranScreen;
    BankDatabase* db;

    public :
    Transaction (){
        accountNumber = 0 ;
        
    }
    Transaction( int userAccount, Screen* scr ,BankDatabase* database ){
    accountNumber = userAccount;
    tranScreen = scr;
    db = database;
    } 

    int getAccountNumber(){
        return accountNumber;
    }

    Screen* getTransactionScreen(){
        return tranScreen;
    }

    BankDatabase* getBankDatabase(){
        return db;
    }

    virtual void execute()=0;
};

class Withdrawal : public Transaction {
    double amount;
    Keypad keypad;
    CashDispenser cashDispenser;


public:
    Withdrawal(int accNum , Screen* sc , BankDatabase* DB , Keypad keypad , CashDispenser atmCashDisp )
        : Transaction (accNum , sc , DB) {
            this->keypad = keypad;
            cashDispenser = atmCashDisp;

        }
    enum withdrawal_menu { CANCELLED = 6 };
    void execute(){
        bool cashDispensed = false ;
        bool transactionCancelled = false;

        do {
            int choice = displayWithdrawalMenu();
        
            if (choice != CANCELLED ){

                amount = choice ;

                double balance=db->getBalance(accountNumber);

                if (amount <= balance ){
                    if (cashDispenser.isSufficientBalanceAvailable(amount))
                    {
                        db->debit(accountNumber,amount);
                        cashDispenser.dispenseCash(amount);
                        cashDispensed = true;
                        tranScreen->display("Please Take your cash from Cash dispenser.");
                    }
                    else 
                        tranScreen->display("Insufficient cash Available in the ATM \n\nPleaase Choose the smaller amount ");
                }
            }
            else {
                tranScreen->display("Cancelling transaction... ");
                transactionCancelled = true;
            }

          } while(!cashDispensed && !transactionCancelled );     
    
    }

    int displayWithdrawalMenu(){

        int ch = 0 ;

        int amounts[5] = {20,40,60,100,200};

        while (ch==0)
        {
            
            tranScreen->display("\nWithdrawal options:" );
            tranScreen->display("1 - $20" );
            tranScreen->display("2 - $40" );
            tranScreen->display("3 - $60" );
            tranScreen->display("4 - $100" );
            tranScreen->display("5 - $200" );
            tranScreen->display("6 - Cancel transaction" );
            tranScreen->display("\nChoose a withdrawal option (1-6): ");

            int inp = keypad.getInput();

            switch ( inp ){
                case 1: case 2: case 3: case 4: case 5: 
                    ch = amounts[ inp ];
                    break;
                case CANCELLED: 
                  ch = CANCELLED; 
                    break;
                default:
                    tranScreen->display("\nInvalid selection. Try again." );
                    break;
            }
        }

    return ch ;
    }


};

class Deposit : public Transaction {
    double amount;
    Keypad keypad;
    DepositSlot depositSlot;
    const int CANCELLED = 0 ;
    double InputDepositAmount(){
        tranScreen->show("Please Enter a Deposit Amount : " );
        int inp = keypad.getInput();
        if (inp == CANCELLED)
            return CANCELLED ;
        else 
            return inp ;
        
    }

public:
    Deposit( int AccountNumber, Screen* atmScreen, BankDatabase* atmBankDatabase, Keypad atmKeypad,DepositSlot atmDepositSlot )
    :Transaction( AccountNumber, atmScreen, atmBankDatabase )
    {
        keypad = atmKeypad ;
        depositSlot = atmDepositSlot ;
    }
    
    void execute(){
        amount  = InputDepositAmount();
        if (amount != CANCELLED ){
            tranScreen->show("Please insert the deposit envelope containing ");
            tranScreen->displayNum(amount);
            tranScreen->display(" in the deposit slot. \n");

            bool envelopRec = depositSlot.isEnvelopRecieved();

            if (envelopRec){
                tranScreen->display("\nYour envelope has been received.\nThe money just deposited will not be available until we verify the amount of any enclosed cash, and any enclosed checks clear." );
                db->credit(accountNumber,amount);
            }
            else {
                tranScreen->display("\nYou did not insert an envelope, so the ATM has canceled your transaction." );

            }
        }
        else
        {
            tranScreen->display( "\nCanceling transaction..." );
        }
   
    }



};

class BalanceInquiry : public Transaction {
    public:
        BalanceInquiry(int accNum , Screen* sc , BankDatabase* DB)
        : Transaction (accNum , sc , DB) {}

        void execute(){
            double totalBalance = db->getBalance( accountNumber );// display the balance information on the screen
            tranScreen->show( "\nBalance Information:\n - Total Balance: $" );
            tranScreen->displayNum(totalBalance);
        }


};



class ATM {
    
        bool authenticated;
        int accountNumber;
        Screen* screen;
        Keypad* keypad;
        CashDispenser* cashDispenser;
        DepositSlot* depositSlot;
        BankDatabase* bankDatabase;

        enum menu {
            WITHDRAW = 2,
            DEPOSIT = 3,
            BALANCE_INQUIRY = 1 ,
            EXIT = 4 
        };


    public:
        ATM(){
            authenticated = false;
            accountNumber = 0;
            screen  = new Screen();
            cashDispenser = new CashDispenser(); 
            depositSlot = new DepositSlot(); 
            bankDatabase = new BankDatabase();
            keypad = new Keypad();
                      
        }

        void start(){
            while(1){
                while(!authenticated){
                    screen->display("Welcome\n");
                    authenticateUser();
                }
            
            
            doTransactions();
            authenticated = false;
            accountNumber = 0 ;
            screen->cls();
            screen->display("\n\nThankyou! Good bye\n\n");
            
            }
        }

        void authenticateUser(){
            screen->display("Enter your Account Number : ");
            int accNum = keypad->getInput();
            screen->display("Enter your PIN : ");
            int pin = keypad->getInput();

            authenticated = bankDatabase->authenticateUser(accNum,pin);

            if (authenticated){
                accountNumber = accNum;
            }else {
                screen->display("\nInvalid account number or PIN , Please try again. \n");
            }
            
        }

        void doTransactions(){
            Transaction* transaction = nullptr ;
            bool exitt = false;

            while (!exitt){
                int mainMenuSelection = displayMainMenu();// decide how to proceed based on user's menu selection
                switch (mainMenuSelection){
                        case BALANCE_INQUIRY:
                        case WITHDRAW:
                        case DEPOSIT:// initialize as new object of chosen type
                        transaction = createTransaction( mainMenuSelection );
                        transaction->execute(); // execute transaction
                        break;
                        case EXIT: // user chose to terminate session
                            screen->display( "\nExiting the system..." );
                            exitt = true; // this ATM session should end
                            exit(1);
                            break;
                        default: // user did not enter an integer from 1-4
                        screen->display("\nYou did not enter a valid selection. Try again.\n" );
                        break;
                } 
            }            

        }

        int displayMainMenu(){
        
            screen->display( "\nMain menu:" );
            screen->display( "1 - View my balance" );
            screen->display( "2 - Withdraw cash" );
            screen->display( "3 - Deposit funds" );
            screen->display( "4 - Exit\n" );
            screen->display( "Enter a choice: " );
            return keypad->getInput();
            
        }

        Transaction* createTransaction(int x){
            Transaction* transaction = nullptr;
            switch (x)
            {
            case BALANCE_INQUIRY:
                transaction = new BalanceInquiry( accountNumber, screen, bankDatabase);
                break;
            case WITHDRAW:
                transaction =  new Withdrawal( accountNumber, screen,bankDatabase, *keypad, *cashDispenser);
                break;

            case DEPOSIT:
                transaction = new Deposit( accountNumber, screen, bankDatabase, *keypad, *depositSlot);
                break;

            }

            return transaction;
        }
     
};

int main (){
    cout<<"----------------------------------------------------------------------------------------------------"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-     -------------------------------------                                                        -"<<endl;
    cout<<"-     -welcome to ATM                     -                                                        -"<<endl;
    cout<<"-     -1.View Your Balance                -                                                        -"<<endl;
    cout<<"-     -2.With Draw Cash                   -                                                        -"<<endl;
    cout<<"-     -3.Deposit Cash                     -                                                        -"<<endl;
    cout<<"-     -4.Exit                             -                                                        -"<<endl;
    cout<<"-     -                                   -                                                        -"<<endl;
    cout<<"-     -------------------------------------                                                        -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-                                                      ------------------------------              -"<<endl;
    cout<<"-      1            2            3                     Enter cash Envelop here                     -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-      4            5            6                                                                 -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-      7            8            9                                                                 -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-                   0             Enter                --------------------------------            -"<<endl;
    cout<<"-                                                          Take cash from here                     -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"-                                                                                                  -"<<endl;
    cout<<"----------------------------------------------------------------------------------------------------"<<endl;  
    ATM* atm = new ATM();
    atm->start();
}