#include "NewspaperItem.h"

#include "Helpers.h"
#include "Tariffs.h"

NewspaperItem::NewspaperItem() : OrderItem("newspaper") {
    size = "A3";
}

Price NewspaperItem::calculatePrice() const {
    int design = pages * tariffs.newspaperPagePrice;
    int sheets = (pages + 1) / 2;
    int printOne = printCost(pages, sheets);
    return makePrice(design, printOne);
}

std::string NewspaperItem::getName() const {
    return "Newspaper";
}

std::string NewspaperItem::getDetails() const {
    std::string name = title.empty() ? "untitled" : "\"" + title + "\"";
    return name + ", " + std::to_string(pages) + " pages, " + size + ", " + paper;
}

std::string NewspaperItem::setOption(const std::string& key, const std::string& value) {
    if (key == "title") {
        title = trim(value);
        return "";
    }
    if (key == "pages") {
        int n = 0;
        if (!toInt(value, n) || n < 1 || n > 64) return "Pages must be a whole number from 1 to 64";
        pages = n;
        return "";
    }
    if (key == "size") return setSize(value, {"A5", "A4", "A3"});
    if (key == "paper") return setPaper(value);
    return "Unknown option: " + key;
}

std::map<std::string, std::string> NewspaperItem::getOptions() const {
    return {{"title", title}, {"pages", std::to_string(pages)}, {"size", size}, {"paper", paper}};
}
