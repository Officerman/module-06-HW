#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <fstream>
#include <thread>
#include <vector>
#include <memory>

// ----------------------------- Паттерн "Одиночка" -----------------------------

class ConfigurationManager {
private:
    static ConfigurationManager* instance;
    static std::mutex mtx;
    
    std::map<std::string, std::string> settings;

    ConfigurationManager() {}

public:
    static ConfigurationManager* GetInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr) {
                instance = new ConfigurationManager();
            }
        }
        return instance;
    }

    void LoadSettingsFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Не удалось открыть файл настроек.");
        }

        std::string key, value;
        while (file >> key >> value) {
            settings[key] = value;
        }

        file.close();
    }

    std::string GetSetting(const std::string& key) {
        if (settings.find(key) != settings.end()) {
            return settings[key];
        }
        throw std::runtime_error("Настройка не найдена.");
    }

    void SetSetting(const std::string& key, const std::string& value) {
        settings[key] = value;
    }

    void SaveSettingsToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Не удалось открыть файл для записи.");
        }

        for (const auto& pair : settings) {
            file << pair.first << " " << pair.second << "\n";
        }

        file.close();
    }
};

ConfigurationManager* ConfigurationManager::instance = nullptr;
std::mutex ConfigurationManager::mtx;

// ----------------------------- Паттерн "Строитель" -----------------------------

class Report {
private:
    std::string header;
    std::string content;
    std::string footer;

public:
    void SetHeader(const std::string& h) { header = h; }
    void SetContent(const std::string& c) { content = c; }
    void SetFooter(const std::string& f) { footer = f; }

    void Display() const {
        std::cout << "Header: " << header << std::endl;
        std::cout << "Content: " << content << std::endl;
        std::cout << "Footer: " << footer << std::endl;
    }
};

class IReportBuilder {
public:
    virtual ~IReportBuilder() = default;
    virtual void SetHeader(const std::string& header) = 0;
    virtual void SetContent(const std::string& content) = 0;
    virtual void SetFooter(const std::string& footer) = 0;
    virtual Report GetReport() = 0;
};

class TextReportBuilder : public IReportBuilder {
private:
    Report report;


public:
    void SetHeader(const std::string& header) override {
        report.SetHeader("Text Header: " + header);
    }

    void SetContent(const std::string& content) override {
        report.SetContent("Text Content: " + content);
    }

    void SetFooter(const std::string& footer) override {
        report.SetFooter("Text Footer: " + footer);
    }

    Report GetReport() override {
        return report;
    }
};

class HtmlReportBuilder : public IReportBuilder {
private:
    Report report;
    

public:
    void SetHeader(const std::string& header) override {
        report.SetHeader("<h1>" + header + "</h1>");
    }

    void SetContent(const std::string& content) override {
        report.SetContent("<p>" + content + "</p>");
    }

    void SetFooter(const std::string& footer) override {
        report.SetFooter("<footer>" + footer + "</footer>");
    }

    Report GetReport() override {
        return report;
    }
};

class ReportDirector {
public:
    Report ConstructReport(IReportBuilder& builder) {
        builder.SetHeader("Report Header");
        builder.SetContent("This is the report content.");
        builder.SetFooter("Report Footer");
        return builder.GetReport();
    }
};

// ----------------------------- Паттерн "Прототип" -----------------------------

class Product {
public:
    std::string name;
    double price;

    Product(const std::string& n, double p) : name(n), price(p) {}

    std::shared_ptr<Product> Clone() const {
        return std::make_shared<Product>(*this);
    }

    void Display() const {
        std::cout << "Product: " << name << ", Price: " << price << std::endl;
    }
};

class Order {
private:
    std::vector<std::shared_ptr<Product>> products;
    double shippingCost;
    double discount;
    std::string paymentMethod;

public:
    Order(double shipping, double disc, const std::string& payment) 
        : shippingCost(shipping), discount(disc), paymentMethod(payment) {}

    void AddProduct(const std::shared_ptr<Product>& product) {
        products.push_back(product->Clone());
    }

    std::shared_ptr<Order> Clone() const {
        std::shared_ptr<Order> newOrder = std::make_shared<Order>(shippingCost, discount, paymentMethod);
        for (const auto& product : products) {
            newOrder->AddProduct(product);
        }
        return newOrder;
    }

    void Display() const {
        std::cout << "Order details:" << std::endl;
        for (const auto& product : products) {
            product->Display();
        }
        std::cout << "Shipping Cost: " << shippingCost << ", Discount: " << discount << ", Payment: " << paymentMethod << std::endl;
    }
};

// ----------------------------- Осн программа -----------------------------

void TestSingleton() {
    ConfigurationManager* config = ConfigurationManager::GetInstance();
    std::cout << "Настройка 'username': " << config->GetSetting("username") << std::endl;
}

int main() {
    // Тестирование паттерна "Одиночка"
    ConfigurationManager* config = ConfigurationManager::GetInstance();
    config->SetSetting("username", "user1");

    std::thread t1(TestSingleton);
    std::thread t2(TestSingleton);
    t1.join();
    t2.join();

    // Тестирование паттерна "Строитель"
    TextReportBuilder textBuilder;
    HtmlReportBuilder htmlBuilder;
    ReportDirector director;

    Report textReport = director.ConstructReport(textBuilder);
    Report htmlReport = director.ConstructReport(htmlBuilder);

    std::cout << "\nText Report:" << std::endl;
    textReport.Display();

    std::cout << "\nHTML Report:" << std::endl;
    htmlReport.Display();

    // Тестирование паттерна "Прототип"
    auto product1 = std::make_shared<Product>("Laptop", 1200);
    auto product2 = std::make_shared<Product>("Smartphone", 800);

    Order originalOrder(50, 10, "Credit Card");
    originalOrder.AddProduct(product1);
    originalOrder.AddProduct(product2);

    std::cout << "\nOriginal Order:" << std::endl;
    originalOrder.Display();

    std::shared_ptr<Order> clonedOrder = originalOrder.Clone();
    clonedOrder->Display();

    return 0;
}
