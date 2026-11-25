#include "CardItem.h"

#include <sstream>

#include "Helpers.h"
#include "Tariffs.h"

CardItem::CardItem() : OrderItem("card") {
    size = "A6";
}

// 10 cards A5: design 800 + printing 10 * 200 = 2000, discount 10% = 200 -> 2600
Price CardItem::calculatePrice() const {
    int design = sides * tariffs.cardSidePrice;
    int printOne = printCost(sides, 1);
    return makePrice(design, printOne);
}

std::string CardItem::getName() const {
    if (kind == "menu") return "Menu cards";
    if (kind == "thanks") return "Thank-you cards";
    return "Guest name cards";
}

int CardItem::countNames() const {
    std::istringstream lines(guestNames);
    std::string line;
    int count = 0;
    while (std::getline(lines, line)) {
        if (!trim(line).empty()) count++;
    }
    return count;
}

std::string CardItem::getDetails() const {
    std::string sidesText = (sides == 2) ? "double-sided" : "single-sided";
    std::string text = size + ", " + sidesText + ", " + paper;
    if (kind == "guest") {
        int names = countNames();
        if (names == 0) text += ", guest list not received yet";
        else text += ", names in list: " + std::to_string(names);
    }
    return text;
}

std::string CardItem::setOption(const std::string& key, const std::string& value) {
    if (key == "kind") {
        if (value != "guest" && value != "menu" && value != "thanks") return "Unknown card type";
        kind = value;
        return "";
    }
    if (key == "guestNames") {
        guestNames = value;
        return "";
    }
    if (key == "sides") return setSides(value, sides);
    if (key == "size") return setSize(value, {"A6", "A5", "A4", "A3"});
    if (key == "paper") return setPaper(value);
    return "Unknown option: " + key;
}

std::map<std::string, std::string> CardItem::getOptions() const {
    return {{"kind", kind}, {"guestNames", guestNames}, {"sides", std::to_string(sides)},
            {"size", size}, {"paper", paper}};
}
