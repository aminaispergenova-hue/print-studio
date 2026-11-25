#include "PosterItem.h"

#include "Helpers.h"
#include "Tariffs.h"

PosterItem::PosterItem() : OrderItem("poster") {
    size = "A3";
}

Price PosterItem::calculatePrice() const {
    int design = tariffs.posterDesignPrice;
    int printOne = printCost(1, 1);
    return makePrice(design, printOne);
}

std::string PosterItem::getName() const {
    return "Poster";
}

std::string PosterItem::getDetails() const {
    std::string name = theme.empty() ? "" : "\"" + theme + "\", ";
    return name + size + ", " + paper;
}

std::string PosterItem::setOption(const std::string& key, const std::string& value) {
    if (key == "theme") {
        theme = trim(value);
        return "";
    }
    if (key == "size") return setSize(value, {"A4", "A3", "A2"});
    if (key == "paper") return setPaper(value);
    return "Unknown option: " + key;
}

std::map<std::string, std::string> PosterItem::getOptions() const {
    return {{"theme", theme}, {"size", size}, {"paper", paper}};
}
