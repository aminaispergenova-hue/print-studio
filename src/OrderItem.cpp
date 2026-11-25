#include "OrderItem.h"

#include "Tariffs.h"

std::string stageName(Stage stage) {
    switch (stage) {
        case Stage::Accepted:  return "Accepted";
        case Stage::InDesign:  return "In design";
        case Stage::Review:    return "Awaiting approval";
        case Stage::Printing:  return "Printing";
        case Stage::Ready:     return "Ready";
        case Stage::Delivered: return "Delivered";
    }
    return "";
}

OrderItem::OrderItem(std::string type) {
    this->type = type;
}

// the check is here and not only on the web page,
// so nobody can put 0 or a negative number into quantity
bool OrderItem::setQuantity(int newQuantity) {
    if (newQuantity <= 0) {
        return false;  // old value stays
    }
    quantity = newQuantity;
    return true;
}

int OrderItem::getQuantity() const {
    return quantity;
}

// 1-4 -> 0%, 5-9 -> 5%, 10-29 -> 10%, 30+ -> 20% (numbers are in tariffs.json)
int OrderItem::getDiscountPercent() const {
    int percent = 0;
    for (size_t i = 0; i < tariffs.discountFrom.size(); i++) {
        if (quantity >= tariffs.discountFrom[i]) {
            percent = tariffs.discountPercent[i];
        }
    }
    return percent;
}

// used by every product: design once + printing with discount
Price OrderItem::makePrice(int design, int printOne) const {
    Price p;
    p.design = design;
    p.printOne = printOne;
    p.quantity = quantity;
    p.printAll = printOne * quantity;
    p.discountPercent = getDiscountPercent();
    p.discount = (p.printAll * p.discountPercent + 50) / 100;  // rounded to whole tenge
    p.total = p.design + p.printAll - p.discount;
    return p;
}

// sides * price of the size + sheets * extra for the paper
int OrderItem::printCost(int sides, int sheets) const {
    return sides * tariffs.printPerSide[size] + sheets * tariffs.paperPerSheet[paper];
}

std::string OrderItem::setSize(const std::string& value, const std::vector<std::string>& allowed) {
    for (const std::string& s : allowed) {
        if (s == value) {
            size = value;
            return "";
        }
    }
    return "This size is not available for this product";
}

std::string OrderItem::setPaper(const std::string& value) {
    if (value != "glossy" && value != "matte" && value != "kraft") {
        return "Unknown paper type";
    }
    paper = value;
    return "";
}

std::string OrderItem::setSides(const std::string& value, int& sides) {
    if (value == "1") sides = 1;
    else if (value == "2") sides = 2;
    else return "Printing can only be single- or double-sided";
    return "";
}

std::string OrderItem::getType() const { return type; }
Stage OrderItem::getStage() const { return stage; }
void OrderItem::setStage(Stage newStage) { stage = newStage; }
