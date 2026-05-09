#include <stdio.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100

struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void initializeFile(FILE *fPtr);
void searchRecord(FILE *fPtr);
void checkBalance(FILE *fPtr);

int main(void) {
    FILE *cfPtr;
    unsigned int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL) {
            printf("File could not be created.\n");
            exit(1);
        }
        initializeFile(cfPtr);
    }

    while ((choice = enterChoice()) != 8) {
        switch (choice) {
            case 1: textFile(cfPtr); break;
            case 2: updateRecord(cfPtr); break;
            case 3: newRecord(cfPtr); break;
            case 4: deleteRecord(cfPtr); break;
            case 5: displayAll(cfPtr); break;
            case 6: searchRecord(cfPtr); break;
            case 7: checkBalance(cfPtr); break;
            default: printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

void initializeFile(FILE *fPtr) {
    struct clientData blank = {0, "", "", 0.0};
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        fwrite(&blank, sizeof(struct clientData), 1, fPtr);
    }
}

void textFile(FILE *readPtr) {
    FILE *writePtr;
    struct clientData client;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        printf("File could not be opened.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1) {
        if (client.acctNum != 0) {
            fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName,
                    client.firstName, client.balance);
        }
    }

    fclose(writePtr);
    printf("accounts.txt file created successfully.\n");
}

void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client;

    printf("Enter account number (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS) {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1) {
        printf("Error reading record.\n");
        return;
    }

    if (client.acctNum == 0) {
        printf("Account does not exist.\n");
        return;
    }

    printf("Current Balance: %.2f\n", client.balance);

    printf("Enter deposit (+) or withdrawal (-): ");
    scanf("%lf", &transaction);

    if (client.balance + transaction < 0) {
        printf("Insufficient balance.\n");
        return;
    }

    client.balance += transaction;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated Balance: %.2f\n", client.balance);
}

// 🔥 UPDATED FUNCTION
void newRecord(FILE *fPtr) {
    unsigned int account;
    struct clientData client;

    printf("Enter new account number (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS) {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1) {
        printf("Error reading record.\n");
        return;
    }

    // ✅ FIXED LOGIC
    if (client.acctNum != 0) {
        printf("Account already exists.\n");
        return;
    }

    // ✅ CLEAN INPUT
    printf("Enter Last Name: ");
    scanf(" %14s", client.lastName);

    printf("Enter First Name: ");
    scanf(" %9s", client.firstName);

    printf("Enter Balance: ");
    scanf(" %lf", &client.balance);

    client.acctNum = account;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1) {
        printf("Error writing record.\n");
        return;
    }

    printf("Account created successfully.\n");
}

void deleteRecord(FILE *fPtr) {
    unsigned int account;
    struct clientData client;
    struct clientData blank = {0, "", "", 0.0};

    printf("Enter account number to delete (1-100): ");
    scanf("%u", &account);

    if (account < 1 || account > MAX_ACCOUNTS) {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully.\n");
}

void displayAll(FILE *fPtr) {
    struct clientData client;

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

void searchRecord(FILE *fPtr) {
    unsigned int account;
    struct clientData client;

    printf("Enter account number to search: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
        printf("Account not found.\n");
    else {
        printf("\nAccount Found!\n");
        printf("Name: %s %s\n", client.firstName, client.lastName);
        printf("Balance: %.2f\n", client.balance);
    }
}

void checkBalance(FILE *fPtr) {
    unsigned int account;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
        printf("Account does not exist.\n");
    else
        printf("Current Balance: %.2f\n", client.balance);
}

unsigned int enterChoice(void) {
    unsigned int choice;

    printf("\n=============================\n");
    printf(" BANK MANAGEMENT SYSTEM\n");
    printf("=============================\n");
    printf("1 - Create text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Display all accounts\n");
    printf("6 - Search account\n");
    printf("7 - Check balance\n");
    printf("8 - Exit\n");
    printf("=============================\n");
    printf("Enter your choice: ");

    scanf("%u", &choice);
    return choice;
}