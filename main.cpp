#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <list>
#include <ranges>
#include <mutex>
#include <thread>

std::mutex coutMutex;

class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() const = 0;
    virtual void display() const = 0;
    [[nodiscard]] virtual Animal* clone() const = 0;
    [[nodiscard]] virtual std::string getType() const = 0;
    virtual void info() const = 0;
};

class Dog : public Animal {
private:
    std::string name;
public:
    explicit Dog(std::string name) : name(std::move(name)) {}

    void speak() const override {
        std::cout << name << " says Woof!" << std::endl;
    }

    void display() const override {
        std::cout << "Dog: " << name << std::endl;
    }

    [[nodiscard]] Animal* clone() const override {
        return new Dog(*this);
    }

    [[nodiscard]] std::string getType() const override {
        return "Dog";
    }

    void info() const override {
        std::cout << "Dog Info: " << name << std::endl;
    }
};

class Cat : public Animal {
private:
    std::string name;
public:
    explicit Cat(std::string name) : name(std::move(name)) {}

    void speak() const override {
        std::cout << name << " says Meow!" << std::endl;
    }

    void display() const override {
        std::cout << "Cat: " << name << std::endl;
    }

    [[nodiscard]] Animal* clone() const override {
        return new Cat(*this);
    }

    [[nodiscard]] std::string getType() const override {
        return "Cat";
    }

    void info() const override {
        std::cout << "Cat Info: " << name << std::endl;
    }
};

class AbstractAnimalFactory {
public:
    virtual ~AbstractAnimalFactory() = default;
    virtual std::shared_ptr<Animal> createAnimal(const std::string& name) = 0;
};

class DogFactory : public AbstractAnimalFactory {
public:
    std::shared_ptr<Animal> createAnimal(const std::string& name) override {
        return std::make_shared<Dog>(name);
    }
};

class CatFactory : public AbstractAnimalFactory {
public:
    std::shared_ptr<Animal> createAnimal(const std::string& name) override {
        return std::make_shared<Cat>(name);
    }
};

class AnimalContainer {
private:
    std::vector<std::shared_ptr<Animal>> container;
    static int instanceCount;
public:
    AnimalContainer() {
        ++instanceCount;
    }

    void addAnimal(const std::shared_ptr<Animal>& animal) {
        container.push_back(animal);
    }

    void displayAll() const {
        for (const auto& animal : container) {
            animal->display();
        }
    }

    void removeAnimal(const std::string& name) {
        std::erase_if(container, [&name](const std::shared_ptr<Animal>& animal) {
            return animal->getType() == name;
        });
    }

    void displayAnimalInfo(const std::string& name) const {
        for (const auto& animal : container) {
            if (animal->getType() == name) {
                animal->info();
            }
        }
    }

    void sortAnimals() {
        std::ranges::sort(container, [](const std::shared_ptr<Animal>& a, const std::shared_ptr<Animal>& b) {
            return a->getType() < b->getType();
        });
    }

    static void showInstanceCount() {
        std::cout << "Total AnimalContainer instances: " << instanceCount << std::endl;
    }

    ~AnimalContainer() {
        --instanceCount;
    }
};

int AnimalContainer::instanceCount = 0;

class AnimalObserver {
public:
    virtual ~AnimalObserver() = default;
    virtual void update(const std::shared_ptr<Animal>& animal) = 0;
};

class AnimalNotifier {
private:
    std::list<std::shared_ptr<AnimalObserver>> observers;
public:
    void addObserver(const std::shared_ptr<AnimalObserver>& observer) {
        observers.push_back(observer);
    }

    void notify(const std::shared_ptr<Animal>& animal) {
        for (const auto& observer : observers) {
            observer->update(animal);
        }
    }
};

class AnimalDetailsObserver : public AnimalObserver {
public:
    void update(const std::shared_ptr<Animal>& animal) override {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Observer: ";
        animal->info();
    }
};

class AnimalFactory {
public:
    static std::shared_ptr<Animal> createAnimal(const std::string& type, const std::string& name) {
        if (type == "Dog") {
            return std::make_shared<Dog>(name);
        } else if (type == "Cat") {
            return std::make_shared<Cat>(name);
        } else {
            throw std::invalid_argument("Unknown animal type");
        }
    }
};

void menu() {
    std::cout << "1. Add Animal\n";
    std::cout << "2. Display All Animals\n";
    std::cout << "3. Remove Animal\n";
    std::cout << "4. Display Animal Info\n";
    std::cout << "5. Sort Animals\n";
    std::cout << "6. Show AnimalContainer Instance Count\n";
    std::cout << "7. Exit\n";
}

void threadTest(const AnimalContainer& container) {
    std::cout << "Started a thread for displaying all animals.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    container.displayAll();
}

int main() {
    AnimalContainer container;
    AnimalNotifier notifier;
    AnimalDetailsObserver observer;

    notifier.addObserver(std::make_shared<AnimalDetailsObserver>(observer));

    bool running = true;
    while (running) {
        menu();
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::string type, name;
            std::cout << "Enter animal type (Dog/Cat): ";
            std::cin >> type;
            std::cout << "Enter animal name: ";
            std::cin >> name;

            try {
                auto animal = AnimalFactory::createAnimal(type, name);
                container.addAnimal(animal);
                notifier.notify(animal);
            } catch (const std::invalid_argument& e) {
                std::cout << e.what() << std::endl;
            }
            break;
        }
        case 2:
            container.displayAll();
            break;
        case 3: {
            std::string name;
            std::cout << "Enter animal name to remove: ";
            std::cin >> name;
            container.removeAnimal(name);
            break;
        }
        case 4: {
            std::string name;
            std::cout << "Enter animal type to get info: ";
            std::cin >> name;
            container.displayAnimalInfo(name);
            break;
        }
        case 5:
            container.sortAnimals();
            std::cout << "Animals sorted.\n";
            break;
        case 6:
            AnimalContainer::showInstanceCount();
            break;
        case 7:
            running = false;
            break;
        default:
            std::cout << "Invalid option. Please try again.\n";
        }

        std::thread t(threadTest, std::ref(container));
        t.join();
    }

    return 0; // No need for explicit return; C++ will return 0 implicitly.
}
