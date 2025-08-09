// ================================================================================
// LIBRARY MANAGEMENT SYSTEM 
// ================================================================================
// Author: Chaman Yadav
// Description: Complete Library Management System with all features from the
//              original file-based version, now fully integrated with a
//              MySQL database backend.
// ================================================================================

// C++ Standard Libraries
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>

// MySQL Connector/C++ (X DevAPI)
#include <mysqlx/xdevapi.h>

// Use only the non-conflicting std names
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::getline;
using std::unique_ptr;
using std::make_unique;
using std::numeric_limits;
using std::streamsize;
using std::string; // Explicitly use std::string to avoid conflicts

// ============================================================================
// HELPER FUNCTIONS & DATA CLASSES
// ============================================================================

// Helper function to get the current date in YYYY-MM-DD format for SQL
string getCurrentDateForSQL() {
    time_t now = time(0);
    tm ltm;
#ifdef _WIN32
    localtime_s(&ltm, &now); // Use localtime_s for safety on Windows
#else
    localtime_r(&now, &ltm); // Use localtime_r for safety on POSIX systems
#endif
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &ltm);
    return string(buffer);
}

// Helper to calculate days between two "YYYY-MM-DD" dates
int calculateDays(const string& date1_str, const string& date2_str) {
    struct tm tm1 = {}, tm2 = {};
    std::stringstream ss1(date1_str);
    std::stringstream ss2(date2_str);

    ss1 >> std::get_time(&tm1, "%Y-%m-%d");
    ss2 >> std::get_time(&tm2, "%Y-%m-%d");

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 == -1 || time2 == -1) {
        return 0; // Invalid date format
    }

    return (int)difftime(time2, time1) / (24 * 3600);
}


class Book {
public:
    string bookID;
    string title;
    string author;
    int totalCopies;
    int availableCopies;
    bool isActive;
    string downloadLink;
    int downloadLimit;

    Book() = default;

    Book(string id, string t, string a, int total, int available, bool active = true, string link = "", int limit = 0)
        : bookID(id), title(t), author(a), totalCopies(total), availableCopies(available), isActive(active), downloadLink(link), downloadLimit(limit) {}

    void displayDetails() const {
        cout << "\n" << string(50, '=') << endl;
        cout << "Book ID: " << bookID << endl;
        cout << "Title: " << title << endl;
        cout << "Author: " << author << endl;
        cout << "Total Copies: " << totalCopies << endl;
        cout << "Available Copies: " << availableCopies << endl;
        cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
        if (!downloadLink.empty()) {
            cout << "Download Link: " << downloadLink << endl;
            cout << "Download Limit: " << downloadLimit << endl;
        }
        cout << string(50, '=') << endl;
    }
};

class User {
public:
    string userID;
    string name;
    string email;
    string phone;
    bool isActive;

    User() = default;

    User(string id, string n, string e, string p, bool active = true)
        : userID(id), name(n), email(e), phone(p), isActive(active) {}

    void displayDetails() const {
        cout << "\n" << string(50, '=') << endl;
        cout << "User ID: " << userID << endl;
        cout << "Name: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Phone: " << phone << endl;
        cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
        cout << string(50, '=') << endl;
    }
};

class BorrowRecord {
public:
    string bookID;
    string title;
    string borrowDate;

    BorrowRecord(string bID, string t, string bDate) : bookID(bID), title(t), borrowDate(bDate) {}
};


// ============================================================================
// DATABASE CLASS (Handles all SQL operations) 🛠️
// ============================================================================
class Database {
private:
    mysqlx::Session& sess;
    mysqlx::Schema db;
    mysqlx::Table books_table;
    mysqlx::Table users_table;
    mysqlx::Table borrow_records_table;

public:
    Database(mysqlx::Session& session) :
        sess(session),
        db(sess.getSchema("library_db")),
        books_table(db.getTable("books")),
        users_table(db.getTable("users")),
        borrow_records_table(db.getTable("borrow_records"))
    {}

    // --- Book Operations ---
    bool addBook(const Book& newBook) {
        try {
            books_table.insert("book_id", "title", "author", "total_copies", "available_copies", "download_link", "download_limit")
                .values(newBook.bookID, newBook.title, newBook.author, newBook.totalCopies, newBook.availableCopies, newBook.downloadLink, newBook.downloadLimit)
                .execute();
            return true;
        } catch (const mysqlx::Error&) {
            return false;
        }
    }

    bool removeBook(const string& bookID) {
        try {
            mysqlx::RowResult result = borrow_records_table.select("COUNT(*)")
                .where("book_id = :id AND is_returned = false")
                .bind("id", bookID)
                .execute();
            if (result.fetchOne()[0].get<int>() > 0) {
                cout << "Error: Cannot remove book. Some copies are currently borrowed." << endl;
                return false;
            }
            return books_table.remove().where("book_id = :id").bind("id", bookID).execute().getAffectedItemsCount() > 0;
        } catch (const mysqlx::Error& err) {
            cout << "Database error during book removal: " << err << endl;
            return false;
        }
    }

    unique_ptr<Book> findBook(const string& bookID) {
        mysqlx::RowResult result = books_table.select("*").where("book_id = :id").bind("id", bookID).execute();
        mysqlx::Row row = result.fetchOne();
        if (row) {
            return make_unique<Book>(
                row[0].get<string>(), row[1].get<string>(), row[2].get<string>(),
                row[3].get<int>(), row[4].get<int>(), row[5].get<bool>(),
                row[6].isNull() ? "" : row[6].get<string>(),
                row[7].isNull() ? 0 : row[7].get<int>()
            );
        }
        return nullptr;
    }

    vector<Book> searchBook(const string& query) {
        vector<Book> results;
        string likeQuery = "%" + query + "%";
        try {
            mysqlx::RowResult result = books_table.select("*")
                .where("title LIKE :query OR author LIKE :query OR book_id = :id")
                .bind("query", likeQuery)
                .bind("id", query)
                .execute();

            for (mysqlx::Row row : result.fetchAll()) {
                results.emplace_back(
                    row[0].get<string>(), row[1].get<string>(), row[2].get<string>(),
                    row[3].get<int>(), row[4].get<int>(), row[5].get<bool>(),
                    row[6].isNull() ? "" : row[6].get<string>(),
                    row[7].isNull() ? 0 : row[7].get<int>()
                );
            }
        } catch (const mysqlx::Error& err) {
             cout << "Database error during book search: " << err << endl;
        }
        return results;
    }

    vector<Book> getAllBooks() {
        vector<Book> allBooks;
        mysqlx::RowResult result = books_table.select("*").execute();
        for (mysqlx::Row row : result.fetchAll()) {
             allBooks.emplace_back(
                    row[0].get<string>(), row[1].get<string>(), row[2].get<string>(),
                    row[3].get<int>(), row[4].get<int>(), row[5].get<bool>(),
                    row[6].isNull() ? "" : row[6].get<string>(),
                    row[7].isNull() ? 0 : row[7].get<int>()
                );
        }
        return allBooks;
    }

    // --- User Operations ---
    bool addUser(const User& newUser) {
        try {
            users_table.insert("user_id", "name", "email", "phone")
                .values(newUser.userID, newUser.name, newUser.email, newUser.phone)
                .execute();
            return true;
        } catch (const mysqlx::Error&) {
            return false;
        }
    }

    bool removeUser(const string& userID) {
        try {
            mysqlx::RowResult result = borrow_records_table.select("COUNT(*)")
                .where("user_id = :id AND is_returned = false")
                .bind("id", userID)
                .execute();
            if (result.fetchOne()[0].get<int>() > 0) {
                cout << "Error: Cannot remove user. User has unreturned books." << endl;
                return false;
            }
            return users_table.remove().where("user_id = :id").bind("id", userID).execute().getAffectedItemsCount() > 0;
        } catch (const mysqlx::Error& err) {
            cout << "Database error during user removal: " << err << endl;
            return false;
        }
    }

    unique_ptr<User> findUser(const string& userID) {
        mysqlx::RowResult result = users_table.select("*").where("user_id = :id").bind("id", userID).execute();
        mysqlx::Row row = result.fetchOne();
        if (row) {
            return make_unique<User>(
                row[0].get<string>(), row[1].get<string>(), row[2].get<string>(), 
                row[3].get<string>(), row[4].get<bool>()
            );
        }
        return nullptr;
    }

    vector<User> getAllUsers() {
        vector<User> allUsers;
        mysqlx::RowResult result = users_table.select("*").execute();
        for (mysqlx::Row row : result.fetchAll()) {
            allUsers.emplace_back(
                 row[0].get<string>(), row[1].get<string>(), row[2].get<string>(), 
                 row[3].get<string>(), row[4].get<bool>()
            );
        }
        return allUsers;
    }

    // --- Borrowing Operations ---
    bool isBookAlreadyBorrowedByUser(const string& userID, const string& bookID){
        mysqlx::RowResult result = borrow_records_table.select("COUNT(*)")
            .where("user_id = :uid AND book_id = :bid AND is_returned = false")
            .bind("uid", userID).bind("bid", bookID).execute();
        return result.fetchOne()[0].get<int>() > 0;
    }

    bool issueBook(const string& userID, const string& bookID) {
        try {
            sess.startTransaction();
            mysqlx::RowResult bookResult = books_table.select("available_copies")
                .where("book_id = :id AND available_copies > 0")
                .bind("id", bookID)
                .execute();
            if (!bookResult.fetchOne()) {
                cout << "Error: Book is not available for borrowing." << endl;
                sess.rollback();
                return false;
            }

            books_table.update().set("available_copies", mysqlx::expr("available_copies - 1")).where("book_id = :id").bind("id", bookID).execute();
            
            string today = getCurrentDateForSQL();
            borrow_records_table.insert("user_id", "book_id", "borrow_date").values(userID, bookID, today).execute();
            
            sess.commit();
            return true;

        } catch (const mysqlx::Error& err) {
            cout << "Database error during book issue: " << err << endl;
            sess.rollback();
            return false;
        }
    }
    
    // Returns pair: {success_status, borrow_date_string}
    std::pair<bool, string> returnBook(const string& userID, const string& bookID, const string& returnDate) {
        try {
             sess.startTransaction();
             mysqlx::RowResult borrowResult = borrow_records_table.select("borrow_date", "record_id")
                .where("user_id = :uid AND book_id = :bid AND is_returned = false")
                .bind("uid", userID).bind("bid", bookID).execute();
            
            mysqlx::Row row = borrowResult.fetchOne();
            if (!row) {
                cout << "Error: This book is not actively borrowed by this user." << endl;
                sess.rollback();
                return {false, ""};
            }
            string borrowDate = row[0].get<string>();
            int recordId = row[1].get<int>();

            borrow_records_table.update().set("is_returned", true).set("return_date", returnDate)
                .where("record_id = :rid").bind("rid", recordId).execute();

            books_table.update().set("available_copies", mysqlx::expr("available_copies + 1"))
                .where("book_id = :bid").bind("bid", bookID).execute();
            
            sess.commit();
            return {true, borrowDate};

        } catch (const mysqlx::Error& err) {
            cout << "Database error during book return: " << err << endl;
            sess.rollback();
            return {false, ""};
        }
    }
vector<BorrowRecord> getBorrowedBooksForUser(const string& userID) {
    vector<BorrowRecord> records;
    try {
        // CORRECTION: Use CAST(.. AS CHAR) to force the database to format the date.
        // This ensures C++ always receives a readable string.
        mysqlx::SqlResult result = sess.sql(
            "SELECT br.book_id, b.title, CAST(br.borrow_date AS CHAR) "
            "FROM borrow_records AS br "
            "JOIN books AS b ON br.book_id = b.book_id "
            "WHERE br.user_id = ? AND br.is_returned = false"
        ).bind(userID).execute();

        for (mysqlx::Row row : result.fetchAll()) {
            // Now we can safely get the date as a string because the DB already converted it.
            records.emplace_back(
                row[0].get<string>(), // book_id
                row[1].get<string>(), // title
                row[2].get<string>()  // borrow_date (now a proper string)
            );
        }
    } catch (const mysqlx::Error& err) {
        cout << "Database error while fetching borrowed books: " << err << endl;
    }
    return records;
}

void getStatistics(int& totalTitles, int& availableCopies, int& borrowedCopies, int& totalUsers) {
    try {
        // FINAL CORRECTION: Use CAST(... AS SIGNED) to force the database to return
        // a standard integer type. This resolves the final conversion error.
        
        // Initialize all values to a default of 0.
        totalTitles = 0;
        availableCopies = 0;
        borrowedCopies = 0;
        totalUsers = 0;

        mysqlx::Row row;

        // Get total book titles (COUNT is always an integer)
        row = sess.sql("SELECT COUNT(*) FROM books").execute().fetchOne();
        if (row) totalTitles = row[0].get<int>();

        // Get available copies, casting the result to a signed integer
        row = sess.sql("SELECT CAST(COALESCE(SUM(available_copies), 0) AS SIGNED) FROM books").execute().fetchOne();
        if (row) availableCopies = row[0].get<int64_t>();

        // Get total copies to calculate borrowed count, casting the result
        row = sess.sql("SELECT CAST(COALESCE(SUM(total_copies), 0) AS SIGNED) FROM books").execute().fetchOne();
        if (row) {
            int64_t totalCopies = row[0].get<int64_t>();
            borrowedCopies = totalCopies - availableCopies;
        }
        
        // Get total registered users (COUNT is always an integer)
        row = sess.sql("SELECT COUNT(*) FROM users").execute().fetchOne();
        if (row) totalUsers = row[0].get<int>();

    } catch (const mysqlx::Error& err) {
         cout << "Database error while fetching statistics: " << err.what() << endl;
    }
}
};

// ============================================================================
// LIBRARY CLASS (Manages the application logic using the Database)
// ============================================================================
class Library {
private:
    Database db;

public:
    Library(mysqlx::Session& session) : db(session) {}

    void addBookMenu() {
        string id, title, author, link, isDigital;
        int copies, limit = 0;
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nEnter Book ID: ";      getline(cin, id);
        cout << "Enter Title: ";        getline(cin, title);
        cout << "Enter Author: ";       getline(cin, author);
        cout << "Enter Number of Copies: "; cin >> copies;
        
        cout << "Is this a digital book? (y/n): "; cin >> isDigital;
        if (isDigital == "y" || isDigital == "Y") {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter Download Link: "; getline(cin, link);
            cout << "Enter Download Limit: "; cin >> limit;
        }

        Book newBook(id, title, author, copies, copies, true, link, limit);
        if (db.addBook(newBook)) {
            cout << "Book added successfully!" << endl;
        } else {
            cout << "Error: Could not add book. ID might already exist." << endl;
        }
    }

    void removeBookMenu() {
        string bookID;
        cout << "\nEnter Book ID to remove: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, bookID);

        if (db.removeBook(bookID)) {
            cout << "Book removed successfully!" << endl;
        } else {
            cout << "Error: Could not remove book. Check if it exists or is borrowed." << endl;
        }
    }
    
    void searchBookMenu() {
        string query;
        cout << "\nEnter search query (Title/Author/Book ID): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, query);
        
        vector<Book> results = db.searchBook(query);
        if (results.empty()) {
            cout << "No books found matching your query." << endl;
        } else {
            cout << "\nSearch Results (" << results.size() << " found):" << endl;
            for (const auto& book : results) {
                book.displayDetails();
            }
        }
    }

    void displayAllBooks() {
        vector<Book> allBooks = db.getAllBooks();
        if (allBooks.empty()) {
            cout << "No books in the library." << endl;
            return;
        }
        cout << "\nALL BOOKS IN LIBRARY (" << allBooks.size() << " titles)" << endl;
        for (const auto& book : allBooks) {
            book.displayDetails();
        }
    }

    void registerUserMenu() {
        string id, name, email, phone;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nEnter User ID: ";    getline(cin, id);
        cout << "Enter Name: ";       getline(cin, name);
        cout << "Enter Email: ";      getline(cin, email);
        cout << "Enter Phone: ";      getline(cin, phone);

        User newUser(id, name, email, phone);
        if (db.addUser(newUser)) {
            cout << "User registered successfully!" << endl;
        } else {
            cout << "Error: Could not register user. ID or Email might already exist." << endl;
        }
    }
    
    void removeUserMenu() {
        string userID;
        cout << "\nEnter User ID to remove: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, userID);
        if (db.removeUser(userID)) {
            cout << "User removed successfully!" << endl;
        } else {
            cout << "Error: Could not remove user. Check if ID is correct or user has books." << endl;
        }
    }

    void displayAllUsers() {
        vector<User> allUsers = db.getAllUsers();
        if (allUsers.empty()) {
            cout << "No users registered." << endl;
            return;
        }
        cout << "\nALL REGISTERED USERS (" << allUsers.size() << " users)" << endl;
        for (const auto& user : allUsers) {
            user.displayDetails();
        }
    }

    void issueBookMenu() {
        string userID, bookID;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nEnter User ID: ";    getline(cin, userID);
        cout << "Enter Book ID: ";    getline(cin, bookID);
        
        if (!db.findUser(userID)) { cout << "Error: User not found." << endl; return; }
        if (!db.findBook(bookID)) { cout << "Error: Book not found." << endl; return; }
        if (db.isBookAlreadyBorrowedByUser(userID, bookID)) {
             cout << "Error: User has already borrowed this book." << endl;
             return;
        }
        
        if (db.issueBook(userID, bookID)) {
            cout << "Book issued successfully on " << getCurrentDateForSQL() << "!" << endl;
            cout << "Please return within 14 days to avoid a fine." << endl;
        } else {
            cout << "Failed to issue book." << endl;
        }
    }
    
    void returnBookMenu() {
        string userID, bookID;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nEnter User ID: ";    getline(cin, userID);
        cout << "Enter Book ID: ";    getline(cin, bookID);

        string returnDate = getCurrentDateForSQL();
        auto result = db.returnBook(userID, bookID, returnDate);

        if (result.first) { // if return was successful
            cout << "Book returned successfully on " << returnDate << "!" << endl;
            string borrowDate = result.second;
            int days = calculateDays(borrowDate, returnDate);
            if (days > 14) {
                double fine = (days - 14) * 2.0;
                cout << "Fine Applicable: Rs. " << std::fixed << std::setprecision(2) << fine << endl;
            } else {
                cout << "No fine applicable." << endl;
            }
        } else {
            cout << "Failed to return book. Please check the details." << endl;
        }
    }
    
    void viewBorrowedBooksMenu() {
        string userID;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nEnter User ID to view borrowed books: ";
        getline(cin, userID);
        
        auto user = db.findUser(userID);
        if (!user) {
            cout << "User not found!" << endl;
            return;
        }

        vector<BorrowRecord> records = db.getBorrowedBooksForUser(userID);
        cout << "\n" << string(60, '=') << endl;
        cout << "Borrowed Books for User: " << user->name << " (ID: " << user->userID << ")" << endl;
        cout << string(60, '=') << endl;
        
        if (records.empty()) {
            cout << "No active borrowed books." << endl;
        } else {
            for (const auto& record : records) {
                cout << "Book ID:     " << record.bookID << endl;
                cout << "Book Title:  " << record.title << endl;
                cout << "Borrow Date: " << record.borrowDate << " (YYYY-MM-DD)" << endl;
                cout << "Status:      Not Returned" << endl;
                cout << string(30, '-') << endl;
            }
        }
        cout << string(60, '=') << endl;
    }
    
    void displayStatistics() {
        int totalTitles = 0, availableCopies = 0, borrowedCopies = 0, totalUsers = 0;
        db.getStatistics(totalTitles, availableCopies, borrowedCopies, totalUsers);

        cout << "\n" << string(60, '=') << endl;
        cout << "LIBRARY STATISTICS" << endl;
        cout << string(60, '=') << endl;
        cout << "Total Book Titles: " << totalTitles << endl;
        cout << "Total Available Copies: " << availableCopies << endl;
        cout << "Total Borrowed Copies: " << borrowedCopies << endl;
        cout << "Total Registered Users: " << totalUsers << endl;
        cout << string(60, '=') << endl;
    }
};


// ============================================================================
// MENU SYSTEM CLASS (Handles user interaction)
// ============================================================================
class LibrarySystem {
private:
    Library library;

public:
    LibrarySystem(mysqlx::Session& session) : library(session) {}

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void pauseScreen() {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void displayMainMenu() {
        clearScreen();
        cout << "\n" << string(60, '=') << endl;
        cout << "         LIBRARY MANAGEMENT SYSTEM (DATABASE)" << endl;
        cout << string(60, '=') << endl;
        cout << " 1. Add New Book" << endl;
        cout << " 2. Remove Book" << endl;
        cout << " 3. Search Books" << endl;
        cout << " 4. Display All Books" << endl;
        cout << " 5. Register New User" << endl;
        cout << " 6. Remove User" << endl;
        cout << " 7. Display All Users" << endl;
        cout << " 8. Issue Book" << endl;
        cout << " 9. Return Book" << endl;
        cout << "10. View User's Borrowed Books" << endl;
        cout << "11. Library Statistics" << endl;
        cout << " 0. Exit" << endl;
        cout << string(60, '=') << endl;
        cout << "Enter your choice: ";
    }

    void run() {
        int choice;
        while (true) {
            displayMainMenu();
            cin >> choice;
            if (!cin) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number." << endl;
                pauseScreen();
                continue;
            }

            switch (choice) {
                case 1: library.addBookMenu(); break;
                case 2: library.removeBookMenu(); break;
                case 3: library.searchBookMenu(); break;
                case 4: library.displayAllBooks(); break;
                case 5: library.registerUserMenu(); break;
                case 6: library.removeUserMenu(); break;
                case 7: library.displayAllUsers(); break;
                case 8: library.issueBookMenu(); break;
                case 9: library.returnBookMenu(); break;
                case 10: library.viewBorrowedBooksMenu(); break;
                case 11: library.displayStatistics(); break;
                case 0:
                    cout << "\nThank you for using the system!" << endl;
                    return;
                default:
                    cout << "Invalid choice! Please try again." << endl;
            }
            pauseScreen();
        }
    }
};

// ============================================================================
// MAIN FUNCTION (Entry point of the program)
// ============================================================================
int main() {
    cout << "Attempting to connect to the database..." << endl;
    try {
        // ❗❗❗ IMPORTANT: Replace "your_password" with your actual MySQL password ❗❗❗
        mysqlx::Session sess("mysqlx://root:38113114@localhost/library_db");
        cout << "✅ Connection to the database is established." << endl;

        LibrarySystem system(sess);
        system.run();

    } catch (const mysqlx::Error& err) {
        cout << "❌ Database Error: " << err << endl;
        cout << "Please ensure the database server is running, the 'library_db' schema exists, and credentials are correct." << endl;
        cout << "Press Enter to exit.";
        cin.get();
        return 1;
    } catch (const std::exception& ex) {
        cout << "❌ A standard error occurred: " << ex.what() << endl;
        cout << "Press Enter to exit.";
        cin.get();
        return 1;
    }
    return 0;
}
