#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

/* ================= DATA ================= */
class Item {
public:
    int id;
    string name;
    float price;
};

class User {
public:
    string username;
    string password;
    string role;
};

vector<Item> menu;
vector<User> users;
User* currentUser = NULL;

/* ================= BOX UI (SAFE ASCII) ================= */
void printBox(string title) {
    cout << "\n+----------------------------------+\n";
    cout << "| " << title;

    for (int i = title.length(); i < 32; i++)
        cout << " ";

    cout << "|\n";
    cout << "+----------------------------------+\n";
}

/* ================= PASSWORD HASH ================= */
string hashPass(string pass) {
    int hash = 0;
    for (char c : pass) {
        hash = (hash * 31 + c) % 100000;
    }
    return to_string(hash);
}

/* ================= LOAD USERS ================= */
void loadUsers() {
    ifstream file("users.txt");
    User u;

    while (file >> u.username >> u.password >> u.role) {
        users.push_back(u);
    }
}

/* ================= LOAD MENU ================= */
void loadMenu() {
    ifstream file("menu.txt");
    Item i;

    while (file >> i.id >> i.name >> i.price) {
        menu.push_back(i);
    }
}

/* ================= SAVE MENU ================= */
void saveMenu() {
    ofstream file("menu.txt");

    for (auto i : menu) {
        file << i.id << " " << i.name << " " << i.price << endl;
    }
}

/* ================= LOGIN ================= */
User* login() {
    string user, pass;

    printBox("LOGIN SYSTEM");

    cout << "Username: ";
    cin >> user;

    cout << "Password: ";
    cin >> pass;

    string hashed = hashPass(pass);

    for (auto &u : users) {
        if (u.username == user && u.password == hashed) {
            cout << "Login Successful!\n";
            return &u;
        }
    }

    cout << "Invalid Credentials!\n";
    return NULL;
}

/* ================= REGISTER ================= */
void registerUser() {
    ofstream file("users.txt", ios::app);

    string user, pass;

    printBox("REGISTER USER");

    cout << "Username: ";
    cin >> user;

    cout << "Password: ";
    cin >> pass;

    file << user << " " << hashPass(pass) << " user" << endl;

    cout << "User Registered Successfully!\n";
}

/* ================= SHOW MENU ================= */
void showMenu() {
    printBox("COFFEE MENU");

    cout << "ID   NAME            PRICE\n";
    cout << "----------------------------------\n";

    for (auto i : menu) {
        cout << i.id << "    "
             << i.name << "        "
             << i.price << " Tk\n";
    }
}

/* ================= BILL ================= */
void printBill(vector<pair<Item,int>> cart, float total) {
    printBox("FINAL BILL");

    cout << "ITEM           QTY    TOTAL\n";
    cout << "----------------------------------\n";

    for (auto c : cart) {
        cout << c.first.name << "       "
             << c.second << "      "
             << c.first.price * c.second << "\n";
    }

    cout << "----------------------------------\n";
    cout << "TOTAL PAYABLE: " << total << " Tk\n";
}

/* ================= SAVE RECEIPT ================= */
void saveReceipt(string user, float total) {
    ofstream file("receipts.txt", ios::app);
    file << user << " " << total << endl;
}

/* ================= ORDER SYSTEM ================= */
void orderSystem() {
    vector<pair<Item,int>> cart;
    int id, qty;
    float total = 0;

    while (true) {
        showMenu();

        cout << "Enter ID (0 to finish): ";
        cin >> id;

        if (id == 0) break;

        cout << "Quantity: ";
        cin >> qty;

        bool found = false;

        for (auto i : menu) {
            if (i.id == id) {
                cart.push_back({i, qty});
                total += i.price * qty;
                cout << "Added to cart!\n";
                found = true;
            }
        }

        if (!found) {
            cout << "Invalid ID!\n";
        }
    }

    if (total > 500) {
        cout << "10% Discount Applied!\n";
        total *= 0.9;
    }

    printBill(cart, total);
    saveReceipt(currentUser->username, total);
}

/* ================= SALES REPORT ================= */
void viewSales() {
    ifstream file("receipts.txt");
    string user;
    float total, sum = 0;

    printBox("SALES REPORT");

    cout << "USER        TOTAL\n";
    cout << "----------------------\n";

    while (file >> user >> total) {
        cout << user << "        " << total << "\n";
        sum += total;
    }

    cout << "----------------------\n";
    cout << "TOTAL SALES: " << sum << " Tk\n";
}

/* ================= ADMIN PANEL ================= */
void adminPanel() {
    int choice;

    do {
        printBox("ADMIN PANEL");

        cout << "1. View Menu\n";
        cout << "2. Add Item\n";
        cout << "3. Delete Item\n";
        cout << "4. Sales Report\n";
        cout << "0. Exit\n";

        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) showMenu();

        else if (choice == 2) {
            Item i;
            cout << "ID Name Price: ";
            cin >> i.id >> i.name >> i.price;
            menu.push_back(i);
            saveMenu();
        }

        else if (choice == 3) {
            int id;
            cout << "Enter ID: ";
            cin >> id;

            for (int i = 0; i < menu.size(); i++) {
                if (menu[i].id == id) {
                    menu.erase(menu.begin() + i);
                    break;
                }
            }
            saveMenu();
        }

        else if (choice == 4) viewSales();

    } while (choice != 0);
}

/* ================= AUTH MENU ================= */
void authMenu() {
    int choice;

    do {
        printBox("COFFEE SHOP SYSTEM");

        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "0. Exit\n";

        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            currentUser = login();
            if (currentUser != NULL) break;
        }
        else if (choice == 2) {
            registerUser();
        }

    } while (choice != 0);
}

/* ================= MAIN ================= */
int main() {
    loadUsers();
    loadMenu();

    authMenu();

    if (currentUser != NULL) {
        if (currentUser->role == "admin")
            adminPanel();
        else
            orderSystem();
    }

    return 0;
}
