/*
================================================================================
                    LIBRARY MANAGEMENT SYSTEM
================================================================================
Author: Chaman Yadav
Date: 2025
Description: Complete Library Management System using OOP concepts in C++
             with binary file storage (.dat files)

Features:
- Book Management (Add, Remove, Search, Display)
- User Management (Register, Remove, Display)  
- Issue/Return Books with Date Tracking
- Fine Calculation (Rs. 2/day after 14 days)
- Binary File Storage (books.dat, users.dat)
- Complete Error Handling and Validation

OOP Concepts Used:
- Encapsulation: Private data with controlled access
- Inheritance: DigitalBook inherits from Book
- Polymorphism: Virtual functions
- Abstraction: Clean interfaces
- Composition: Library contains Books and Users

Files Created Automatically:
- books.dat (stores all book data)
- users.dat (stores all user data)

================================================================================
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;

// ============================================================================
//                              BOOK CLASS
// ============================================================================

class Book {
private:
    char bookID[20];        // Unique book identifier
    char title[100];        // Book title
    char author[50];        // Book author
    int totalCopies;        // Total number of copies
    int availableCopies;    // Available copies for borrowing
    bool isActive;          // Book status (active/inactive)

public:
    // Default Constructor
    Book() {
        strcpy(bookID, "");
        strcpy(title, "");
        strcpy(author, "");
        totalCopies = 0;
        availableCopies = 0;
        isActive = true;
    }
    
    // Parameterized Constructor
    Book(const string& id, const string& bookTitle, const string& bookAuthor, int copies) {
        // Safe string copying to prevent buffer overflow
        strncpy(bookID, id.c_str(), sizeof(bookID) - 1);
        bookID[sizeof(bookID) - 1] = '\0';
        
        strncpy(title, bookTitle.c_str(), sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
        
        strncpy(author, bookAuthor.c_str(), sizeof(author) - 1);
        author[sizeof(author) - 1] = '\0';
        
        totalCopies = copies;
        availableCopies = copies;
        isActive = true;
    }
    
    // Getter Methods (Encapsulation)
    string getBookID() const { return string(bookID); }
    string getTitle() const { return string(title); }
    string getAuthor() const { return string(author); }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }
    bool getIsActive() const { return isActive; }
    
    // Setter Methods (Encapsulation)
    void setBookID(const string& id) {
        strncpy(bookID, id.c_str(), sizeof(bookID) - 1);
        bookID[sizeof(bookID) - 1] = '\0';
    }
    
    void setTitle(const string& bookTitle) {
        strncpy(title, bookTitle.c_str(), sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
    }
    
    void setAuthor(const string& bookAuthor) {
        strncpy(author, bookAuthor.c_str(), sizeof(author) - 1);
        author[sizeof(author) - 1] = '\0';
    }
    
    void setTotalCopies(int copies) { totalCopies = copies; }
    void setAvailableCopies(int copies) { availableCopies = copies; }
    void setIsActive(bool active) { isActive = active; }
    
    // Display book details with formatting
    virtual void displayDetails() const {
        cout << "\n" << string(50, '=') << endl;
        cout << "Book ID: " << bookID << endl;
        cout << "Title: " << title << endl;
        cout << "Author: " << author << endl;
        cout << "Total Copies: " << totalCopies << endl;
        cout << "Available Copies: " << availableCopies << endl;
        cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
        cout << string(50, '=') << endl;
    }
    
    // Check if book is available for borrowing
    bool isAvailable() const {
        return isActive && availableCopies > 0;
    }
    
    // Borrow a book (decrease available copies)
    bool borrowBook() {
        if (isAvailable()) {
            availableCopies--;
            return true;
        }
        return false;
    }
    
    // Return a book (increase available copies)
    bool returnBook() {
        if (availableCopies < totalCopies) {
            availableCopies++;
            return true;
        }
        return false;
    }
    
    // Operator overloading for comparison
    bool operator==(const string& id) const {
        return string(bookID) == id;
    }
};

// ============================================================================
//                          DIGITAL BOOK CLASS (INHERITANCE)
// ============================================================================

class DigitalBook : public Book {
private:
    char downloadLink[200];  // Download URL for digital book
    int downloadLimit;       // Maximum download attempts
    
public:
    // Default Constructor
    DigitalBook() : Book() {
        strcpy(downloadLink, "");
        downloadLimit = 0;
    }
    
    // Parameterized Constructor
    DigitalBook(const string& id, const string& title, const string& author, 
                int copies, const string& link, int limit) 
        : Book(id, title, author, copies) {
        strncpy(downloadLink, link.c_str(), sizeof(downloadLink) - 1);
        downloadLink[sizeof(downloadLink) - 1] = '\0';
        downloadLimit = limit;
    }
    
    // Setter and Getter for digital book specific attributes
    void setDownloadLink(const string& link) {
        strncpy(downloadLink, link.c_str(), sizeof(downloadLink) - 1);
        downloadLink[sizeof(downloadLink) - 1] = '\0';
    }
    
    void setDownloadLimit(int limit) { downloadLimit = limit; }
    string getDownloadLink() const { return string(downloadLink); }
    int getDownloadLimit() const { return downloadLimit; }
    
    // Polymorphism: Override displayDetails method
    void displayDetails() const override {
        Book::displayDetails();  // Call parent class method
        cout << "Download Link: " << downloadLink << endl;
        cout << "Download Limit: " << downloadLimit << endl;
        cout << string(50, '=') << endl;
    }
};

// ============================================================================
//                          BORROW RECORD STRUCTURE
// ============================================================================

struct BorrowRecord {
    char bookID[20];        // ID of borrowed book
    char borrowDate[12];    // Borrow date (DD/MM/YYYY)
    char returnDate[12];    // Return date (DD/MM/YYYY)
    bool isReturned;        // Return status
    
    // Default Constructor
    BorrowRecord() {
        strcpy(bookID, "");
        strcpy(borrowDate, "");
        strcpy(returnDate, "");
        isReturned = false;
    }
    
    // Parameterized Constructor
    BorrowRecord(const string& id, const string& bDate) {
        strncpy(bookID, id.c_str(), sizeof(bookID) - 1);
        bookID[sizeof(bookID) - 1] = '\0';
        
        strncpy(borrowDate, bDate.c_str(), sizeof(borrowDate) - 1);
        borrowDate[sizeof(borrowDate) - 1] = '\0';
        
        strcpy(returnDate, "");
        isReturned = false;
    }
};

// ============================================================================
//                              USER CLASS
// ============================================================================

class User {
private:
    char userID[20];                    // Unique user identifier
    char name[50];                      // User's full name
    char email[50];                     // User's email address
    char phone[15];                     // User's phone number
    BorrowRecord borrowedBooks[10];     // Array of borrowed books (max 10)
    int borrowCount;                    // Current number of borrowed books
    bool isActive;                      // User status (active/inactive)
    
public:
    // Default Constructor
    User() {
        strcpy(userID, "");
        strcpy(name, "");
        strcpy(email, "");
        strcpy(phone, "");
        borrowCount = 0;
        isActive = true;
    }
    
    // Parameterized Constructor
    User(const string& id, const string& userName, const string& userEmail, const string& userPhone) {
        strncpy(userID, id.c_str(), sizeof(userID) - 1);
        userID[sizeof(userID) - 1] = '\0';
        
        strncpy(name, userName.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        
        strncpy(email, userEmail.c_str(), sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
        
        strncpy(phone, userPhone.c_str(), sizeof(phone) - 1);
        phone[sizeof(phone) - 1] = '\0';
        
        borrowCount = 0;
        isActive = true;
    }
    
    // Getter Methods (Encapsulation)
    string getUserID() const { return string(userID); }
    string getName() const { return string(name); }
    string getEmail() const { return string(email); }
    string getPhone() const { return string(phone); }
    int getBorrowCount() const { return borrowCount; }
    bool getIsActive() const { return isActive; }
    
    // Setter Methods (Encapsulation)
    void setUserID(const string& id) {
        strncpy(userID, id.c_str(), sizeof(userID) - 1);
        userID[sizeof(userID) - 1] = '\0';
    }
    
    void setName(const string& userName) {
        strncpy(name, userName.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }
    
    void setEmail(const string& userEmail) {
        strncpy(email, userEmail.c_str(), sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
    }
    
    void setPhone(const string& userPhone) {
        strncpy(phone, userPhone.c_str(), sizeof(phone) - 1);
        phone[sizeof(phone) - 1] = '\0';
    }
    
    void setIsActive(bool active) { isActive = active; }
    
    // Borrow a book (add to user's borrowed list)
    bool borrowBook(const string& bookID, const string& borrowDate) {
        // Check borrowing limit
        if (borrowCount >= 10) {
            cout << "Cannot borrow more than 10 books at a time!" << endl;
            return false;
        }
        
        // Check if book is already borrowed by this user
        if (hasBorrowedBook(bookID)) {
            cout << "Book already borrowed by this user!" << endl;
            return false;
        }
        
        // Add book to borrowed list
        borrowedBooks[borrowCount] = BorrowRecord(bookID, borrowDate);
        borrowCount++;
        return true;
    }
    
    // Return a book (mark as returned)
    bool returnBook(const string& bookID, const string& returnDate) {
        for (int i = 0; i < borrowCount; i++) {
            if (string(borrowedBooks[i].bookID) == bookID && !borrowedBooks[i].isReturned) {
                strncpy(borrowedBooks[i].returnDate, returnDate.c_str(), sizeof(borrowedBooks[i].returnDate) - 1);
                borrowedBooks[i].returnDate[sizeof(borrowedBooks[i].returnDate) - 1] = '\0';
                borrowedBooks[i].isReturned = true;
                return true;
            }
        }
        return false;
    }
    
    // Display all borrowed books for this user
    void listBorrowedBooks() const {
        cout << "\n" << string(60, '=') << endl;
        cout << "Borrowed Books for User: " << name << " (ID: " << userID << ")" << endl;
        cout << string(60, '=') << endl;
        
        bool hasActiveBorrows = false;
        for (int i = 0; i < borrowCount; i++) {
            if (!borrowedBooks[i].isReturned) {
                cout << "Book ID: " << borrowedBooks[i].bookID << endl;
                cout << "Borrow Date: " << borrowedBooks[i].borrowDate << endl;
                cout << "Status: Not Returned" << endl;
                cout << string(30, '-') << endl;
                hasActiveBorrows = true;
            }
        }
        
        if (!hasActiveBorrows) {
            cout << "No active borrowed books." << endl;
        }
        cout << string(60, '=') << endl;
    }
    
    // Check if user has borrowed a specific book
    bool hasBorrowedBook(const string& bookID) const {
        for (int i = 0; i < borrowCount; i++) {
            if (string(borrowedBooks[i].bookID) == bookID && !borrowedBooks[i].isReturned) {
                return true;
            }
        }
        return false;
    }
    
    // Get borrow date for a specific book
    string getBorrowDate(const string& bookID) const {
        for (int i = 0; i < borrowCount; i++) {
            if (string(borrowedBooks[i].bookID) == bookID && !borrowedBooks[i].isReturned) {
                return string(borrowedBooks[i].borrowDate);
            }
        }
        return "";
    }
    
    // Display user details
    void displayDetails() const {
        cout << "\n" << string(50, '=') << endl;
        cout << "User ID: " << userID << endl;
        cout << "Name: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Phone: " << phone << endl;
        cout << "Active Borrowed Books: " << borrowCount << endl;
        cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
        cout << string(50, '=') << endl;
    }
    
    // Operator overloading for comparison
    bool operator==(const string& id) const {
        return string(userID) == id;
    }
};

// ============================================================================
//                              LIBRARY CLASS
// ============================================================================

class Library {
private:
    vector<Book> books;                 // Collection of all books
    vector<User> users;                 // Collection of all users
    const string BOOKS_FILE = "books.dat";  // Binary file for books
    const string USERS_FILE = "users.dat";  // Binary file for users
    
    // Helper function to calculate days between two dates
    int calculateDays(const string& date1, const string& date2) const {
        // Simple date calculation (DD/MM/YYYY format)
        // Note: This is simplified - real implementation should use proper date library
        struct tm tm1 = {}, tm2 = {};
        
        sscanf(date1.c_str(), "%d/%d/%d", &tm1.tm_mday, &tm1.tm_mon, &tm1.tm_year);
        sscanf(date2.c_str(), "%d/%d/%d", &tm2.tm_mday, &tm2.tm_mon, &tm2.tm_year);
        
        tm1.tm_mon--; tm2.tm_mon--;           // Month is 0-based
        tm1.tm_year -= 1900; tm2.tm_year -= 1900;  // Year since 1900
        
        time_t time1 = mktime(&tm1);
        time_t time2 = mktime(&tm2);
        
        return (int)difftime(time2, time1) / (24 * 3600);
    }
    
    // Validate date format (DD/MM/YYYY)
    bool isValidDate(const string& date) const {
        if (date.length() != 10) return false;
        if (date[2] != '/' || date[5] != '/') return false;
        
        int day, month, year;
        if (sscanf(date.c_str(), "%d/%d/%d", &day, &month, &year) != 3) return false;
        
        return (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 1900);
    }
    
    // Get current system date
    string getCurrentDate() const {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        
        char buffer[11];
        sprintf(buffer, "%02d/%02d/%04d", 
                ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
        
        return string(buffer);
    }
    
public:
    // Constructor - Load data from files
    Library() {
        cout << "Initializing Library Management System..." << endl;
        loadFromFile();
        cout << "System ready!" << endl;
    }
    
    // Destructor - Save data to files
    ~Library() {
        cout << "Saving data and shutting down..." << endl;
        saveToFile();
    }
    
    // ========================================================================
    //                          BOOK MANAGEMENT
    // ========================================================================
    
    // Add a new book to the library
    bool addBook(const Book& book) {
        // Check if book ID already exists (prevent duplicates)
        for (const auto& b : books) {
            if (b.getBookID() == book.getBookID()) {
                cout << "Error: Book with ID " << book.getBookID() << " already exists!" << endl;
                return false;
            }
        }
        
        books.push_back(book);
        cout << "Book added successfully!" << endl;
        saveToFile();  // Auto-save after each operation
        return true;
    }
    
    // Remove a book from the library
    bool removeBook(const string& bookID) {
        auto it = find_if(books.begin(), books.end(), 
                         [&bookID](const Book& b) { return b.getBookID() == bookID; });
        
        if (it == books.end()) {
            cout << "Error: Book with ID " << bookID << " not found!" << endl;
            return false;
        }
        
        // Check if all copies are available (none borrowed)
        if (it->getAvailableCopies() != it->getTotalCopies()) {
            cout << "Error: Cannot remove book. Some copies are currently borrowed!" << endl;
            return false;
        }
        
        books.erase(it);
        cout << "Book removed successfully!" << endl;
        saveToFile();
        return true;
    }
    
    // Search books by title, author, or book ID
    vector<Book> searchBook(const string& query) const {
        vector<Book> results;
        string lowerQuery = query;
        transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        for (const auto& book : books) {
            string title = book.getTitle();
            string author = book.getAuthor();
            string bookID = book.getBookID();
            
            // Convert to lowercase for case-insensitive search
            transform(title.begin(), title.end(), title.begin(), ::tolower);
            transform(author.begin(), author.end(), author.begin(), ::tolower);
            transform(bookID.begin(), bookID.end(), bookID.begin(), ::tolower);
            
            // Check if query matches any field
            if (title.find(lowerQuery) != string::npos || 
                author.find(lowerQuery) != string::npos || 
                bookID.find(lowerQuery) != string::npos) {
                results.push_back(book);
            }
        }
        
        return results;
    }
    
    // Find a specific book by ID
    Book* findBook(const string& bookID) {
        auto it = find_if(books.begin(), books.end(), 
                         [&bookID](const Book& b) { return b.getBookID() == bookID; });
        
        return (it != books.end()) ? &(*it) : nullptr;
    }
    
    // Display all books in the library
    void displayAllBooks() const {
        if (books.empty()) {
            cout << "No books in the library!" << endl;
            return;
        }
        
        cout << "\n" << string(80, '=') << endl;
        cout << "ALL BOOKS IN LIBRARY (" << books.size() << " books)" << endl;
        cout << string(80, '=') << endl;
        
        for (const auto& book : books) {
            book.displayDetails();
        }
    }
    
    // ========================================================================
    //                          USER MANAGEMENT
    // ========================================================================
    
    // Register a new user
    bool registerUser(const User& user) {
        // Check if user ID already exists (prevent duplicates)
        for (const auto& u : users) {
            if (u.getUserID() == user.getUserID()) {
                cout << "Error: User with ID " << user.getUserID() << " already exists!" << endl;
                return false;
            }
        }
        
        users.push_back(user);
        cout << "User registered successfully!" << endl;
        saveToFile();
        return true;
    }
    
    // Remove a user from the system
    bool removeUser(const string& userID) {
        auto it = find_if(users.begin(), users.end(), 
                         [&userID](const User& u) { return u.getUserID() == userID; });
        
        if (it == users.end()) {
            cout << "Error: User with ID " << userID << " not found!" << endl;
            return false;
        }
        
        // Check if user has any borrowed books
        if (it->getBorrowCount() > 0) {
            cout << "Error: Cannot remove user. User has borrowed books!" << endl;
            return false;
        }
        
        users.erase(it);
        cout << "User removed successfully!" << endl;
        saveToFile();
        return true;
    }
    
    // Find a specific user by ID
    User* findUser(const string& userID) {
        auto it = find_if(users.begin(), users.end(), 
                         [&userID](const User& u) { return u.getUserID() == userID; });
        
        return (it != users.end()) ? &(*it) : nullptr;
    }
    
    // Display all registered users
    void displayAllUsers() const {
        if (users.empty()) {
            cout << "No users registered!" << endl;
            return;
        }
        
        cout << "\n" << string(80, '=') << endl;
        cout << "ALL REGISTERED USERS (" << users.size() << " users)" << endl;
        cout << string(80, '=') << endl;
        
        for (const auto& user : users) {
            user.displayDetails();
        }
    }
    
    // ========================================================================
    //                          BOOK OPERATIONS
    // ========================================================================
    
    // Issue a book to a user
    bool issueBook(const string& userID, const string& bookID) {
        User* user = findUser(userID);
        Book* book = findBook(bookID);
        
        // Validate user exists
        if (!user) {
            cout << "Error: User with ID " << userID << " not found!" << endl;
            return false;
        }
        
        // Validate book exists
        if (!book) {
            cout << "Error: Book with ID " << bookID << " not found!" << endl;
            return false;
        }
        
        // Check if book is available
        if (!book->isAvailable()) {
            cout << "Error: Book is not available for borrowing!" << endl;
            return false;
        }
        
        // Check if user already has this book
        if (user->hasBorrowedBook(bookID)) {
            cout << "Error: User has already borrowed this book!" << endl;
            return false;
        }
        
        string currentDate = getCurrentDate();
        
        // Issue the book (update both user and book records)
        if (user->borrowBook(bookID, currentDate) && book->borrowBook()) {
            cout << "Book issued successfully!" << endl;
            cout << "Issue Date: " << currentDate << endl;
            cout << "Return Date: Please return within 14 days to avoid fine." << endl;
            saveToFile();
            return true;
        }
        
        cout << "Error: Failed to issue book!" << endl;
        return false;
    }
    
    // Return a book from a user
    bool returnBook(const string& userID, const string& bookID, const string& returnDate = "") {
        User* user = findUser(userID);
        Book* book = findBook(bookID);
        
        // Validate user exists
        if (!user) {
            cout << "Error: User with ID " << userID << " not found!" << endl;
            return false;
        }
        
        // Validate book exists
        if (!book) {
            cout << "Error: Book with ID " << bookID << " not found!" << endl;
            return false;
        }
        
        // Check if user has borrowed this book
        if (!user->hasBorrowedBook(bookID)) {
            cout << "Error: User has not borrowed this book!" << endl;
            return false;
        }
        
        // Use current date if no return date provided
        string actualReturnDate = returnDate.empty() ? getCurrentDate() : returnDate;
        
        // Validate return date format
        if (!isValidDate(actualReturnDate)) {
            cout << "Error: Invalid return date format! Use DD/MM/YYYY" << endl;
            return false;
        }
        
        string borrowDate = user->getBorrowDate(bookID);
        double fine = calculateFine(borrowDate, actualReturnDate);
        
        // Return the book (update both user and book records)
        if (user->returnBook(bookID, actualReturnDate) && book->returnBook()) {
            cout << "Book returned successfully!" << endl;
            cout << "Return Date: " << actualReturnDate << endl;
            
            if (fine > 0) {
                cout << "Fine Amount: Rs. " << fixed << setprecision(2) << fine << endl;
            } else {
                cout << "No fine applicable." << endl;
            }
            
            saveToFile();
            return true;
        }
        
        cout << "Error: Failed to return book!" << endl;
        return false;
    }
    
    // Calculate fine for overdue books
    double calculateFine(const string& issueDate, const string& returnDate) const {
        int days = calculateDays(issueDate, returnDate);
        int overdueDays = days - 14;  // 14-day free period
        
        return (overdueDays > 0) ? overdueDays * 2.0 : 0.0;  // Rs. 2 per day
    }
    
    // ========================================================================
    //                          FILE OPERATIONS
    // ========================================================================
    
    // Load data from binary files
    bool loadFromFile() {
        // Load books from books.dat
        ifstream bookFile(BOOKS_FILE, ios::binary);
        if (bookFile.is_open()) {
            Book book;
            while (bookFile.read(reinterpret_cast<char*>(&book), sizeof(Book))) {
                books.push_back(book);
            }
            bookFile.close();
            cout << "Loaded " << books.size() << " books from " << BOOKS_FILE << endl;
        } else {
            cout << "No existing book data found. Starting fresh." << endl;
        }
        
        // Load users from users.dat
        ifstream userFile(USERS_FILE, ios::binary);
        if (userFile.is_open()) {
            User user;
            while (userFile.read(reinterpret_cast<char*>(&user), sizeof(User))) {
                users.push_back(user);
            }
            userFile.close();
            cout << "Loaded " << users.size() << " users from " << USERS_FILE << endl;
        } else {
            cout << "No existing user data found. Starting fresh." << endl;
        }
        
        return true;
    }
    
    // Save data to binary files
    bool saveToFile() {
        // Save books to books.dat
        ofstream bookFile(BOOKS_FILE, ios::binary);
        if (bookFile.is_open()) {
            for (const auto& book : books) {
                bookFile.write(reinterpret_cast<const char*>(&book), sizeof(Book));
            }
            bookFile.close();
        } else {
            cout << "Error: Could not save books to file!" << endl;
            return false;
        }
        
        // Save users to users.dat
        ofstream userFile(USERS_FILE, ios::binary);
        if (userFile.is_open()) {
            for (const auto& user : users) {
                userFile.write(reinterpret_cast<const char*>(&user), sizeof(User));
            }
            userFile.close();
        } else {
            cout << "Error: Could not save users to file!" << endl;
            return false;
        }
        
        return true;
    }
    
    // ========================================================================
    //                          STATISTICS & REPORTS
    // ========================================================================
    
    // Display library statistics
    void displayStatistics() const {
        int totalBooks = books.size();
        int availableBooks = 0;
        int borrowedBooks = 0;
        
        for (const auto& book : books) {
            availableBooks += book.getAvailableCopies();
            borrowedBooks += (book.getTotalCopies() - book.getAvailableCopies());
        }
        
        cout << "\n" << string(60, '=') << endl;
        cout << "LIBRARY STATISTICS" << endl;
        cout << string(60, '=') << endl;
        cout << "Total Book Titles: " << totalBooks << endl;
        cout << "Total Available Copies: " << availableBooks << endl;
        cout << "Total Borrowed Copies: " << borrowedBooks << endl;
        cout << "Total Registered Users: " << users.size() << endl;
        cout << string(60, '=') << endl;
    }
};

// ============================================================================
//                          MAIN PROGRAM & MENU SYSTEM
// ============================================================================

class LibrarySystem {
private:
    Library library;  // Main library object (Composition)
    
    // Clear screen function (cross-platform)
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    // Pause screen and wait for user input
    void pauseScreen() {
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
    // Display main menu
    void displayMainMenu() {
        cout << "\n" << string(60, '=') << endl;
        cout << "           LIBRARY MANAGEMENT SYSTEM" << endl;
        cout << string(60, '=') << endl;
        cout << "1.  Add New Book" << endl;
        cout << "2.  Remove Book" << endl;
        cout << "3.  Search Books" << endl;
        cout << "4.  Display All Books" << endl;
        cout << "5.  Register New User" << endl;
        cout << "6.  Remove User" << endl;
        cout << "7.  Display All Users" << endl;
        cout << "8.  Issue Book" << endl;
        cout << "9.  Return Book" << endl;
        cout << "10. View User's Borrowed Books" << endl;
        cout << "11. Library Statistics" << endl;
        cout << "0.  Exit" << endl;
        cout << string(60, '=') << endl;
        cout << "Enter your choice: ";
    }
    
    // Add new book menu
    void addBookMenu() {
        string bookID, title, author;
        int copies;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "ADD NEW BOOK" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter Book ID: ";
        getline(cin, bookID);
        
        cout << "Enter Title: ";
        getline(cin, title);
        
        cout << "Enter Author: ";
        getline(cin, author);
        
        cout << "Enter Number of Copies: ";
        cin >> copies;
        
        if (copies <= 0) {
            cout << "Error: Number of copies must be positive!" << endl;
            return;
        }
        
        Book newBook(bookID, title, author, copies);
        library.addBook(newBook);
    }
    
    // Remove book menu
    void removeBookMenu() {
        string bookID;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "REMOVE BOOK" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter Book ID to remove: ";
        getline(cin, bookID);
        
        library.removeBook(bookID);
    }
    
    // Search books menu
    void searchBookMenu() {
        string query;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "SEARCH BOOKS" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter search query (Title/Author/Book ID): ";
        getline(cin, query);
        
        vector<Book> results = library.searchBook(query);
        
        if (results.empty()) {
            cout << "No books found matching your search!" << endl;
        } else {
            cout << "\nSearch Results (" << results.size() << " found):" << endl;
            for (const auto& book : results) {
                book.displayDetails();
            }
        }
    }
    
    // Register new user menu
    void registerUserMenu() {
        string userID, name, email, phone;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "REGISTER NEW USER" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter User ID: ";
        getline(cin, userID);
        
        cout << "Enter Name: ";
        getline(cin, name);
        
        cout << "Enter Email: ";
        getline(cin, email);
        
        cout << "Enter Phone: ";
        getline(cin, phone);
        
        User newUser(userID, name, email, phone);
        library.registerUser(newUser);
    }
    
    // Remove user menu
    void removeUserMenu() {
        string userID;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "REMOVE USER" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter User ID to remove: ";
        getline(cin, userID);
        
        library.removeUser(userID);
    }
    
    // Issue book menu
    void issueBookMenu() {
        string userID, bookID;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "ISSUE BOOK" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter User ID: ";
        getline(cin, userID);
        
        cout << "Enter Book ID: ";
        getline(cin, bookID);
        
        library.issueBook(userID, bookID);
    }
    
    // Return book menu
    void returnBookMenu() {
        string userID, bookID, returnDate;
        char useCurrentDate;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "RETURN BOOK" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter User ID: ";
        getline(cin, userID);
        
        cout << "Enter Book ID: ";
        getline(cin, bookID);
        
        cout << "Use current date as return date? (y/n): ";
        cin >> useCurrentDate;
        
        if (useCurrentDate == 'n' || useCurrentDate == 'N') {
            cin.ignore();
            cout << "Enter Return Date (DD/MM/YYYY): ";
            getline(cin, returnDate);
            library.returnBook(userID, bookID, returnDate);
        } else {
            library.returnBook(userID, bookID);
        }
    }
    
    // View borrowed books menu
    void viewBorrowedBooksMenu() {
        string userID;
        
        cout << "\n" << string(40, '=') << endl;
        cout << "VIEW BORROWED BOOKS" << endl;
        cout << string(40, '=') << endl;
        
        cin.ignore();
        cout << "Enter User ID: ";
        getline(cin, userID);
        
        User* user = library.findUser(userID);
        if (user) {
            user->listBorrowedBooks();
        } else {
            cout << "User not found!" << endl;
        }
    }
    
public:
    // Main program loop
    void run() {
        int choice;
        
        cout << "Welcome to Library Management System!" << endl;
        cout << "Files will be created automatically: books.dat, users.dat" << endl;
        
        while (true) {
            clearScreen();
            displayMainMenu();
            
            // Input validation
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number." << endl;
                pauseScreen();
                continue;
            }
            
            // Menu selection
            switch (choice) {
                case 1:
                    addBookMenu();
                    break;
                case 2:
                    removeBookMenu();
                    break;
                case 3:
                    searchBookMenu();
                    break;
                case 4:
                    library.displayAllBooks();
                    break;
                case 5:
                    registerUserMenu();
                    break;
                case 6:
                    removeUserMenu();
                    break;
                case 7:
                    library.displayAllUsers();
                    break;
                case 8:
                    issueBookMenu();
                    break;
                case 9:
                    returnBookMenu();
                    break;
                case 10:
                    viewBorrowedBooksMenu();
                    break;
                case 11:
                    library.displayStatistics();
                    break;
                case 0:
                    cout << "\nThank you for using Library Management System!" << endl;
                    cout << "All data has been saved to books.dat and users.dat" << endl;
                    return;
                default:
                    cout << "Invalid choice! Please try again." << endl;
            }
            
            pauseScreen();
        }
    }
};

// ============================================================================
//                              MAIN FUNCTION
// ============================================================================

int main() {
    try {
        cout << "================================================================================\n";
        cout << "                    LIBRARY MANAGEMENT SYSTEM\n";
        cout << "================================================================================\n";
        cout << "Features: Book Management, User Management, Issue/Return, Fine Calculation\n";
        cout << "Data Storage: Binary files (books.dat, users.dat) - Created automatically\n";
        cout << "OOP Concepts: Encapsulation, Inheritance, Polymorphism, Abstraction\n";
        cout << "================================================================================\n\n";
        
        LibrarySystem system;
        system.run();
        
    } catch (const exception& e) {
        cout << "An error occurred: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

/*
================================================================================
                            COMPILATION & USAGE
================================================================================

COMPILATION:
-----------
Windows: g++ -std=c++11 library_management_system.cpp -o library_system.exe
Linux/Mac: g++ -std=c++11 library_management_system.cpp -o library_system

USAGE:
------
Windows: library_system.exe
Linux/Mac: ./library_system

FILES CREATED:
--------------
- books.dat: Binary file storing all book data
- users.dat: Binary file storing all user data

These files are created automatically when you first run the program.
Data persists between program runs.

SAMPLE WORKFLOW:
---------------
1. Run the program
2. Add some books (option 1)
3. Register some users (option 5)  
4. Issue books to users (option 8)
5. Return books (option 9)
6. View statistics (option 11)

The system handles all edge cases and provides comprehensive error checking.
================================================================================
*/
