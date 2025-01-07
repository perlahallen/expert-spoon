#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <exception>
#include <string>

class LibraryException : public std::exception {
public:
    explicit LibraryException(std::string message) : msg_(std::move(message)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class LibraryItem {
public:
    virtual ~LibraryItem() = default;
    virtual void display() const = 0;
    [[nodiscard]] virtual std::string getType() const = 0;
};

class Book : public LibraryItem {
public:
    explicit Book(std::string title, std::string author)
        : title_(std::move(title)), author_(std::move(author)) {}

    Book(const Book& other) : title_(other.title_), author_(other.author_) {}

    Book& operator=(const Book& other) {
        if (this != &other) {
            title_ = other.title_;
            author_ = other.author_;
        }
        return *this;
    }

    void display() const override {
        std::cout << "Book: " << title_ << " by " << author_ << std::endl;
    }

    [[nodiscard]] std::string getType() const override {
        return "Book";
    }

private:
    std::string title_;
    std::string author_;
};

class Magazine : public LibraryItem {
public:
    explicit Magazine(std::string title, int issue)
        : title_(std::move(title)), issue_(issue) {}

    Magazine(const Magazine& other) : title_(other.title_), issue_(other.issue_) {}

    Magazine& operator=(const Magazine& other) {
        if (this != &other) {
            title_ = other.title_;
            issue_ = other.issue_;
        }
        return *this;
    }

    void display() const override {
        std::cout << "Magazine: " << title_ << " Issue: " << issue_ << std::endl;
    }

    [[nodiscard]] std::string getType() const override {
        return "Magazine";
    }

private:
    std::string title_;
    int issue_;
};

class Member {
public:
    explicit Member(std::string name) : name_(std::move(name)) {}

    Member(const Member& other) = default;
    Member& operator=(const Member& other) = default;

    void display() const {
        std::cout << "Member: " << name_ << std::endl;
    }

private:
    std::string name_;
};

class Library {
public:
    static Library& getInstance() {
        static Library instance;
        return instance;
    }

    void addItem(std::unique_ptr<LibraryItem> item) {
        items_.push_back(std::move(item));
    }

    void addMember(Member member) {
        members_.push_back(std::move(member));
    }

    void displayItems() const {
        for (const auto& item : items_) {
            item->display();
        }
    }

    void displayMembers() const {
        for (const auto& member : members_) {
            member.display();
        }
    }

private:
    Library() = default;
    std::vector<std::unique_ptr<LibraryItem>> items_;
    std::list<Member> members_;
};

template <typename T>
class Container {
public:
    void add(T item) {
        items_.push_back(std::move(item));
    }

    void sort() {
        std::sort(items_.begin(), items_.end());
    }

    void display() const {
        for (const auto& item : items_) {
            std::cout << item << std::endl;
        }
    }

private:
    std::vector<T> items_;
};

std::unique_ptr<LibraryItem> createLibraryItem(const std::string& type, std::string title, std::string authorOrIssue) {
    if (type == "book") {
        return std::make_unique<Book>(std::move(title), std::move(authorOrIssue));
    } else if (type == "magazine") {
        return std::make_unique<Magazine>(std::move(title), std::stoi(authorOrIssue));
    } else {
        throw LibraryException("Unknown library item type");
    }
}

int main() {
    Library& library = Library::getInstance();

    while (true) {
        std::cout << "1. Add Book\n";
        std::cout << "2. Add Magazine\n";
        std::cout << "3. Add Member\n";
        std::cout << "4. Display Items\n";
        std::cout << "5. Display Members\n";
        std::cout << "6. Exit\n";
        std::cout << "Choose an option: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: {
                std::string title, author;
                std::cout << "Enter book title: ";
                std::getline(std::cin, title);
                std::cout << "Enter book author: ";
                std::getline(std::cin, author);
                library.addItem(createLibraryItem("book", std::move(title), std::move(author)));
                break;
            }
            case 2: {
                std::string title;
                int issue;
                std::cout << "Enter magazine title: ";
                std::getline(std::cin, title);
                std::cout << "Enter magazine issue number: ";
                std::cin >> issue;
                std::cin.ignore();
                library.addItem(createLibraryItem("magazine", std::move(title), std::to_string(issue)));
                break;
            }
            case 3: {
                std::string memberName;
                std::cout << "Enter member name: ";
                std::getline(std::cin, memberName);
                library.addMember(Member(std::move(memberName)));
                break;
            }
            case 4:
                library.displayItems();
                break;
            case 5:
                library.displayMembers();
                break;
            case 6:
                return 0;
            default:
                std::cout << "Invalid option, please try again.\n";
                break;
        }
    }
}
