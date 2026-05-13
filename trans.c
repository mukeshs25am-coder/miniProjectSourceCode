// Advanced Bank Account Management System
// Improved version of the original random-access banking program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 100
#define DATA_FILE "credit.dat"
#define TEXT_FILE "accounts.txt"
#define LOG_FILE "bank.log"

// structure definition
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
    char phone[15];
    char email[40];
    char lastUpdated[30];
};

// function prototypes
unsigned int enterChoice(void);

void initializeFile(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);
void displayAll(FILE *fPtr);

void logAction(const char *action);
void getCurrentTime(char *buffer);

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    initializeFile();

    if ((cfPtr = fopen(DATA_FILE, "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        exit(EXIT_FAILURE);
    }

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        case 5:
            searchRecord(cfPtr);
            break;

        case 6:
            displayAll(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
            break;
        }
    }

    fclose(cfPtr);

    printf("Program terminated.\n");

    return 0;
}

// initialize file with blank records
void initializeFile(void)
{
    FILE *file;

    if ((file = fopen(DATA_FILE, "rb")) == NULL)
    {
        file = fopen(DATA_FILE, "wb");

        if (file == NULL)
        {
            printf("Unable to create file.\n");
            exit(EXIT_FAILURE);
        }

        struct clientData blankClient = {0, "", "", 0.0, "", "", ""};

        for (int i = 0; i < MAX_ACCOUNTS; i++)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, file);
        }

        printf("Database initialized successfully.\n");
    }

    fclose(file);
}

// menu
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n========== BANK MENU ==========\n");
    printf("1 - Generate text report\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Search account\n");
    printf("6 - Display all accounts\n");
    printf("7 - Exit\n");
    printf("Enter your choice: ");

    scanf("%u", &choice);

    return choice;
}

// generate formatted text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;

    if ((writePtr = fopen(TEXT_FILE, "w")) == NULL)
    {
        printf("File could not be created.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s %-15s %-10s %-10s %-15s %-30s %-20s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance",
            "Phone",
            "Email",
            "Last Updated");

    struct clientData client;

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u %-15s %-10s %-10.2f %-15s %-30s %-20s\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance,
                    client.phone,
                    client.email,
                    client.lastUpdated);
        }
    }

    fclose(writePtr);

    printf("accounts.txt generated successfully.\n");

    logAction("Generated accounts.txt");
}

// update record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client;

    printf("Enter account number (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    printf("\nCurrent Balance: %.2f\n", client.balance);

    printf("Enter amount (+ deposit / - withdrawal): ");
    scanf("%lf", &transaction);

    client.balance += transaction;

    getCurrentTime(client.lastUpdated);

    fseek(fPtr,
          -sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated Balance: %.2f\n", client.balance);

    logAction("Updated account");
}

// add new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, "", "", ""};

    unsigned int accountNum;

    printf("Enter new account number (1-100): ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    client.acctNum = accountNum;

    printf("Enter last name: ");
    scanf("%14s", client.lastName);

    printf("Enter first name: ");
    scanf("%9s", client.firstName);

    printf("Enter balance: ");
    scanf("%lf", &client.balance);

    printf("Enter phone number: ");
    scanf("%14s", client.phone);

    printf("Enter email: ");
    scanf("%39s", client.email);

    getCurrentTime(client.lastUpdated);

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully.\n");

    logAction("Created new account");
}

// delete record
void deleteRecord(FILE *fPtr)
{
    unsigned int accountNum;

    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0, "", "", ""};

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully.\n");

    logAction("Deleted account");
}

// search account
void searchRecord(FILE *fPtr)
{
    unsigned int accountNum;

    struct clientData client;

    printf("Enter account number to search: ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\n========== ACCOUNT DETAILS ==========\n");

    printf("Account Number : %u\n", client.acctNum);
    printf("Last Name      : %s\n", client.lastName);
    printf("First Name     : %s\n", client.firstName);
    printf("Balance        : %.2f\n", client.balance);
    printf("Phone          : %s\n", client.phone);
    printf("Email          : %s\n", client.email);
    printf("Last Updated   : %s\n", client.lastUpdated);
}

// display all accounts
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n================ ALL ACCOUNTS ================\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-5u %-15s %-10s %-10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

// log actions
void logAction(const char *action)
{
    FILE *logFile;

    logFile = fopen(LOG_FILE, "a");

    if (logFile == NULL)
    {
        return;
    }

    char timeBuffer[30];

    getCurrentTime(timeBuffer);

    fprintf(logFile,
            "[%s] %s\n",
            timeBuffer,
            action);

    fclose(logFile);
}

// get current date and time
void getCurrentTime(char *buffer)
{
    time_t now = time(NULL);

    struct tm *t = localtime(&now);

    strftime(buffer,
             30,
             "%Y-%m-%d %H:%M:%S",
             t);
}