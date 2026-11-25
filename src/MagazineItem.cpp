#include "MagazineItem.h"

#include "Helpers.h"
#include "Tariffs.h"

MagazineItem::MagazineItem() : OrderItem("magazine") {
    size = "A4";
}

// 12 pages -> design 12 * 1000, printing 12 sides on 6 sheets
Price MagazineItem::calculatePrice() const {
    int design = pages * tariffs.magazinePagePrice;
    int sheets = (pages + 1) / 2;
    int printOne = printCost(pages, sheets);
    return makePrice(design, printOne);
}

std::string MagazineItem::getName() const {
    return "Personal magazine";
}

std::string MagazineItem::getDetails() const {
    std::string name = title.empty() ? "untitled" : "\"" + title + "\"";
    return name + ", " + std::to_string(pages) + " pages, " + size + ", " + paper;
}

std::string MagazineItem::setOption(const std::string& key, const std::string& value) {
    if (key == "title") {
        title = trim(value);
        return "";
    }
    if (key == "pages") {
        int n = 0;
        if (!toInt(value, n) || n < 1 || n > 200) return "Pages must be a whole number from 1 to 200";
        pages = n;
        return "";
    }
    if (key == "size") return setSize(value, {"A5", "A4", "A3"});
    if (key == "paper") return setPaper(value);
    return "Unknown option: " + key;
}

std::map<std::string, std::string> MagazineItem::getOptions() const {
    return {{"title", title}, {"pages", std::to_string(pages)}, {"size", size}, {"paper", paper}};
}
