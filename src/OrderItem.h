#pragma once
#include <map>
#include <string>
#include <vector>

// stages of work for one item (the order of values matters: bigger = further)
enum class Stage { Accepted, InDesign, Review, Printing, Ready, Delivered };

std::string stageName(Stage stage);

// how the price of one item was calculated (all in tenge)
struct Price {
    int design = 0;           // paid once for the layout
    int printOne = 0;         // printing of one copy
    int quantity = 0;
    int printAll = 0;         // printOne * quantity
    int discountPercent = 0;
    int discount = 0;         // only from printing
    int total = 0;            // design + printAll - discount
};

// base class for all products in an order (inheritance)
class OrderItem {
public:
    OrderItem(std::string type);
    virtual ~OrderItem() {}

    // encapsulation: quantity can be changed only through this method
    bool setQuantity(int newQuantity);
    int getQuantity() const;

    // common discount rule for all products
    int getDiscountPercent() const;

    // polymorphism: every product calculates its price in its own way
    virtual Price calculatePrice() const = 0;

    virtual std::string getName() const = 0;     // "Personal magazine"
    virtual std::string getDetails() const = 0;  // "12 pages, A4, glossy"

    // parameters of the product, e.g. "pages" = "12"
    // setOption returns "" if ok, otherwise the error text
    virtual std::string setOption(const std::string& key, const std::string& value) = 0;
    virtual std::map<std::string, std::string> getOptions() const = 0;

    std::string getType() const;
    Stage getStage() const;
    void setStage(Stage newStage);

protected:
    Price makePrice(int design, int printOne) const;
    int printCost(int sides, int sheets) const;
    std::string setSize(const std::string& value, const std::vector<std::string>& allowed);
    std::string setPaper(const std::string& value);
    std::string setSides(const std::string& value, int& sides);

    std::string size = "A5";
    std::string paper = "glossy";

private:
    std::string type;
    int quantity = 1;
    Stage stage = Stage::Accepted;
};
