# Library Management System

![Library Management System](Demo_Images/startingpanel.png)  

A complete **Library Management System** implemented in C++ using **Object-Oriented Programming principles** and a **MySQL database backend** (via MySQL Connector/C++).  


---

## Table of Contents
- [Features](#features)
- [OOP Concepts Implemented](#oop-concepts-implemented)
- [System Architecture](#system-architecture)
- [Installation](#installation)
- [Database Setup](#database-setup)
- [Usage](#usage)
- [Database Structure](#database-structure)
- [Modules](#modules)
- [Screenshots](#screenshots)
- [Technical Specifications](#technical-specifications)
- [License](#license)

---

## Features

### Book Management
- Add new books with complete details
- Remove books (only if no active borrowings)
- Search by title, author, or ID
- Display all available books
- Track total and available copies
- Digital book support (download link & download limit)

### User Management
- Register new users
- Remove users (only if no active borrowings)
- Display all users
- View borrowing history

### Transaction System
- Issue books to registered users
- Return books with overdue fine calculation (₹2/day after 14 days)
- Borrowing limit enforcement

### Database Integration
- Persistent storage in MySQL
- Relational schema for books, users, and borrow records
- Transactions for issuing/returning books to maintain consistency

---

## OOP Concepts Implemented

| Concept          | Implementation Example                          |
|------------------|--------------------------------------------------|
| Encapsulation    | Private data members with public getters/setters |
| Inheritance      | `DigitalBook` derived from `Book` base class     |
| Polymorphism     | Virtual methods for dynamic behavior             |
| Abstraction      | Simplified menu & DB access layers               |
| Composition      | `Library` contains `Book` and `User` objects     |

---

## System Architecture

```
Library Management System
├── Core Classes
│   ├── Book / DigitalBook
│   ├── User
│   ├── BorrowRecord
│   ├── Database (MySQL interface)
│   ├── Library (Business logic)
│   └── LibrarySystem (Menu/UI)
├── Database (library_db)
│   ├── books
│   ├── users
│   └── borrow_records
└── MySQL Connector/C++
```

---

## Installation

### Prerequisites
- C++11 compatible compiler (e.g., g++)
- MySQL Server 5.7+ (or MariaDB equivalent)
- MySQL Connector/C++ (X DevAPI version)
- Standard C++ libraries


## Database Setup

Run the following SQL commands in MySQL:

```sql
CREATE DATABASE IF NOT EXISTS library_db;
USE library_db;

CREATE TABLE books (
    book_id VARCHAR(20) PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    author VARCHAR(50),
    total_copies INT NOT NULL,
    available_copies INT NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    download_link VARCHAR(200),
    download_limit INT
);

CREATE TABLE users (
    user_id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    email VARCHAR(50) UNIQUE,
    phone VARCHAR(15),
    is_active BOOLEAN DEFAULT TRUE
);

CREATE TABLE borrow_records (
    record_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id VARCHAR(20) NOT NULL,
    book_id VARCHAR(20) NOT NULL,
    borrow_date DATE NOT NULL,
    return_date DATE,
    is_returned BOOLEAN DEFAULT FALSE,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (book_id) REFERENCES books(book_id)
);
```

---

## Usage

1. Start your MySQL server.
2. Compile and run the program.
3. **Edit the connection string** in `main()` to match your MySQL username, password, and host:

```cpp
mysqlx::Session sess("mysqlx://root:YOUR_PASSWORD@localhost/library_db");
```

4. Use the menu to:
   - Add/remove/search/display books
   - Register/remove/display users
   - Issue/return books
   - View borrowed books per user
   - See library statistics

---

## Database Structure

| Table           | Description |
|-----------------|-------------|
| **books**       | Stores book info, copies, and digital data |
| **users**       | Stores user details and status |
| **borrow_records** | Tracks issued books, dates, and return status |

---

## Modules

- **Book Module:** Handles book-related CRUD
- **User Module:** Manages user records
- **Transaction Module:** Issues and returns books
- **Database Module:** Encapsulates MySQL operations
- **UI Module:** Displays menus and handles input

---

## Screenshots

<h3 align="center">Starting Panel</h3> <p align="center"> <img src="Demo_Images/startingpanel.png" alt="Starting Panel"> </p>
<h3 align="center">Add New Book</h3> <p align="center"> <img src="Demo_Images/AddNewBook.png" alt="Add New Book"> </p>
<h3 align="center">Display All Books</h3> <p align="center"> <img src="Demo_Images/DisplayAllBooks.png" alt="Display All Books"> </p>
<h3 align="center">Search Books</h3> <p align="center"> <img src="Demo_Images/SearchBooks.png" alt="Search Books"> </p>
<h3 align="center">Display All Users</h3> <p align="center"> <img src="Demo_Images/DisplayAllUsers.png" alt="Display All Users"> </p>
<h3 align="center">Library Statistics</h3> <p align="center"> <img src="Demo_Images/LibraryStats.png" alt="Library Statistics"> </p>

---

## Technical Specifications

- **Language:** C++11  
- **Database:** MySQL (`library_db`)  
- **Connector:** MySQL Connector/C++ (X DevAPI)  
- **Date Handling:** SQL `DATE` fields & `strftime` in C++  
- **Transactions:** Used for issuing/returning books  
- **Error Handling:** SQL exceptions and input validation  
- **Platform:** Cross-platform (Windows/Linux/Mac)  

---

## License

This project is licensed under the [MIT License](LICENSE).
