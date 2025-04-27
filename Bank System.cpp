//Created by Abdulrahman Mohammad Salem

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
const string settingsFileName = "Settings.txt";
const string historyFileName = "History.txt";

enum enMainMenuOptions {
    eManageClients = 1, eManageUsers = 2, eShowActiveUser = 3, eSettings = 4, eLogout = 5
};

enum enManageClientsMenuOptions {
    eShowClientList = 1, eAddNewClient = 2, eDeleteClient = 3,
    eUpdateClient = 4, eFindClient = 5, eTransactions = 6
};

enum enManageUsersMenuOptions {
    eShowUsersList = 1, eAddNewUser = 2, eDeleteUser = 3,
    eUpdateUser = 4, eFindUser = 5, eMainMenuFromManage = 6
};

enum enSettingsOptions {
    eClientsTableSettings = 1, eUsersTableSettings = 2, eTextColor = 3, eBackgroundColor = 4, eBackToMainMenu = 5
};

enum enClientTableFields {
    eAccountNumber, ePinCode, eName, ePhone, eAccountBalance, eDateCreatedClients, eDateModifiedClients,
};

enum enUserTableFields {
    eUsername, ePassword, ePermissions, eDateCreatedUsers, eDateModifiedUsers
};

enum enTextBackground {
    eBackground = 0, eText = 1
};

enum enTransactionsMenuOptions {
    eDeposit = 1, eWithdraw = 2, eTotalBalances = 3, eManageClientsScreenFromTransactions = 4
};

enum enWhereToGoBack {
    eMainMenuScreen, eManageClientsScreen, eManageUsersScreen
};

enum enClientUpdateType {
    eClientCreated, eFullUpdate, eOnlyDeposit, eOnlyWithdraw
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

struct sClientForHistory {
    string accountNumber = "";
    string pinCode = "";
    string name = "";
    string phone = "";
    string date = "";
    double accountBalance = 0;
    enClientUpdateType whatHappened;
};

struct sUser {
    string username = "";
    string password = "";
    short manageClientsPermissions = 0;
    short manageUsersPermissions = 0;
    string dateCreated;
    string dateModified = "-";
    bool markForDelete = false;
};

struct sSettings {
    short clientTableSettings = 0;
    short userTableSettings = 0;
    string programColors;
};

vector<sClient> vClients;
vector<sUser> vUsers;

sUser activeUser;
sSettings globalSettings;

void displayMainMenuScreen();
void displayTransactionsMenuScreen();
void displayLoginScreen();
void displayManageUsersScreen();
void displayManageClientsScreen();
void displaySettingsScreen();
void displayFindClientScreen();
void displayClientHistoryScreen(const string&);

short power(const short & a, short b) {
    short result = 1;

    for (b; b > 0; b--)
        result *= a;

    return result;
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

vector<string> splitStringToVector(string str, const char del) {
    str += del;
    vector<string> vctOfWrds;
    string wrd = "";

    for (unsigned short i = 0; i < str.length(); i++) {
        if (str[i] != del) {
            wrd += str[i];

            if (str[i + 1] == del) {
                vctOfWrds.push_back(wrd);
                wrd = "";
            }
        }
    }

    return vctOfWrds;
}

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

string getSettingsLine(const string & settingsFileName) {
    fstream settingsFile(settingsFileName, ios::in);
    string line = "";

    if (settingsFile.is_open()) {
        getline(settingsFile, line);
        settingsFile.close();
    }
    
    return line;
}

bool shouldClientFieldAppear(const short & clientTableSettings, const enClientTableFields & tableField) {
    if (clientTableSettings == -1)
        return true;

    short settingAsPower = power(2, (short) tableField);
    return (clientTableSettings & settingAsPower) == settingAsPower;
}

bool shouldUserFieldAppear(const short & userTableSettings, const enUserTableFields & tableField) {
    if (userTableSettings == -1)
        return true;

    short settingAsPower = power(2, (short) tableField);
    return (userTableSettings & settingAsPower) == settingAsPower;
}

sSettings loadSettingsFromFile(const string & settingsFileName) {
    fstream settingsFile(settingsFileName, ios::app);

    if (settingsFile.is_open()) {
        string settingsString = getSettingsLine(settingsFileName);
        
        if (settingsString == "") {
            settingsFile << "-1/-1/0F";
            return {-1, -1, "0F"};
        } else {
            vector<string> vSettings = splitStringToVector(settingsString, '/');
            sSettings settings;

            settings.clientTableSettings = stoi(vSettings[0]);
            settings.userTableSettings = stoi(vSettings[1]);
            settings.programColors = vSettings[2];

            settingsFile.close();
            return settings;
        }
    }

    return {0, 0, "0f"};
}

string convertSettingsToLine(const sSettings & settings) {
    return to_string(settings.clientTableSettings) + '/' + to_string(settings.userTableSettings) + '/' + settings.programColors;
}

void saveSettingsToFile(const sSettings & settings, const string & settingsFilePath) {
    fstream settingsFile(settingsFilePath, ios::out);

    if (settingsFile.is_open()) {
        settingsFile << convertSettingsToLine(settings);

        settingsFile.close();
    }
}

void printMainMenuScreen() {
    cout << "==================================================";
    cout << "\n                Main Menu Screen";
    cout << "\n==================================================";
    cout << "\n\t[1]: Manage Clients.";
    cout << "\n\t[2]: Manage Users.";
    cout << "\n\t[3]: Show Active User.";
    cout << "\n\t[4]: Settings.";
    cout << "\n\t[5]: Logout.";
    cout << "\n==================================================" << endl;
}

void printManageClientsMenu() {
    cout << "==================================================";
    cout << "\n              Manage Clients Screen";
    cout << "\n==================================================";
    cout << "\n\t[1]: Show Clients List.";
    cout << "\n\t[2]: Add new Client.";
    cout << "\n\t[3]: Delete Client.";
    cout << "\n\t[4]: Update Client Info.";
    cout << "\n\t[5]: Find Client.";
    cout << "\n\t[6]: Transactions.";
    cout << "\n\t[7]: Main Menu.";
    cout << "\n==================================================" << endl;
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
    cout << "\nPress any key to return to Main Menu Screen...";
    system("pause>nul");

    displayMainMenuScreen();
}

void backToManageClientsScreen() {
    cout << "\nPress any key to return to Manage Clients Menu Screen...";
    system("pause>nul");

    displayManageClientsScreen();
}

void backToManageUsersScreen() {
    cout << "\nPress any key to return to Manage Users Menu Screen...";
    system("pause>nul");

    displayManageUsersScreen();
}

void printDashes(short n) {
    for (n; n > 0; n--)
        cout << '-';
}

void printLineForClientsTable() {
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eAccountNumber))
        printDashes(19);
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::ePinCode))
        printDashes(13);
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eName))
        printDashes(30);
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::ePhone))
        printDashes(16);
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eAccountBalance))
        printDashes(22);
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eDateCreatedClients))
        printDashes(21);
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eDateModifiedClients))
        printDashes(21);

    cout << '-';
}

void printClientsTableHeader() {
    cout << '\n';
    printLineForClientsTable();
    cout << "\n|";

    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eAccountNumber))
        cout << "  Account Number  |";
    
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::ePinCode))
        cout << "  PIN Code  |";
    
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eName))
        cout << "         Client Name         |";
    
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::ePhone))
        cout << "     Phone     |";
    
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eAccountBalance))
        cout << "       Balance       |";
    
    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eDateCreatedClients))
        cout << "    Date Created    |";

    if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eDateModifiedClients))
        cout << "   Date Modified    |";

    cout << '\n';
    printLineForClientsTable();
    cout << endl;
}

void printClientsTable() {
    for (sClient & currentClient : vClients) {
        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eAccountNumber))
            cout << "| " << left << setw(17) << currentClient.accountNumber;

        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::ePinCode))
            cout << "| " << left << setw(11) << currentClient.pinCode;

        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eName))
            cout << "| " << left << setw(28) << currentClient.name;

        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::ePhone))
            cout << "| " << left << setw(14) << currentClient.phone;

        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eAccountBalance))
            cout << "| " << left << setw(20) << currentClient.accountBalance;

        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eDateCreatedClients))
            cout << "| " << left << setw(19) << currentClient.dateCreated;

        if (shouldClientFieldAppear(globalSettings.clientTableSettings, enClientTableFields::eDateModifiedClients))
            cout << "| " << left << setw(19) << (currentClient.dateModified == "-" ? "        -" : currentClient.dateModified);

        cout << "|\n";
    }

    printLineForClientsTable();
    cout << endl;
}

void displayClientsListScreen() {
    system("cls");

    if (!vClients.empty()) {
        cout << "* Clients List - " << vClients.size() << " Client(s) available";

        printClientsTableHeader();

        printClientsTable();
    } else
        cout << "No clients available.\n";

    backToManageClientsScreen();
}

bool doesAccountNumberExist(const string & accountNumber) {
    for (sClient & currentClient : vClients)
        if (currentClient.accountNumber == accountNumber)
            return true;

    return false;
}

void readClientRecord(sClient & client) {
    client.pinCode = readWord("Enter PIN code        : ");
    client.name = readName("Enter name            : ");
    client.phone = readWord("Enter phone           : ");
    client.accountBalance = readPositiveNumber("Enter account balance : ");
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

string convertClientHistoryRecordToLine(const sClient & client, const string delimiter, const enClientUpdateType whatHappened) {
    string result = "";

    if (client.dateModified == "-")
        result += client.dateCreated + delimiter;
    else
        result += client.dateModified + delimiter;

    result += client.pinCode + delimiter;
    result += client.name + delimiter;
    result += client.phone + delimiter;
    result += to_string(client.accountBalance) + delimiter;
    result += to_string(whatHappened);

    return result;
}

void appendClientRecordToClientsFile(const string & fileName, sClient & client) {
    fstream clientsFile(fileName, ios::app);

    if (clientsFile.is_open()) {
        clientsFile << convertClientRecordToLine(client, "#//#") << endl;

        clientsFile.close();
    }
}

void appendClientRecordToHistoryFile(const string & fileName, sClient & client) {
    fstream historyFile(fileName, ios::app);

    if (historyFile.is_open()) {
        historyFile << client.accountNumber << '\n';
        historyFile << convertClientHistoryRecordToLine(client, "#//#", enClientUpdateType::eClientCreated) << '\n';

        historyFile.close();
    }
}

void displayAddNewClientScreen() {
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
            newAccountNumber = readWord("Enter account number  : ");

            if (newAccountNumber == "-99")
                backToManageClientsScreen();

            errorMessage = "Account number [" + newAccountNumber + "] already exists.\n";
        } while (doesAccountNumberExist(newAccountNumber));

        sClient newClient;

        newClient.accountNumber = newAccountNumber;

        readClientRecord(newClient);
        newClient.dateCreated = getSystemDateTime();
        
        vClients.push_back(newClient);

        appendClientRecordToClientsFile(clientsFileName, newClient);
        appendClientRecordToHistoryFile(historyFileName, newClient);

        cout << "\nClient added successfully.\n";
        shouldAddNewClient = readCharacter("Do you want to add a new client? [Y] - [N]: ");
    }

    backToManageClientsScreen();
}

void printAccountRecord(const sClient & client) {
    cout << "\nThe following are the client's details:";
    cout << "\n------------------------------------------";
    cout << "\nAccount Number  :" << client.accountNumber;
    cout << "\nPIN Code        :" << client.pinCode;
    cout << "\nName            :" << client.name;
    cout << "\nPhone           :" << client.phone;
    cout << "\nAccount Balance :" << client.accountBalance;
    cout << "\n------------------------------------------\n";
}

void printAccountRecord(const string & accountNumber) {
    for (sClient & currentClient : vClients) {
        if (currentClient.accountNumber == accountNumber) {
            printAccountRecord(currentClient);
            break;
        }
    }
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
            backToManageClientsScreen();

        clientPosition = getClientPosition(accountNumber);

        errorMessage = "Account Number [" + accountNumber + "] does not exist.\n\n";
    } while (clientPosition == -1);

    return clientPosition;
}

void copyVectorToFile(const string & fileName, vector<string> & vector) {
    fstream clientsFile(fileName, ios::out);

    if (clientsFile.is_open()) {
        for (string & currentLine : vector)
            clientsFile << currentLine << '\n';

        clientsFile.close();
    }
}

void saveClientsToClientsFile(const string fileName) {
    vector<string> vClientsLines;

    for (sClient & currentClientRecord : vClients)
        if (!currentClientRecord.markForDelete)
            vClientsLines.push_back(convertClientRecordToLine(currentClientRecord, "#//#"));

    copyVectorToFile(fileName, vClientsLines);
}

void insertStringInVector(vector<string> & vct, const short & pos, const string & element) {
    vector<string> vTemp;

    for (short i = 0; i < pos; i++)
        vTemp.push_back(vct.at(i));

    vTemp.push_back(element);

    for (short i = pos; i < vct.size(); i++)
        vTemp.push_back(vct.at(i));

    vct = vTemp;
}

short getNextAccountNumberPosition(vector<string> & vLines, const string & accountNumber) {
    bool hasFoundAccountNumber = false;
    vLines.push_back(" "); //This is crucial in case the accountNumber is the last in file

    for (short i = 0; i < vLines.size(); i++) {
        if (vLines[i] == accountNumber) {
            hasFoundAccountNumber = true;
            continue;
        }
            
        if (hasFoundAccountNumber && vLines[i].find("#//#") == string::npos) {
            vLines.pop_back(); //To get rid of that white space
            return i;
        }
            
    }

    return -1; //This will never be reached anyways
}

void insertClientRecordInHistoryFile(const string & historyFilePath, const sClient & client, const enClientUpdateType whatHappened) {
    vector<string> vAllLines = copyFileToVector(historyFilePath);
    
    insertStringInVector(vAllLines, getNextAccountNumberPosition(vAllLines, client.accountNumber), convertClientHistoryRecordToLine(client, "#//#", whatHappened));

    copyVectorToFile(historyFilePath, vAllLines);
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

                saveClientsToClientsFile(clientsFileName);
                vClients = loadClientsFromFile(clientsFileName); //To refresh the vector

                cout << "\nClient deleted successfully.\n";

                shouldDeleteClient = readCharacter("Do you want to delete another client? [Y] - [N]: ");
            }
        }
    }

    backToManageClientsScreen();
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

                saveClientsToClientsFile(clientsFileName);
                insertClientRecordInHistoryFile(historyFileName, vClients[clientToUpdatePosition], enClientUpdateType::eFullUpdate);
                
                cout << "\nClient updated successfully.\n";
                shouldUpdateClient = readCharacter("Do you want to update another client? [Y] - [N]: ");
            }
        }
    }

    backToManageClientsScreen();
}

vector<string> copyHistoryClientLinesFromFileToVector(const string & historyFilePath, const string & accountNumber) {
    fstream historyFile(historyFilePath, ios::in);
    vector<string> vClientLines;

    if (historyFile.is_open()) {
        string currentLine = "";
        bool shouldPushBack = false;
        
        while (getline(historyFile, currentLine)) {
            if (shouldPushBack) {
                if (currentLine.find("#//#") == string::npos)
                    break;
                else
                    vClientLines.push_back(currentLine);

            } else if (currentLine == accountNumber)
                shouldPushBack = true;
        }

        historyFile.close();
    }

    return vClientLines;
}

sClientForHistory convertClientHistoryLineToRecord(string & line) {
    vector<string> vHistoryRecordData = splitStringToVector(line, "#//#");
    sClientForHistory clientHistoryPoint;

    clientHistoryPoint.date = vHistoryRecordData[0];
    clientHistoryPoint.pinCode = vHistoryRecordData[1];
    clientHistoryPoint.name = vHistoryRecordData[2];
    clientHistoryPoint.phone = vHistoryRecordData[3];
    clientHistoryPoint.accountBalance = stoi(vHistoryRecordData[4]);
    clientHistoryPoint.whatHappened = (enClientUpdateType) stoi(vHistoryRecordData[5]);
    
    return clientHistoryPoint;
}

vector<sClientForHistory> loadClientRecordsFromFile(const string & accountNumber, const string & historyFilePath) {
    vector<string> vClientLines = copyHistoryClientLinesFromFileToVector(historyFilePath, accountNumber);
    vector<sClientForHistory> vClientHistory;

    for (string & currentLine : vClientLines)
        vClientHistory.push_back(convertClientHistoryLineToRecord(currentLine));
    
    return vClientHistory;
}

string getClientUpdateTypeText(const enClientUpdateType & updateType) {
    switch (updateType) {
        case enClientUpdateType::eClientCreated: return "Account Created";
        case enClientUpdateType::eFullUpdate: return "Full Update";
        case enClientUpdateType::eOnlyDeposit: return "Deposit";
        case enClientUpdateType::eOnlyWithdraw: return "Withdraw";
    }
}

void printClientHistoryHeader(const string & accountNumber) {
    cout << "\t\t\t\t\t\t\t" << accountNumber;

    cout << "\n------------------------------------------------------------------------------------------------------------------";
    cout << "\n|        Date        |  PIN Code  |        Client Name         |     Phone     |    Balance    |   Action Done   |";
    cout << "\n------------------------------------------------------------------------------------------------------------------" << endl;
}

void printClientHistoryData(const string & accountNumber) {
    vector<sClientForHistory> vClientRecords = loadClientRecordsFromFile(accountNumber, historyFileName);

    for (sClientForHistory & currentRecord : vClientRecords) {
        cout << "| " << setw(19) << left << currentRecord.date;
        cout << "| " << setw(11) << left << currentRecord.pinCode;
        cout << "| " << setw(27) << left << currentRecord.name;
        cout << "| " << setw(14) << left << currentRecord.phone;
        cout << "| " << setw(14) << left << currentRecord.accountBalance;
        cout << "| " << setw(16) << left << getClientUpdateTypeText(currentRecord.whatHappened) << "|\n";
    }

    cout << "------------------------------------------------------------------------------------------------------------------" << endl;
}

void printPreviousFindUserScreen(const string & accountNumber) {
    system("cls");

    cout << "* You can enter [-99] to Cancel.\n\n";
    cout << "---------------------------------\n";
    cout << "       Find Client Screen\n";
    cout << "---------------------------------\n";
    cout << "Enter account number: " << accountNumber << '\n';

    printAccountRecord(accountNumber);

    cout << "[1]: Show Client History.";
    cout << "\n[2]: Find Another Client.";
    cout << "\n[3]: Manage Clients Menu.";
    cout << "\n---------------------------------" << endl;

    short userChoice = readNumberInRange("Choose [1] to [3]: ", 1, 3);

    if (userChoice == 1)
        displayClientHistoryScreen(accountNumber);
    else if (userChoice == 2)
        displayFindClientScreen();
    else
        displayManageClientsScreen();
}

void displayClientHistoryScreen(const string & accountNumber) {
    system("cls");

    printClientHistoryHeader(accountNumber);

    printClientHistoryData(accountNumber);

    cout << "\nPress any key to go back...";
    system("pause>nul");
    printPreviousFindUserScreen(accountNumber);
}

void displayFindClientScreen() {
    system("cls");

    if (vClients.empty()) {
        cout << "No clients available.\n";
        backToManageClientsScreen();
    } else {
        short userChoice = 2;
        short clientPosition = 0;

        while (userChoice == 2) {
            system("cls");

            cout << "* You can enter [-99] to Cancel.\n\n";
            cout << "---------------------------------\n";
            cout << "       Find Client Screen\n";
            cout << "---------------------------------\n";

            clientPosition = readClientPosition();
            printAccountRecord(vClients[clientPosition]);

            cout << "[1]: Show Client History.";
            cout << "\n[2]: Find Another Client.";
            cout << "\n[3]: Manage Clients Menu.";
            cout << "\n------------------------------------------\n";
            
            userChoice = readNumberInRange("Choose [1] to [3]: ", 1, 3);

            if (userChoice == 1) {
                displayClientHistoryScreen(vClients[clientPosition].accountNumber);
                break;

            } else if (userChoice == 3) {
                displayManageClientsScreen();
                break;
            }
        }
    }
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
    cout << "\nPress any key to return to Transactions Menu Screen...";
    system("pause>nul");

    displayTransactionsMenuScreen();
}

void depositAmount(sClient & client, const double amount) {
    if (shouldPerformTransaction()) {
        client.accountBalance += amount;
        saveClientsToClientsFile(clientsFileName);
        insertClientRecordInHistoryFile(historyFileName, client, amount > 0 ? enClientUpdateType::eOnlyDeposit : enClientUpdateType::eOnlyWithdraw);

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
        backToManageClientsScreen();
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
            case enTransactionsMenuOptions::eManageClientsScreenFromTransactions:
                displayManageClientsScreen();
        }
    }
}

bool doesUserHaveMainMenuPermission(const sUser & user, const enMainMenuOptions mainMenuOption) {
    return mainMenuOption == enMainMenuOptions::eManageClients ? user.manageClientsPermissions != 0 : user.manageUsersPermissions != 0;
}

void displayAccessDeniedScreen(const enWhereToGoBack whereToReturn) {
    system("cls");

    cout << "---------------------------------------\n";
    cout << "Access Denied,\n";
    cout << "You don't have permission to do this.\n";
    cout << "Please contact your Admin.\n";
    cout << "--------------------------------------\n";

    switch (whereToReturn) {
        case enWhereToGoBack::eMainMenuScreen:
            backToMainMenuScreen();
            break;
            
        case enWhereToGoBack::eManageClientsScreen:
            backToManageClientsScreen();
            break;

        default:
            backToManageUsersScreen();
    }
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

void printLineForUsersTable() {
    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eUsername))
        printDashes(25);
    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::ePassword))
        printDashes(23);
    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::ePermissions))
        printDashes(18);
    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eDateCreatedUsers))
        printDashes(21);
    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eDateModifiedUsers))
        printDashes(21);

    cout << '-';
}

void printUsersTableHeader() {
    cout << '\n';
    printLineForUsersTable();
    cout << "\n|";

    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eUsername))
        cout << "        Username        |";

    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::ePassword))
        cout << "       Password       |";

    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::ePermissions))
        cout << "   Permissions   |";

    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eDateCreatedUsers))
        cout << "    Date Created    |";

    if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eDateModifiedUsers))
        cout << "    Date Modified   |";

    cout << '\n';
    printLineForUsersTable();
    cout << endl;
}

void printUsersTable() {
    for (sUser & currentUser : vUsers) {
        if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eUsername))
            cout << "| " << left << setw(23) << (currentUser.username + (currentUser.username == activeUser.username ? " (Active User)" : ""));

        if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::ePassword))
            cout << "| " << left << setw(21) << currentUser.password;

        if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::ePermissions))
            cout << "| " << left << setw(16) << (to_string(currentUser.manageClientsPermissions) + " / " + to_string(currentUser.manageUsersPermissions));
        
        if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eDateCreatedUsers))
            cout << "| " << left << setw(19) << currentUser.dateCreated;
        
        if (shouldUserFieldAppear(globalSettings.userTableSettings, enUserTableFields::eDateModifiedUsers))
            cout << "| " << left << setw(19) << (currentUser.dateModified == "-" ? "        -" : currentUser.dateModified);

        cout << "|\n";
    }

    printLineForUsersTable();
    cout << endl;
}

bool doesUsernameExist(const string & username) {
    for (sUser & currentUser : vUsers)
        if (currentUser.username == username)
            return true;
    
    return false;
}

void readUserRecord(sUser & user) {
    user.password = readWord("Enter password: ");

    //Reading ManageClients Permissions
    if (tolower(readCharacter("Give full access over Clients? [Y] - [N]: ")) == 'y')
        user.manageClientsPermissions = -1;
    else {
        user.manageClientsPermissions = 0;
        cout << "\nGiving Access To:\n";

        if (tolower(readCharacter("  * Show Clients List [Y] - [N]: ")) == 'y')
            user.manageClientsPermissions += 1;

        if (tolower(readCharacter("  * Add new Client [Y] - [N]: ")) == 'y')
            user.manageClientsPermissions += 2;

        if (tolower(readCharacter("  * Delete Client [Y] - [N]: ")) == 'y')
            user.manageClientsPermissions += 4;

        if (tolower(readCharacter("  * Update Client Info [Y] - [N]: ")) == 'y')
            user.manageClientsPermissions += 8;

        if (tolower(readCharacter("  * Find Client [Y] - [N]: ")) == 'y')
            user.manageClientsPermissions += 16;

        if (tolower(readCharacter("  * Transactions [Y] - [N]: ")) == 'y')
            user.manageClientsPermissions += 32;

        if (user.manageClientsPermissions == 63)
            user.manageClientsPermissions = -1;

        cout << endl;
    }
    
    //Reading ManageUsers Permissions
    if (tolower(readCharacter("Give full access over Users? [Y] - [N]: ")) == 'y')
        user.manageUsersPermissions = -1;
    else {
        user.manageUsersPermissions = 0;
        cout << "\nGiving Access To:\n";

        if (tolower(readCharacter("  * Show Users List [Y] - [N]: ")) == 'y')
            user.manageUsersPermissions += 1;

        if (tolower(readCharacter("  * Add New User [Y] - [N]: ")) == 'y')
            user.manageUsersPermissions += 2;

        if (tolower(readCharacter("  * Delete User [Y] - [N]: ")) == 'y')
            user.manageUsersPermissions += 4;

        if (tolower(readCharacter("  * Update User Info [Y] - [N]: ")) == 'y')
            user.manageUsersPermissions += 8;

        if (tolower(readCharacter("  * Find User [Y] - [N]: ")) == 'y')
            user.manageUsersPermissions += 16;

        if (user.manageUsersPermissions == 31)
            user.manageUsersPermissions = -1;
    }
}

string convertUserRecordToLine(const sUser & userRecord, const string delimiter) {
    string result = "";

    result += userRecord.username + delimiter;
    result += userRecord.password + delimiter;
    result += to_string(userRecord.manageClientsPermissions) + delimiter;
    result += to_string(userRecord.manageUsersPermissions) + delimiter;
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

bool doesUserHavePermissionOverClients(const short & manageClientsPermissions, const enManageClientsMenuOptions manageClientsOption) {
    if (manageClientsPermissions == -1)
        return true;

    short optionAsPower = power(2, (short) manageClientsOption - 1);
    return (manageClientsPermissions & optionAsPower) == optionAsPower;
}

bool doesUserHavePermissionOverUsers(const short & manageUsersPermissions, const enManageUsersMenuOptions manageUsersOption) {
    if (manageUsersPermissions == -1)
        return true;

    short optionAsPower = power(2, (short) manageUsersOption - 1);
    return (manageUsersPermissions & optionAsPower) == optionAsPower;
}

void printUserPermissionsOverClients(const short & manageClientsPermissions) {
    if (manageClientsPermissions == -1)
        cout << " Full Access";
    else if (manageClientsPermissions == 0)
        cout << " No Access";
    else {
        if (doesUserHavePermissionOverClients(manageClientsPermissions, enManageClientsMenuOptions::eShowClientList))
            cout << "\n  * Show Client List.";

        if (doesUserHavePermissionOverClients(manageClientsPermissions, enManageClientsMenuOptions::eAddNewClient))
            cout << "\n  * Add New Client.";

        if (doesUserHavePermissionOverClients(manageClientsPermissions, enManageClientsMenuOptions::eDeleteClient))
            cout << "\n  * Delete Client.";

        if (doesUserHavePermissionOverClients(manageClientsPermissions, enManageClientsMenuOptions::eUpdateClient))
            cout << "\n  * Update Client Info.";

        if (doesUserHavePermissionOverClients(manageClientsPermissions, enManageClientsMenuOptions::eFindClient))
            cout << "\n  * Find Client.";

        if (doesUserHavePermissionOverClients(manageClientsPermissions, enManageClientsMenuOptions::eTransactions))
            cout << "\n  * Transactions.";
    }
}

void printUserPermissionsOverUsers(const short & manageUsersPermissions) {
    if (manageUsersPermissions == -1)
        cout << " Full Access";
    else if (manageUsersPermissions == 0)
        cout << " No Access";
    else {
        if (doesUserHavePermissionOverUsers(manageUsersPermissions, enManageUsersMenuOptions::eShowUsersList))
            cout << "\n  * Show Users List.";

        if (doesUserHavePermissionOverUsers(manageUsersPermissions, enManageUsersMenuOptions::eAddNewUser))
            cout << "\n  * Add New User.";

        if (doesUserHavePermissionOverUsers(manageUsersPermissions, enManageUsersMenuOptions::eDeleteUser))
            cout << "\n  * Delete User.";

        if (doesUserHavePermissionOverUsers(manageUsersPermissions, enManageUsersMenuOptions::eUpdateUser))
            cout << "\n  * Update User Info.";

        if (doesUserHavePermissionOverUsers(manageUsersPermissions, enManageUsersMenuOptions::eFindUser))
            cout << "\n  * Find User.";
    }
}

void printUserRecord(const sUser & userRecord) {
    cout << "\nThe following are the user's details:";
    cout << "\n-------------------------------------";
    cout << "\nUsername    : " << userRecord.username;

    if (userRecord.username == activeUser.username)
        cout << " (Active User)";

    cout << "\nPassword    : " << userRecord.password;

    cout << "\nPermissions over Clients :";
    printUserPermissionsOverClients(userRecord.manageClientsPermissions);

    cout << "\nPermissions over Users   :";
    printUserPermissionsOverUsers(userRecord.manageUsersPermissions);

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
    user.manageClientsPermissions = stoi(vUserDate.at(2));
    user.manageUsersPermissions = stoi(vUserDate.at(3));
    user.dateCreated = vUserDate.at(4);
    user.dateModified = vUserDate.at(5);

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
            string adminLine = "Admin#//#1234#//#-1#//#-1#//#" + getSystemDateTime() + "#//#-";
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

    cout << "* Users List - " << vUsers.size() << " User(s) available";

    printUsersTableHeader();

    printUsersTable();

    backToManageUsersScreen();
}

void displayAddNewUserScreen() {
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

                if (!doesUserHaveMainMenuPermission(activeUser, enMainMenuOptions::eManageUsers)) {
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
            if (doesUserHavePermissionOverUsers(activeUser.manageUsersPermissions, enManageUsersMenuOptions::eShowUsersList))
                displayUsersListScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageUsersScreen);
            break;

        case enManageUsersMenuOptions::eAddNewUser:
            if (doesUserHavePermissionOverUsers(activeUser.manageUsersPermissions, enManageUsersMenuOptions::eAddNewUser))
                displayAddNewUserScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageUsersScreen);
            break;

        case enManageUsersMenuOptions::eDeleteUser:
            if (doesUserHavePermissionOverUsers(activeUser.manageUsersPermissions, enManageUsersMenuOptions::eDeleteUser))
                displayDeleteUserScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageUsersScreen);
            break;

        case enManageUsersMenuOptions::eUpdateUser:
            if (doesUserHavePermissionOverUsers(activeUser.manageUsersPermissions, enManageUsersMenuOptions::eUpdateUser))
                displayUpdateUserScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageUsersScreen);
            break;

        case enManageUsersMenuOptions::eFindUser:
            if (doesUserHavePermissionOverUsers(activeUser.manageUsersPermissions, enManageUsersMenuOptions::eFindUser))
                displayFindUserScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageUsersScreen);
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

    cout << "\nPermissions over Clients :";
    printUserPermissionsOverClients(activeUser.manageClientsPermissions);

    cout << "\nPermissions over Users   :";
    printUserPermissionsOverUsers(activeUser.manageUsersPermissions);

    cout << "\n---------------------------------" << endl;

    backToMainMenuScreen();
}

void displayManageClientsScreen() {
    system("cls");
    printManageClientsMenu();

    enManageClientsMenuOptions manageClientsOption = (enManageClientsMenuOptions) readNumberInRange("Choose [1] to [7]: ", 1, 7);

    switch (manageClientsOption) {
        case enManageClientsMenuOptions::eShowClientList:
            if (doesUserHavePermissionOverClients(activeUser.manageClientsPermissions, enManageClientsMenuOptions::eShowClientList))
                displayClientsListScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageClientsScreen);
            break;

        case enManageClientsMenuOptions::eAddNewClient:
            if (doesUserHavePermissionOverClients(activeUser.manageClientsPermissions, enManageClientsMenuOptions::eAddNewClient))
                displayAddNewClientScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageClientsScreen);
            break;

        case enManageClientsMenuOptions::eDeleteClient:
            if (doesUserHavePermissionOverClients(activeUser.manageClientsPermissions, enManageClientsMenuOptions::eDeleteClient))
                displayDeleteClientScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageClientsScreen);
            break;

        case enManageClientsMenuOptions::eUpdateClient:
            if (doesUserHavePermissionOverClients(activeUser.manageClientsPermissions, enManageClientsMenuOptions::eUpdateClient))
                displayUpdateClientScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageClientsScreen);
            break;
            
        case enManageClientsMenuOptions::eFindClient:
            if (doesUserHavePermissionOverClients(activeUser.manageClientsPermissions, enManageClientsMenuOptions::eFindClient))
                displayFindClientScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageClientsScreen);
            break;

        case enManageClientsMenuOptions::eTransactions:
            if (doesUserHavePermissionOverClients(activeUser.manageClientsPermissions, enManageClientsMenuOptions::eTransactions))
                displayTransactionsMenuScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eManageClientsScreen);

        default:
            displayMainMenuScreen();
    }
}

void printClientTableFields(const short & clientTableSettings) {
    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::eAccountNumber))
        cout << "\n  * Account Number";
    
    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::ePinCode))
        cout << "\n  * PIN Code";

    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::eName))
        cout << "\n  * Client Name";
    
    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::ePhone))
        cout << "\n  * Phone";
    
    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::eAccountBalance))
        cout << "\n  * Balance";
    
    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::eDateCreatedClients))
        cout << "\n  * Date Created";
    
    if (shouldClientFieldAppear(clientTableSettings, enClientTableFields::eDateModifiedClients))
        cout << "\n  * Date Modified";
}

short readVisibleClientFields() {
    cout << "\nChoose Visible Fields:\n";
    short settings = 0;

    if (tolower(readCharacter("  * Account Number [Y] - [N]: ")) == 'y')
        settings += 1;

    if (tolower(readCharacter("  * PIN Code [Y] - [N]: ")) == 'y')
        settings += 2;

    if (tolower(readCharacter("  * Client Name [Y] - [N]: ")) == 'y')
        settings += 4;

    if (tolower(readCharacter("  * Phone [Y] - [N]: ")) == 'y')
        settings += 8;

    if (tolower(readCharacter("  * Balance [Y] - [N]: ")) == 'y')
        settings += 16;

    if (tolower(readCharacter("  * Date Created [Y] - [N]: ")) == 'y')
        settings += 32;

    if (tolower(readCharacter("  * Date Modified [Y] - [N]: ")) == 'y')
        settings += 64;

    if (settings == 127)
        settings = -1;

    return settings;
}

void backToSettingsScreen() {
    cout << "\nPress any key to return to Settings Screen...";
    system("pause>nul");

    displaySettingsScreen();
}

void displayClientTableSettingsScreen() {
    system("cls");

    cout << "Here are the visible fields currently used in the Clients Table:";
    printClientTableFields(globalSettings.clientTableSettings);
    cout << "\n-----------------------------";
    cout << "\n[1]: Change Visible Fields.";
    cout << "\n[2]: Back.";
    cout << "\n-----------------------------" << endl;

    short userChoice = readNumberInRange("Choose [1] - [2]: ", 1, 2);
    if (userChoice == 1) {
        short newVisibleClientFields = readVisibleClientFields();

        if (newVisibleClientFields == 0)
            cout << "\nInvalid input, reverting to previous settings...";
        else {
            globalSettings.clientTableSettings = newVisibleClientFields;
            saveSettingsToFile(globalSettings, settingsFileName);
            cout << "\nDone Successfully.";
        }
        
        backToSettingsScreen();
    } else
        displaySettingsScreen();
}

void printUserTableFields(const short & userTableSettings) {
    if (shouldUserFieldAppear(userTableSettings, enUserTableFields::eUsername))
        cout << "\n  * Username";

    if (shouldUserFieldAppear(userTableSettings, enUserTableFields::ePassword))
        cout << "\n  * Password";

    if (shouldUserFieldAppear(userTableSettings, enUserTableFields::ePermissions))
        cout << "\n  * Permissions";

    if (shouldUserFieldAppear(userTableSettings, enUserTableFields::eDateCreatedUsers))
        cout << "\n  * Date Created";

    if (shouldUserFieldAppear(userTableSettings, enUserTableFields::eDateModifiedUsers))
        cout << "\n  * Date Modified";
}

short readVisibleUserFields() {
    cout << "\nChoose Visible Fields:\n";
    short settings = 0;

    if (tolower(readCharacter("  * Username [Y] - [N]: ")) == 'y')
        settings += 1;

    if (tolower(readCharacter("  * Password [Y] - [N]: ")) == 'y')
        settings += 2;

    if (tolower(readCharacter("  * Permissions [Y] - [N]: ")) == 'y')
        settings += 4;

    if (tolower(readCharacter("  * Date Created [Y] - [N]: ")) == 'y')
        settings += 8;

    if (tolower(readCharacter("  * Date Modified [Y] - [N]: ")) == 'y')
        settings += 16;

    if (settings == 31)
        settings = -1;

    return settings;
}

void displayUserTableSettingsScreen() {
    system("cls");

    cout << "Here are the visible fields currently used in the Users Table:";
    printUserTableFields(globalSettings.userTableSettings);
    cout << "\n-----------------------------";
    cout << "\n[1]: Change Visible Fields.";
    cout << "\n[2]: Back.";
    cout << "\n-----------------------------" << endl;

    short userChoice = readNumberInRange("Choose [1] - [2]: ", 1, 2);
    if (userChoice == 1) {
        short newVisibleUserFields = readVisibleUserFields();

        if (newVisibleUserFields == 0)
            cout << "\nInvalid input, reverting to previous settings...";
        else {
            globalSettings.userTableSettings = newVisibleUserFields;
            saveSettingsToFile(globalSettings, settingsFileName);
            cout << "\nDone Successfully.";
        }
        
        backToSettingsScreen();
    } else
        displaySettingsScreen();
}

string getColor(const enTextBackground wantedElement) {
    switch (globalSettings.programColors[(short) wantedElement]) {
        case '0': return "  * Black";
        case '1': return "  * Blue";
        case '2': return "  * Green";
        case '3': return "  * Aqua";
        case '4': return "  * Red";
        case '5': return "  * Purple";
        case '6': return "  * Yellow";
        case '7': return "  * White";
        case '8': return "  * Gray";
        case '9': return "  * Light Blue";
        case 'A': return "  * Light Green";
        case 'B': return "  * Light Aqua";
        case 'C': return "  * Light Red";
        case 'D': return "  * Light Purple";
        case 'E': return "  * Light Yellow";
        case 'F': return "  * Bright White";
    }
}

void printColorsList() {
    cout << "\nHere are the available colors:";
    cout << "\n---------------------------------------------";
    cout << "\n[1]: Black     [2]: Bright White";
    cout << "\n[3]: Red       [4]: Blue";
    cout << "\n[5]: Green     [6]: Yellow";
    cout << "\n[7]: Aqua      [8]: Purple";
    cout << "\n[9]: White     [10]: Gray";
    cout << "\n[11]: Light Red       [12]: Light Blue";
    cout << "\n[13]: Light Green     [14]: Light Yellow";
    cout << "\n[15]: Light Aqua      [16]: Light Purple";
    cout << "\n---------------------------------------------\n";
}

char convertColorOrderToCharacter(const short colorOrder) {
    return "0F41263578C9AEBD"[colorOrder - 1];
}

void displayChangeTextColorScreen() {
    system("cls");
    cout << "Here is the color currently used for Text:\n";
    cout << getColor(enTextBackground::eText);
    
    cout << "\n-----------------------------";
    cout << "\n[1]: Change Color.";
    cout << "\n[2]: Back.";
    cout << "\n-----------------------------" << endl;

    short userChoice = readNumberInRange("Choose [1] - [2]: ", 1, 2);

    if (userChoice == 1) {
        printColorsList();
        char userColorChoiceCharacter;

        while ((userColorChoiceCharacter = convertColorOrderToCharacter(readNumberInRange("Choose [1] to [16]: ", 1, 16))) == globalSettings.programColors[0])
            cout << "Invalid Input, you cannot choose the same color as the Background.\n\n";

        globalSettings.programColors[1] = userColorChoiceCharacter;
        saveSettingsToFile(globalSettings, settingsFileName);

        system(("Color " + globalSettings.programColors).c_str());
        cout << "\nDone successfully.";

        backToSettingsScreen();
    } else
        displaySettingsScreen();
}

void displayChangeBackgroundColorScreen() {
    system("cls");
    cout << "Here is the color currently used for Background:\n";
    cout << getColor(enTextBackground::eBackground);

    cout << "\n-----------------------------";
    cout << "\n[1]: Change Color.";
    cout << "\n[2]: Back.";
    cout << "\n-----------------------------" << endl;

    short userChoice = readNumberInRange("Choose [1] - [2]: ", 1, 2);

    if (userChoice == 1) {
        printColorsList();
        char userColorChoiceCharacter;

        while ((userColorChoiceCharacter = convertColorOrderToCharacter(readNumberInRange("Choose [1] to [16]: ", 1, 16))) == globalSettings.programColors[1])
            cout << "Invalid Input, you cannot choose the same color as the Text.\n\n";

        globalSettings.programColors[0] = userColorChoiceCharacter;
        saveSettingsToFile(globalSettings, settingsFileName);

        system(("Color " + globalSettings.programColors).c_str());
        cout << "\nDone successfully.";
        
        backToSettingsScreen();
    } else
        displaySettingsScreen();
}

void displaySettingsScreen() {
    system("cls");

    cout << "---------------------------------------";
    cout << "\n               Settings";
    cout << "\n---------------------------------------";

    cout << "\n[1]: Clients Data Table Settings.";
    cout << "\n[2]: Users Data Table Settings.";
    cout << "\n[3]: Change Text Color.";
    cout << "\n[4]: Change Background Color.";
    cout << "\n[5]: Main Menu.";
    cout << "\n---------------------------------------\n";

    enSettingsOptions settingOption = (enSettingsOptions) readNumberInRange("Choose [1] to [5]: ", 1, 5);
    
    switch (settingOption) {
        case enSettingsOptions::eClientsTableSettings:
            displayClientTableSettingsScreen();
            break;
        case enSettingsOptions::eUsersTableSettings:
            displayUserTableSettingsScreen();
            break;
        case enSettingsOptions::eTextColor:
            displayChangeTextColorScreen();
            break;
        case enSettingsOptions::eBackgroundColor:
            displayChangeBackgroundColorScreen();
            break;
        default:
            displayMainMenuScreen();
    }
}

void displayMainMenuScreen() {
    system("cls");
    printMainMenuScreen();

    enMainMenuOptions mainMenuOption = (enMainMenuOptions) readNumberInRange("Choose [1] to [5]: ", 1, 5);

    switch (mainMenuOption) {
        case enMainMenuOptions::eManageClients:
            if (doesUserHaveMainMenuPermission(activeUser, enMainMenuOptions::eManageClients))
                displayManageClientsScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eMainMenuScreen);

            break;
        case enMainMenuOptions::eManageUsers:
            if (doesUserHaveMainMenuPermission(activeUser, enMainMenuOptions::eManageUsers))
                displayManageUsersScreen();
            else
                displayAccessDeniedScreen(enWhereToGoBack::eMainMenuScreen);

            break;
        case enMainMenuOptions::eShowActiveUser:
            displayActiveUserScreen();
            break;

        case enMainMenuOptions::eSettings:
            displaySettingsScreen();
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

void loadDataFromFiles() {
    vUsers = loadUsersFromFile(usersFileName);
    vClients = loadClientsFromFile(clientsFileName);
    globalSettings = loadSettingsFromFile(settingsFileName);
}

int main() {
    loadDataFromFiles();
    
    system(("Color " + globalSettings.programColors).c_str());

    displayLoginScreen();

    return 0;
}
