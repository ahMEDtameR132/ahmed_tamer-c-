//./my_program => run the code.
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <limits>
#include <fstream>

using namespace std;

//(perdon)هيكلة متعرف باسم لدخال البيانات 
struct Person {
    string name;
    int age;
    string job;
    string bloodType;
    int phone;
};

// دالة لفتح قاعدة البيانات
bool openDatabase(sqlite3** db) {
    int rc = sqlite3_open("people.db", db);
    if (rc) {
        cerr << "Error: Can't open database: " << sqlite3_errmsg(*db) << endl;
        return false;
    }
    return true;
}

// دالة لإنشاء الجداول إذا لم تكن موجودة
void createTables(sqlite3* db) {
    const char* createUsersTable = "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT);";
    const char* createPeopleTable = "CREATE TABLE IF NOT EXISTS people (name TEXT, age INTEGER, job TEXT, bloodType TEXT);";

    char* errMsg = 0;
    sqlite3_exec(db, createUsersTable, 0, 0, &errMsg);
    sqlite3_exec(db, createPeopleTable, 0, 0, &errMsg);
}

bool registerUser(sqlite3* db) {
    string username, password;

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    string query = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "');";
    char* errMsg = 0;
    int rc = sqlite3_exec(db, query.c_str(), 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "Error: Could not register user: " << errMsg << endl;
        return false;
    }

    cout << "Registration successful!" << endl;
    return true;
}

bool loginUser(sqlite3* db) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    string query = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "';";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        cerr << "Error: Could not execute query!" << endl;
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        cout << "Login successful! Welcome, " << username << "!" << endl;
        sqlite3_finalize(stmt);
        return true;
    }

    cout << "Login failed: Invalid username or password." << endl;
    sqlite3_finalize(stmt);
    return false;
}

// دالة لإدخال البيانات مع التحقق من صحتها
bool getValidatedInput(string& input, const string& prompt) {
    cout << prompt;
    cin >> input;
    return !input.empty();
}

bool getValidatedInput(int& input, const string& prompt) {
    cout << prompt;
    while (!(cin >> input) || input <= 0) {
        cout << "Invalid input. Please enter a positive number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return true;
}

// دالة لإدخال البيانات من المستخدم
void collectData(sqlite3* db) {
    Person person;
    char choice;

    do {
        if (!getValidatedInput(person.name, "Enter name: ")) {
            cerr << "Invalid name. Try again." << endl;
            continue;
        }

        if (!getValidatedInput(person.age, "Enter age: ")) {
            cerr << "Invalid age. Try again." << endl;
            continue;
        }
        if (!getValidatedInput(person.job, "Enter job: ")) {
            cerr << "Invalid job. Try again." << endl;
            continue;
        }

        if (!getValidatedInput(person.bloodType, "Enter blood type: ")) {
            cerr << "Invalid blood type. Try again." << endl;
            continue;
        }
        if (!getValidatedInput(person.phone, "Enter blood type: ")) {
            cerr << "Invalid phone. Try again." << endl;
            continue;
        }

        string query = "INSERT INTO people (name, age, job, bloodType , phone) VALUES ('" + person.name + "', " + to_string(person.age) + ", '" + person.job + "', '" + person.bloodType + "'"+ to_string(person.age);
        char* errMsg = 0;
        int rc = sqlite3_exec(db, query.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "Error: Could not insert data: " << errMsg << endl;
        } else {
            cout << "Person added successfully!" << endl;
        }

        cout << "Do you want to add another person? (y/n): ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');
}

// دالة لعرض قائمة الأشخاص بترتيب أبجدي
void displayPeople(sqlite3* db) {
    string query = "SELECT * FROM people ORDER BY name ASC;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        cerr << "Error: Could not fetch data!" << endl;
        return;
    }

    cout << "\nData for people:\n";
    cout << "-------------------------------------------------------------------------------\n";
    cout << "No\tName  \t\tAge  \tJob\t\t  Blood   Type\n";
    cout << "-------------------------------------------------------------------------------\n";

    int counter = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        string job = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string bloodType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        cout << counter++ << "\t" << name << "\t\t" << age << "\t" << job << "\t\t" << bloodType << endl;
    }

    sqlite3_finalize(stmt);
}

// دالة للبحث عن شخص بالاسم
void searchPerson(sqlite3* db) {
    string name;
    cout << "Enter name to search: ";
    cin >> name;

    string query = "SELECT * FROM people WHERE name LIKE '%" + name + "%';";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        cerr << "Error: Could not execute query!" << endl;
        return;
    }

     cout << "\nData for people:\n";
    cout << "-------------------------------------------------------------------------------\n";
    cout << "No\tName  \t\tAge  \tJob\t\t  Blood   Type\n";
    cout << "-------------------------------------------------------------------------------\n";

    int counter = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        string job = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string bloodType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        cout << counter++ << "\t" << name << "\t\t" << age << "\t" << job << "\t\t" << bloodType << endl;
    }

    sqlite3_finalize(stmt);
}

// دالة لتحديث بيانات شخص موجود
void updatePerson(sqlite3* db) {
    string name;
    cout << "Enter the name of the person to update: ";
    cin >> name;

    string query = "SELECT * FROM people WHERE name = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Person person;
        person.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        person.age = sqlite3_column_int(stmt, 1);
        person.job = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        person.bloodType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        cout << "Current Data: " << person.name << ", " << person.age << ", " << person.job << ", " << person.bloodType << endl;

        getValidatedInput(person.age, "Enter new age: ");
        getValidatedInput(person.job, "Enter new job: ");
        getValidatedInput(person.bloodType, "Enter new blood type: ");

        string updateQuery = "UPDATE people SET age = ?, job = ?, bloodType = ? WHERE name = ?;";
        sqlite3_stmt* updateStmt;
        sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &updateStmt, 0);
        sqlite3_bind_int(updateStmt, 1, person.age);
        sqlite3_bind_text(updateStmt, 2, person.job.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(updateStmt, 3, person.bloodType.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(updateStmt, 4, person.name.c_str(), -1, SQLITE_STATIC);

        sqlite3_step(updateStmt);
        cout << "Data updated successfully!" << endl;
        sqlite3_finalize(updateStmt);
    } else {
        cout << "Person not found." << endl;
    }
    sqlite3_finalize(stmt);
}

// دالة لحذف شخص من القائمة
void deletePerson(sqlite3* db) {
    string name;
    cout << "Enter the name of the person to delete: ";
    cin >> name;

    string query = "DELETE FROM people WHERE name = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Person deleted successfully!" << endl;
    } else {
        cout << "Error deleting person or person not found." << endl;
    }
    sqlite3_finalize(stmt);
}

// دالة لتصدير البيانات إلى ملف CSV
void exportToCSV(sqlite3* db) {
    ofstream file("people.csv");
    if (!file.is_open()) {
        cerr << "Error: Could not open file for writing." << endl;
        return;
    }

    string query = "SELECT * FROM people;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

    file << "Name,Age,Job,BloodType\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        string job = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string bloodType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        file << name << "," << age << "," << job << "," << bloodType << "\n";
    }

    sqlite3_finalize(stmt);
    file.close();
    cout << "Data exported to people.csv successfully!" << endl;
}

int main() {
    sqlite3* db;
    if (!openDatabase(&db)) {
        return 1;
    }

    createTables(db);

    int choice;
    cout << "1. Register\n2. Login\nChoose an option: ";
    cin >> choice;

    if (choice == 1) {
        registerUser(db);
    } else if (choice == 2) {
        if (!loginUser(db)) {
            return 1;
        }
    } else {
        cout << "Invalid choice." << endl;
        return 1;
    }

    while (true) { 
        cout << "\n1. Add Person\n2. Display All People\n3. Search Person\n4. Update Person\n5. Delete Person\n6. Export to CSV\n7. Exit\nChoose an option: ";
        cin >> choice;

        switch (choice) {
            case 1:
                collectData(db);
                break;
            case 2:
                displayPeople(db);
                break;
            case 3:
                searchPerson(db);
                break;
            case 4:
                updatePerson(db);
                break;
            case 5:
                deletePerson(db);
                break;
            case 6:
                exportToCSV(db);
                break;
            case 7:
                cout << "Exiting program..." << endl;
                sqlite3_close(db);
                return 0;
            default:
                cout << "Invalid option. Please try again." << endl;
        }
    }
    sqlite3_close(db); // تأكيد إغلاق قاعدة البيانات عند إنهاء البرنامج
    return 0;
}