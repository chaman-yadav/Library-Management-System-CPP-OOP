-- Create the database if it doesn't already exist
CREATE DATABASE IF NOT EXISTS library_db;

-- Switch to using the new database
USE library_db;

-- Create the table for Books
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

-- Create the table for Users
CREATE TABLE users (
    user_id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    email VARCHAR(50) UNIQUE,
    phone VARCHAR(15),
    is_active BOOLEAN DEFAULT TRUE
);

-- Create the table for tracking borrowed books
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