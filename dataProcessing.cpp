#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
using namespace std;

class InMemoryDB {
private:
    unordered_map<string, int> main_db;
    unordered_map<string, int> transaction_db;
    bool transaction_active;

public:
    InMemoryDB() : transaction_active(false) {}

    int get(const string& key, bool& found) {
        if (transaction_active && transaction_db.find(key) != transaction_db.end()) {
            found = true;
            return transaction_db[key];
        }
        if (main_db.find(key) != main_db.end()) {
            found = true;
            return main_db[key];
        }
        found = false;
        return 0;
    }

    void put(const string& key, int value) {
        if (!transaction_active) {
            throw runtime_error("No active transaction.");
        }
        transaction_db[key] = value;
    }

    void begin_transaction() {
        if (transaction_active) {
            throw runtime_error("Transaction already in progress.");
        }
        transaction_active = true;
        transaction_db.clear();
    }

    void commit() {
        if (!transaction_active) {
            throw runtime_error("No active transaction to commit.");
        }
        for (const auto& [key, value] : transaction_db) {
            main_db[key] = value;
        }
        transaction_active = false;
        transaction_db.clear();
    }

    void rollback() {
        if (!transaction_active) {
            throw runtime_error("No active transaction to rollback.");
        }
        transaction_active = false;
        transaction_db.clear();
    }
};

int main() {
    InMemoryDB db;

    try {
        bool found;
        cout << "Get A: " << (db.get("A", found), (found ? to_string(db.get("A", found)) : "null")) << endl;

        try {
            db.put("A", 5);
        } catch (const exception& e) {
            cout << e.what() << endl;
        }

        db.begin_transaction();

        db.put("A", 5);
        cout << "Get A during transaction: " << (db.get("A", found), (found ? to_string(db.get("A", found)) : "null")) << endl;

        db.put("A", 10);
        cout << "Get A updated in transaction: " << (db.get("A", found), (found ? to_string(db.get("A", found)) : "null")) << endl;

        db.commit();
        cout << "Get A after commit: " << (db.get("A", found), (found ? to_string(db.get("A", found)) : "null")) << endl;

        try {
            db.rollback();
        } catch (const exception& e) {
            cout << e.what() << endl;
        }

        db.begin_transaction();
        db.put("B", 20);
        cout << "Get B during transaction: " << (db.get("B", found), (found ? to_string(db.get("B", found)) : "null")) << endl;
        db.rollback();
        cout << "Get B after rollback: " << (db.get("B", found), (found ? to_string(db.get("B", found)) : "null")) << endl;

    } catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }

    return 0;
}

