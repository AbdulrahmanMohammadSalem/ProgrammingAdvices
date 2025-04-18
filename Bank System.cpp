//Made by Abdulrahman Mohammad Salem

#pragma warning(disable: 4996)

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

const string clientsFileName = "Clients.txt";
const string usersFileName = "Users.txt";

enum enMainMenuOptions {
    eShowClientList = 1, eAddNewClient = 2, eDeleteClient = 3, eUpdateClient = 4,
    eFindClient = 5, eTransactions = 6, eShowActiveUser = 7,eManageUsers = 8, eLogout = 9
};

enum enManageUsersMenuOptions {
    eShowUsersList = 1, eAddNewUser = 2, eDeleteUser = 3,
    eUpdateUser = 4, eFindUser = 5, eMainMenuFromManage = 6
};

enum enTransactionsMenuOptions {
    eDeposit = 1, eWithdraw = 2, eTotalBalances = 3, eMainMenuFromTransactions = 4
};

struct sClient {
    string accountNumber = "";
    string pinCode = "";
    string name = "";
    string phone = "";
    string dateCreated;
    string dateModified = "-";
    double accountBalance = 0;
    bool markForDelete = false;
};

struct sUser {
    string username = "";
    string password = "";
    short permissions = 0;
    string dateCreated;
    string dateModified = "-";
    bool markForDelete = false;
};

vector<sClient> vClients;
vector<sUser> vUsers;

sUser activeUser;

void displayMainMenuScreen();
void displayTransactionsMenuScreen();
void displayLoginScreen();
void displayManageUsersScreen();

short readNumberInRange(const string message, const short from, const short to) {
    short number = 0;

    while (true) {
        cout << message;
        cin >> number;

        if (cin.fail() || cin.peek() != '\n') {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

        } else if (number >= from && number <= to)
            return number;
    }
}

string readWord(const string message) {
    string str;

    while (true) {
        cout << message;
        cin >> str;

        if (cin.peek() != '\n')
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        else
            return str;
    }
}

string readName(string message) {
    string name = "";

    do {
        cout << message;
        getline(cin >> ws, name);
        message = "The name is too long, enter a shorter name: ";
    } while (name.length() > 28);

    return name;
}

char readCharacter(const string msg) {
    char chr = ' ';

    while (true) {
        cout << msg;
        cin >> chr;

        if (cin.fail() || cin.peek() != '\n') {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else
            return chr;
    }
}

double readPositiveNumber(const string message) {
    float number;

    while (true) {
        cout << message;
        cin >> number;

        if (cin.fail() || cin.peek() != '\n') {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

        } else if (number > 0)
            return number;
    }
}

string getSystemDateTime() {
    time_t timeNow = time(0);
    tm * timeNowStruct = localtime(&timeNow);
    ostringstream result;

    result << timeNowStruct->tm_mday << '/';
    result << timeNowStruct->tm_mon + 1 << '/';
    result << timeNowStruct->tm_year + 1900 << ' ';
    result << setw(2) << setfill('0') << timeNowStruct->tm_hour << ':';
    result << setw(2) << setfill('0') << timeNowStruct->tm_min << ':';
    result << setw(2) << setfill('0') << timeNowStruct->tm_sec;

    return result.str();
}

void printMainMenuScreen() {
    cout << "==================================================";
    cout << "\n                Main Menu Screen";
    cout << "\n==================================================";
    cout << "\n\t[1]: Show Clients List.";
    cout << "\n\t[2]: Add new Client.";
    cout << "\n\t[3]: Delete Client.";
    cout << "\n\t[4]: Update Client Info.";
    cout << "\n\t[5]: Find Client.";
    cout << "\n\t[6]: Transactions.";
    cout << "\n\t[7]: Show Active User.";
    cout << "\n\t[8]: Manage Users.";
    cout << "\n\t[9]: Logout.";
    cout << "\n==================================================\n";
    
}

vector<string> copyFileToVector(const string & fileName) {
    fstream file(fileName, ios::in);
    vector<string> vector;

    if (file.is_open()) {
        string currentLine;

        while (getline(file, currentLine))
            if (currentLine != "")
                vector.push_back(currentLine);

        file.close();
    }

    return vector;
}

vector<string> splitStringToVector(string & line, const string delimiter) {
    unsigned short delimiterPosition;
    vector<string> vector;
    string partFromLine;

    while ((delimiterPosition = line.find(delimiter)) != (unsigned short) string::npos) {
        partFromLine = line.substr(0, delimiterPosition);
        vector.push_back(partFromLine);

        line.erase(0, delimiterPosition + delimiter.length());
    }

    if (line != "")
        vector.push_back(line);

    return vector;
}

sClient fillClientRecordFromLine(string & line) {
    vector<string> vClientData = splitStringToVector(line, "#//#");
    sClient client;

    client.accountNumber = vClientData.at(0);
    client.pinCode = vClientData.at(1);
    client.name = vClientData.at(2);
    client.phone = vClientData.at(3);
    client.accountBalance = stod((vClientData.at(4)));
    client.dateCreated = vClientData.at(5);
    client.dateModified = vClientData.at(6);

    return client;
}

vector<sClient> convertLinesToClients(vector<string> vLines) {
    vector<sClient> vClients;
    sClient clientToAdd;

    for (string & currentLine : vLines) {
        clientToAdd = fillClientRecordFromLine(currentLine);

        vClients.push_back(clientToAdd);
    }

    return vClients;
}

vector<sClient> loadClientsFromFile(const string fileName) {
    return convertLinesToClients(copyFileToVector(fileName));;
}

void backToMainMenuScreen() {
    cout << "\nPress any key to return to main menu screen...";
    system("pause>nul");

    displayMainMenuScreen();
}

void printClientsTableHeader() {
    cout << "\n-----------------------------------------------------------------------------------------------------------------------------------------------";
    cout << "\n|  Account Number  ";
    cout << "|  PIN Code  ";
    cout << "|         Client Name         ";
    cout << "|     Phone     ";
    cout << "|       Balance       ";
    cout << "|    Date Created    ";
    cout << "|   Date Modified    |";
    cout << "\n-----------------------------------------------------------------------------------------------------------------------------------------------" << endl;
}

void printClientsTable() {
    for (sClient & currentClient : vClients) {
        cout << "| " << left << setw(17) << currentClient.accountNumber;
        cout << "| " << left << setw(11) << currentClient.pinCode;
        cout << "| " << left << setw(28) << currentClient.name;
        cout << "| " << left << setw(14) << currentClient.phone;
        cout << "| " << left << setw(20) << currentClient.accountBalance;
        cout << "| " << left << setw(19) << currentClient.dateCreated;
        cout << "| " << left << setw(19) << (currentClient.dateModified == "-" ? "        -" : currentClient.dateModified) << "|\n";
    }
    cout << "-----------------------------------------------------------------------------------------------------------------------------------------------" << endl;
}

void displayClientsListScreen() {
    system("cls");

    if (!vClients.empty()) {
        cout << "\t\t\t\t\t\tClients List - " << vClients.size() << " Client(s) available";

        printClientsTableHeader();

        printClientsTable();
    } else
        cout << "No clients available.\n";

    backToMainMenuScreen();
}

bool doesAccountNumberExist(const string & accountNumber) {
    for (sClient & currentClient : vClients)
        if (currentClient.accountNumber == accountNumber)
            return true;

    return false;
}

void readClientRecord(sClient & client) {
    client.pinCode = readWord("Enter PIN code       : ");
    client.name = readName("Enter name           : ");
    client.phone = readWord("Enter phone          : ");
    client.accountBalance = readPositiveNumber("Enter account balance: ");
}

string convertClientRecordToLine(const sClient & client, const string delimiter) {
    string result = "";

    result += client.accountNumber + delimiter;
    result += client.pinCode + delimiter;
    result += client.name + delimiter;
    result += client.phone + delimiter;
    result += to_string(client.accountBalance) + delimiter;
    result += client.dateCreated + delimiter;
    result += client.dateModified;

    return result;
}

void appendClientRecordToFile(const string & fileName, sClient & client) {
    fstream clientsFile(fileName, ios::app);

    if (clientsFile.is_open()) {
        clientsFile << convertClientRecordToLine(client, "#//#") << endl;

        clientsFile.close();
    }
}

void displayNewClientScreen() {
    char shouldAddNewClient = 'Y';

    while (shouldAddNewClient == 'Y' || shouldAddNewClient == 'y') {
        system("cls");
        cout << "* You can enter [-99] to Cancel.\n\n";
        cout << "---------------------------------\n";
        cout << "      Add New Client Screen\n";
        cout << "---------------------------------\n";
        cout << "Adding New Client:\n";

        string newAccountNumber, errorMessage = "";

        do {
            cout << errorMessage << endl;
            newAccountNumber = readWord("Enter account number : ");

            if (newAccountNumber == "-99")
                backToMainMenuScreen();

            errorMessage = "Account number [" + newAccountNumber + "] already exists.\n";
        } while (doesAccountNumberExist(newAccountNumber));

        sClient newClient;

        newClient.accountNumber = newAccountNumber;

        readClientRecord(newClient);
        newClient.dateCreated = getSystemDateTime();
        

        vClients.push_back(newClient);

        appendClientRecordToFile(clientsFileName, newClient);

        cout << "\nClient added successfully.\n";
        shouldAddNewClient = readCharacter("Do you want to add a new client? [Y] - [N]: ");
    }

    backToMainMenuScreen();
}

void printAccountRecord(const sClient & client) {
    cout << "\nThe following are the client's details:";
    cout << "\n-------------------------------------";
    cout << "\nAccount Number  :" << client.accountNumber;
    cout << "\nPIN Code        :" << client.pinCode;
    cout << "\nName            :" << client.name;
    cout << "\nPhone           :" << client.phone;
    cout << "\nAccount Balance :" << client.accountBalance;
    cout << "\n-------------------------------------\n";
}

short getClientPosition(const string & accountNumber) {
    for (short i = 0; i < vClients.size(); i++)
        if (vClients[i].accountNumber == accountNumber)
            return i;

    return -1;
}

short readClientPosition() {
    string accountNumber, errorMessage = "";
    short clientPosition;

    do {
        cout << errorMessage;

        accountNumber = readWord("Enter account number: ");

        if (accountNumber == "-99")
            backToMainMenuScreen();

        clientPosition = getClientPosition(accountNumber);

        errorMessage = "Account Number [" + accountNumber + "] does not exist.\n\n";
    } while (clientPosition == -1);

    return clientPosition;
}

void copyVectorToFile(const string & fileName, vector<string> vector) {
    fstream clientsFile(fileName, ios::out);

    if (clientsFile.is_open()) {
        for (string & currentLine : vector)
            clientsFile << currentLine << endl;

        clientsFile.close();
    }
}

void saveClientsToFile(const string fileName) {
    vector<string> vClientsLines;

    for (sClient & currentClientRecord : vClients)
        if (!currentClientRecord.markForDelete)
            vClientsLines.push_back(convertClientRecordToLine(currentClientRecord, "#//#"));

    copyVectorToFile(fileName, vClientsLines);
}

void displayDeleteClientScreen() {
    system("cls");

    if (vClients.empty())
        cout << "No clients available.\n";
    else {
        char shouldDeleteClient = 'Y';

        while (shouldDeleteClient == 'Y' || shouldDeleteClient == 'y') {
            system("cls");

            cout << "* You can enter [-99] to Cancel.\n\n";
            cout << "---------------------------------\n";
            cout << "      Delete Client Screen\n";
            cout << "---------------------------------\n";

            short clientToDeletePosition = readClientPosition();

            printAccountRecord(vClients[clientToDeletePosition]);

            shouldDeleteClient = readCharacter("\nAre you sure you want to delete this client? [Y] - [N]: ");

            if (shouldDeleteClient == 'Y' || shouldDeleteClient == 'y') {
                vClients[clientToDeletePosition].markForDelete = true;

                saveClientsToFile(clientsFileName);
                vClients = loadClientsFromFile(clientsFileName); //To refresh the vector

                cout << "\nClient deleted successfully.\n";

                shouldDeleteClient = readCharacter("Do you want to delete another client? [Y] - [N]: ");
            }
        }
    }

    backToMainMenuScreen();
}

void displayUpdateClientScreen() {
    system("cls");

    if (vClients.empty())
        cout << "No clients available.\n";
    else {
        char shouldUpdateClient = 'Y';

        while (shouldUpdateClient == 'Y' || shouldUpdateClient == 'y') {
            system("cls");

            cout << "* You can enter [-99] to Cancel.\n\n";
            cout << "---------------------------------\n";
            cout << "    Update Client Info Screen\n";
            cout << "---------------------------------\n";

            short clientToUpdatePosition = readClientPosition();

            printAccountRecord(vClients[clientToUpdatePosition]);

            shouldUpdateClient = readCharacter("\nAre you sure you want to update this client? [Y] - [N]: ");

            if (shouldUpdateClient == 'Y' || shouldUpdateClient == 'y') {
                cout << '\n';
                readClientRecord(vClients[clientToUpdatePosition]);
                vClients[clientToUpdatePosition].dateModified = getSystemDateTime();

                saveClientsToFile(clientsFileName);

                cout << "\nClient updated successfully.\n";
                shouldUpdateClient = readCharacter("Do you want to update another client? [Y] - [N]: ");
            }
        }
    }

    backToMainMenuScreen();
}

void displayFindClientScreen() {
    system("cls");

    if (vClients.empty())
        cout << "No clients available.\n";
    else {
        char shouldFindClient = 'Y';

        while (shouldFindClient == 'Y' || shouldFindClient == 'y') {
            system("cls");

            cout << "* You can enter [-99] to Cancel.\n\n";
            cout << "---------------------------------\n";
            cout << "       Find Client Screen\n";
            cout << "---------------------------------\n";

            printAccountRecord(vClients[readClientPosition()]);

            cout << '\n';
            shouldFindClient = readCharacter("Do you want to find another client? [Y] - [N]: ");
        }
    }

    backToMainMenuScreen();
}

void printTransactionsMenu() {
    cout << "==================================================";
    cout << "\n           Transactions Menu Screen";
    cout << "\n==================================================";
    cout << "\n\t[1]: Deposit.";
    cout << "\n\t[2]: Withdraw.";
    cout << "\n\t[3]: Total Balances.";
    cout << "\n\t[4]: Main Menu.";
    cout << "\n==================================================" << endl;
}

bool shouldPerformTransaction() {
    char shouldPerform = ' ';

    shouldPerform = readCharacter("\nAre you sure you want to perform this transaction? [Y] - [N]: ");

    return shouldPerform == 'Y' || shouldPerform == 'y';
}

void backToTransactionsScreen() {
    cout << "\nPress any key to return to transactions menu screen...";
    system("pause>nul");

    displayTransactionsMenuScreen();
}

void depositAmount(sClient & client, const double amount) {
    if (shouldPerformTransaction()) {
        client.accountBalance += amount;
        saveClientsToFile(clientsFileName);

        cout << "\nDone successfully, new balance is: " << client.accountBalance;
    }
}

void displayDepositScreen() {
    system("cls");

    cout << "* You can enter [-99] to Cancel.\n\n";
    cout << "---------------------------------\n";
    cout << "         Deposit Screen\n";
    cout << "---------------------------------\n";

    short clientPosition = readClientPosition();
    sClient & client = vClients[clientPosition];

    printAccountRecord(client);

    double amount = readPositiveNumber("Enter amount: ");

    depositAmount(client, amount);

    backToTransactionsScreen();
}

void displayWithdrawScreen() {
    system("cls");

    cout << "* You can enter [-99] to Cancel.\n\n";
    cout << "---------------------------------\n";
    cout << "         Withdraw Screen\n";
    cout << "---------------------------------\n";

    short clientPosition = readClientPosition();
    sClient & client = vClients[clientPosition];

    printAccountRecord(client);

    //Validates if the account balance is zero
    if (client.accountBalance == 0) {
        cout << "You cannot withdraw.\n";
        backToTransactionsScreen();
        return;
    }

    double amount = readPositiveNumber("Enter amount: ");

    //Validates if the amount exceeds the balance
    if (amount > client.accountBalance)
        do {
            cout << "Amount exceeds balance, you can withdraw up to " << client.accountBalance << "\n\n";
            amount = readPositiveNumber("Enter amount: ");
        } while (amount > client.accountBalance);

    depositAmount(client, amount * -1);

    backToTransactionsScreen();
}

void printBalances() {
    //Header:
    cout << "\n------------------------------------------------------------------------------------------------";
    cout << "\n|    Account Number    ";
    cout << "|              Client Name              ";
    cout << "|            Balance            |";
    cout << "\n------------------------------------------------------------------------------------------------" << endl;

    //Data:
    for (sClient & currentClient : vClients) {
        cout << "| " << left << setw(21) << currentClient.accountNumber;
        cout << "| " << left << setw(38) << currentClient.name;
        cout << "| " << left << setw(30) << currentClient.accountBalance << "|\n";
    }
    cout << "------------------------------------------------------------------------------------------------" << endl;
}

double getTotalBalances() {
    double sum = 0;

    for (sClient & C : vClients)
        sum += C.accountBalance;

    return sum;
}

void displayTotalBalancesScreen() {
    system("cls");
    
    cout << "\t\t\t\tBalances list - " << vClients.size() << " Clients available";

    printBalances();

    cout << "\t\t\t\t\tTotal Balances = " << getTotalBalances() << endl << endl;
   
    backToTransactionsScreen();
}

void displayTransactionsMenuScreen() {
    system("cls");

    if (vClients.empty()) {
        cout << "No clients available.\n";
        backToMainMenuScreen();
    } else {
        printTransactionsMenu();

        enTransactionsMenuOptions transactionMenuOption = (enTransactionsMenuOptions) readNumberInRange("Choose [1] to [4]: ", 1, 4);

        switch (transactionMenuOption) {
            case enTransactionsMenuOptions::eDeposit:
                displayDepositScreen();
                break;
            case enTransactionsMenuOptions::eWithdraw:
                displayWithdrawScreen();
                break;
            case enTransactionsMenuOptions::eTotalBalances:
                displayTotalBalancesScreen();
                break;
            case enTransactionsMenuOptions::eMainMenuFromTransactions:
                displayMainMenuScreen();
        }
    }
}

short power(const short & a, short b) {
    short result = 1;

    for (b; b > 0; b--)
        result *= a;

    return result;
}

bool doesUserHavePermission(const sUser & user, const enMainMenuOptions mainMenuOption) {
    if (user.permissions == -1)
        return true;
    else
        return (user.permissions & power(2, (short) mainMenuOption - 1)) == power(2, (short) mainMenuOption - 1);
    
}

void displayAccessDeniedScreen() {
    system("cls");

    cout << "---------------------------------------\n";
    cout << "Access Denied,\n";
    cout << "You don't have permission to do this.\n";
    cout << "Please contact your Admin.\n";
    cout << "--------------------------------------\n";

    backToMainMenuScreen();
}

void printManageUsersScreen() {
    cout << "==================================================";
    cout << "\n               Manage Users Screen";
    cout << "\n==================================================";
    cout << "\n\t[1]: Show Users List.";
    cout << "\n\t[2]: Add New User.";
    cout << "\n\t[3]: Delete User.";
    cout << "\n\t[4]: Update User.";
    cout << "\n\t[5]: Find User.";
    cout << "\n\t[6]: Main Menu.";
    cout << "\n==================================================" << endl;
}

void printUsersTableHeader() {
    cout << "\n-------------------------------------------------------------------------------------------------------------";
    cout << "\n|        Username        ";
    cout << "|       Password       ";
    cout << "|   Permissions   ";
    cout << "|    Date Created    ";
    cout << "|    Date Modified   |";
    cout << "\n-------------------------------------------------------------------------------------------------------------" << endl;
}

void printUsersTable() {
    for (sUser & currentUser : vUsers) {
        cout << "| " << left << setw(23) << (currentUser.username + (currentUser.username == activeUser.username ? " (Active User)" : ""));
        cout << "| " << left << setw(21) << currentUser.password;
        cout << "| " << left << setw(16) << currentUser.permissions;
        cout << "| " << left << setw(19) << currentUser.dateCreated;
        cout << "| " << left << setw(19) << (currentUser.dateModified == "-" ? "        -" : currentUser.dateModified) << "|\n";
    }
    cout << "-------------------------------------------------------------------------------------------------------------" << endl;
}

void backToManageUsersScreen() {
    cout << "\nPress any key to return to manage users menu screen...";
    system("pause>nul");

    displayManageUsersScreen();
}

bool doesUsernameExist(const string & username) {
    for (sUser & currentUser : vUsers)
        if (currentUser.username == username)
            return true;
    
    return false;
}

void readUserRecord(sUser & user) {
    user.password = readWord("Enter password: ");

    if (tolower(readCharacter("Do you want to give full access? [Y] - [N]: ")) == 'y')
        user.permissions = -1;
    else {
        user.permissions = 0;
        cout << "\nGiving Access To:\n";

        if (tolower(readCharacter("  * Show Clients List [Y] - [N]: ")) == 'y')
            user.permissions += 1;

        if (tolower(readCharacter("  * Add new Client [Y] - [N]: ")) == 'y')
            user.permissions += 2;

        if (tolower(readCharacter("  * Delete Client [Y] - [N]: ")) == 'y')
            user.permissions += 4;

        if (tolower(readCharacter("  * Update Client Info [Y] - [N]: ")) == 'y')
            user.permissions += 8;

        if (tolower(readCharacter("  * Find Client [Y] - [N]: ")) == 'y')
            user.permissions += 16;

        if (tolower(readCharacter("  * Transactions [Y] - [N]: ")) == 'y')
            user.permissions += 32;

        if (tolower(readCharacter("  * Manage Users [Y] - [N]: ")) == 'y')
            user.permissions += 64;

        if (user.permissions == 127)
            user.permissions = -1;
    }
}

string convertUserRecordToLine(const sUser & userRecord, const string delimiter) {
    string result = "";

    result += userRecord.username + delimiter;
    result += userRecord.password + delimiter;
    result += to_string(userRecord.permissions) + delimiter;
    result += userRecord.dateCreated + delimiter;
    result += userRecord.dateModified;

    return result;
}

void appendUserRecordToFile(const string & fileName, const sUser & userRecord) {
    fstream clientsFile(fileName, ios::app);

    if (clientsFile.is_open()) {
        clientsFile << convertUserRecordToLine(userRecord, "#//#") << endl;

        clientsFile.close();
    }
}

short getUserPosition(const string & username) {
    short pos = 0;

    for (sUser & currentUser : vUsers) {
        if (currentUser.username == username)
            return pos;

        pos++;
    }

    return -1;
}

short readUserPosition(const enManageUsersMenuOptions menuOption) {
    string username, errorMessage = "";
    short userPosition = -1;

    do {
        cout << errorMessage;

        username = readWord("Enter username: ");

        if (username == "-99")
            backToManageUsersScreen();
        else if (username == "Admin") {
            if (menuOption == enManageUsersMenuOptions::eDeleteUser) {
                cout << "You cannot delete this user.\n\n";
                errorMessage = "";
                continue;
            } else if (menuOption == enManageUsersMenuOptions::eUpdateUser) {
                cout << "You cannot update this user.\n\n";
                errorMessage = "";
                continue;
            }
        }

        userPosition = getUserPosition(username);

        errorMessage = "Username [" + username + "] does not exist.\n\n";
    } while (userPosition == -1);

    return userPosition;
}

void printUserPermissions(const sUser & userRecord) {
    if (userRecord.permissions == -1)
        cout << " Full Access";
    else {
        if (doesUserHavePermission(userRecord, enMainMenuOptions::eShowClientList))
            cout << "\n  * Show Client List.";

        if (doesUserHavePermission(userRecord, enMainMenuOptions::eAddNewClient))
            cout << "\n  * Add New Client.";

        if (doesUserHavePermission(userRecord, enMainMenuOptions::eDeleteClient))
            cout << "\n  * Delete Client.";

        if (doesUserHavePermission(userRecord, enMainMenuOptions::eUpdateClient))
            cout << "\n  * Update Client Info.";

        if (doesUserHavePermission(userRecord, enMainMenuOptions::eFindClient))
            cout << "\n  * Find Client.";

        if (doesUserHavePermission(userRecord, enMainMenuOptions::eTransactions))
            cout << "\n  * Transactions.";

        if (doesUserHavePermission(userRecord, enMainMenuOptions::eManageUsers))
            cout << "\n  * Manage Users.";
    }
}

void printUserRecord(const sUser & userRecord) {
    cout << "\nThe following are the user's details:";
    cout << "\n-------------------------------------";
    cout << "\nUsername    : " << userRecord.username;

    if (userRecord.username == activeUser.username)
        cout << " (Active User)";

    cout << "\nPassword    : " << userRecord.password;
    cout << "\nPermissions :";
    printUserPermissions(userRecord);

    cout << "\n-------------------------------------" << endl;
}

void saveUsersToFile(const string & fileName) {
    vector<string> vUsersLines;

    for (sUser & currentUserRecord : vUsers)
        if (!currentUserRecord.markForDelete)
            vUsersLines.push_back(convertUserRecordToLine(currentUserRecord, "#//#"));

    copyVectorToFile(fileName, vUsersLines);
}

sUser fillUserRecordFromLine(string & line) {
    vector<string> vUserDate = splitStringToVector(line, "#//#");
    sUser user;

    user.username = vUserDate.at(0);
    user.password = vUserDate.at(1);
    user.permissions = stoi(vUserDate.at(2));
    user.dateCreated = vUserDate.at(3);
    user.dateModified = vUserDate.at(4);

    return user;
}

vector<sUser> convertLinesToUsers(vector<string> & vLines) {
    vector<sUser> vUsers;
    sUser userToAdd;

    for (string & currentLine : vLines) {
        userToAdd = fillUserRecordFromLine(currentLine);

        vUsers.push_back(userToAdd);
    }

    return vUsers;
}

vector<sUser> loadUsersFromFile(const string & fileName) {
    fstream usersFile(fileName, ios::app);

    if (usersFile.is_open()) {
        vector<string> vLines = copyFileToVector(fileName);

        if (vLines.empty()) {
            string adminLine = "Admin#//#1234#//#-1#//#" + getSystemDateTime() + "#//#-";
            usersFile << adminLine << endl;
            vLines.push_back(adminLine);
        }

        usersFile.close();
        return convertLinesToUsers(vLines);
    }

    return {{"Error", "Error", 0}};
}

short getUserPosition(const string & username, const string & password) {
    short pos = 0;

    for (sUser & currentUser : vUsers) {
        if (currentUser.username == username && currentUser.password == password)
            return pos;

        pos++;
    }

    return -1;
}

void displayUsersListScreen() {
    system("cls");

    cout << "\t\t\t\t\tUsers List - " << vUsers.size() << " User(s) available";

    printUsersTableHeader();

    printUsersTable();

    backToManageUsersScreen();
}

void displayNewUserScreen() {
    char shouldAddNewUser = 'Y';

    while (shouldAddNewUser == 'Y' || shouldAddNewUser == 'y') {
        system("cls");

        cout << "* You can enter [-99] to Cancel.\n\n";
        cout << "---------------------------------\n";
        cout << "       Add New User Screen       \n";
        cout << "---------------------------------\n";
        cout << "Adding New User:\n";

        string newUsername, errorMessage = "";

        do {
            cout << errorMessage << endl;
            newUsername = readWord("Enter username: ");

            if (newUsername == "-99")
                backToManageUsersScreen();

            errorMessage = "Username [" + newUsername + "] already exists.\n";
        } while (doesUsernameExist(newUsername));

        sUser newUser;

        newUser.username = newUsername;

        readUserRecord(newUser);
        newUser.dateCreated = getSystemDateTime();

        vUsers.push_back(newUser);

        appendUserRecordToFile(usersFileName, newUser);

        cout << "\nUser added successfully.\n";
        shouldAddNewUser = readCharacter("Do you want to add a new user? [Y] - [N]: ");
    }

    backToManageUsersScreen();
}

void displayDeleteUserScreen() {
    char shouldDeleteUser = 'Y';

    while (shouldDeleteUser == 'Y' || shouldDeleteUser == 'y') {
        system("cls");

        cout << "* You can enter [-99] to Cancel.\n\n";
        cout << "---------------------------------\n";
        cout << "       Delete User Screen\n";
        cout << "---------------------------------\n";

        short userToDeletePosition = readUserPosition(enManageUsersMenuOptions::eDeleteUser);
        bool isActiveUser = vUsers[userToDeletePosition].username == activeUser.username;

        printUserRecord(vUsers[userToDeletePosition]);

        if (isActiveUser) {
            cout << "\nThis is the active user, you will be logged out.\n";
            shouldDeleteUser = readCharacter("Confirm? [Y] - [N]: ");
        } else
            shouldDeleteUser = readCharacter("\nAre you sure you want to delete this user? [Y] - [N]: ");

        if (shouldDeleteUser == 'Y' || shouldDeleteUser == 'y') {
            vUsers[userToDeletePosition].markForDelete = true;

            saveUsersToFile(usersFileName);
            vUsers = loadUsersFromFile(usersFileName); //To refresh the vector

            cout << "\nUser deleted successfully.";

            if (isActiveUser) {
                cout << "\nPress any key to logout...";
                system("pause>nul");
                displayLoginScreen();
            }

            shouldDeleteUser = readCharacter("\nDo you want to delete another user? [Y] - [N]: ");
        }
    }

    backToManageUsersScreen();
}

void displayUpdateUserScreen() {
    short userToUpdatePosition = 0;
    char shouldUpdateUser = 'Y';
    bool isUserToUpdateActive = false;

    while (shouldUpdateUser == 'Y' || shouldUpdateUser == 'y') {
        system("cls");

        cout << "* You can enter [-99] to Cancel.\n\n";
        cout << "---------------------------------\n";
        cout << "     Update User Info Screen\n";
        cout << "---------------------------------\n";

        userToUpdatePosition = readUserPosition(enManageUsersMenuOptions::eUpdateUser);
        isUserToUpdateActive = vUsers[userToUpdatePosition].username == activeUser.username;

        printUserRecord(vUsers[userToUpdatePosition]);

        shouldUpdateUser = readCharacter("\nAre you sure you want to update this user? [Y] - [N]: ");

        if (shouldUpdateUser == 'Y' || shouldUpdateUser == 'y') {
            cout << '\n';

            readUserRecord(vUsers[userToUpdatePosition]);
            vUsers[userToUpdatePosition].dateModified = getSystemDateTime();

            saveUsersToFile(usersFileName);

            cout << "\nUser updated successfully.\n";

            if (isUserToUpdateActive) {
                activeUser = vUsers[userToUpdatePosition];

                if (!doesUserHavePermission(activeUser, enMainMenuOptions::eManageUsers)) {
                    cout << "You no longer have access to this menu.\n";
                    backToMainMenuScreen();
                }
            }

            shouldUpdateUser = readCharacter("Do you want to update another user? [Y] - [N]: ");
        }
    }
    
    backToManageUsersScreen();
}

void displayFindUserScreen() {
    char shouldFindNewUser = 'Y';

    while (shouldFindNewUser == 'Y' || shouldFindNewUser == 'y') {
        system("cls");

        cout << "* You can enter [-99] to Cancel.\n\n";
        cout << "---------------------------------\n";
        cout << "       Find Client Screen\n";
        cout << "---------------------------------\n";

        printUserRecord(vUsers[readUserPosition(enManageUsersMenuOptions::eFindUser)]);

        cout << '\n';
        shouldFindNewUser = readCharacter("Do you want to find a new user? [Y] - [N]: ");
    }
    
    backToManageUsersScreen();
}

void displayManageUsersScreen() {
    system("cls");
    printManageUsersScreen();

    enManageUsersMenuOptions manageUsersOption = (enManageUsersMenuOptions) readNumberInRange("Choose [1] to [6]: ", 1, 6);

    switch (manageUsersOption) {
        case enManageUsersMenuOptions::eShowUsersList:
            displayUsersListScreen();
            break;
        case enManageUsersMenuOptions::eAddNewUser:
            displayNewUserScreen();
            break;
        case enManageUsersMenuOptions::eDeleteUser:
            displayDeleteUserScreen();
            break;
        case enManageUsersMenuOptions::eUpdateUser:
            displayUpdateUserScreen();
            break;
        case enManageUsersMenuOptions::eFindUser:
            displayFindUserScreen();
            break;
        case enManageUsersMenuOptions::eMainMenuFromManage:
            displayMainMenuScreen();
    }
}

void displayActiveUserScreen() {
    system("cls");

    cout << "---------------------------------";
    cout << "\n       Active User Details";
    cout << "\n---------------------------------";
    cout << "\nUsername    : " << activeUser.username;
    cout << "\nPassword    : " << activeUser.password;
    cout << "\nPermissions :";

    printUserPermissions(activeUser);
    cout << "\n---------------------------------" << endl;

    backToMainMenuScreen();
}

void displayMainMenuScreen() {
    system("cls");
    printMainMenuScreen();

    enMainMenuOptions mainMenuOption = (enMainMenuOptions) readNumberInRange("Choose [1] to [9]: ", 1, 9);

    switch (mainMenuOption) {
        case enMainMenuOptions::eShowClientList:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eShowClientList))
                displayClientsListScreen();
            else
                displayAccessDeniedScreen();

            break;
        case enMainMenuOptions::eAddNewClient:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eAddNewClient))
                displayNewClientScreen();
            else
                displayAccessDeniedScreen();

            break;
        case enMainMenuOptions::eDeleteClient:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eDeleteClient))
                displayDeleteClientScreen();
            else
                displayAccessDeniedScreen();

            break;
        case enMainMenuOptions::eUpdateClient:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eUpdateClient))
                displayUpdateClientScreen();
            else
                displayAccessDeniedScreen();
            
            break;
        case enMainMenuOptions::eFindClient:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eFindClient))
                displayFindClientScreen();
            else
                displayAccessDeniedScreen();

            break;
        case enMainMenuOptions::eTransactions:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eTransactions))
                displayTransactionsMenuScreen();
            else
                displayAccessDeniedScreen();

            break;

        case enMainMenuOptions::eShowActiveUser:
            displayActiveUserScreen();
            break;

        case enMainMenuOptions::eManageUsers:
            if (doesUserHavePermission(activeUser, enMainMenuOptions::eManageUsers))
                displayManageUsersScreen();
            else
                displayAccessDeniedScreen();

            break;
        case enMainMenuOptions::eLogout:
            displayLoginScreen();
    }
}

void displayLoginScreen() {
    system("cls");

    cout << "---------------------------------\n";
    cout << "          Login Screen\n";
    cout << "---------------------------------\n";

    string username = "";
    string password = "";
    short currenUserPosition = 0;

    while (true) {
        username = readWord("Enter username:\n");
        password = readWord("Enter password:\n");

        currenUserPosition = getUserPosition(username, password);

        if (currenUserPosition != -1) {
            activeUser = vUsers.at(currenUserPosition);
            break;
        } else
            cout << "Invalid username/password\n\n";
    }
    
    displayMainMenuScreen();
}

int main() {
    vUsers = loadUsersFromFile(usersFileName);
    vClients = loadClientsFromFile(clientsFileName);

    displayLoginScreen();

    system("pause>nul");
    return 0;
}
