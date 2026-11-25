#include "InvitationItem.h"

#include "Helpers.h"
#include "Tariffs.h"

InvitationItem::InvitationItem() : OrderItem("invitation") {
    size = "A5";
}

Price InvitationItem::calculatePrice() const {
    int design = sides * tariffs.invitationSidePrice;
    int printOne = printCost(sides, 1);
    return makePrice(design, printOne);
}

std::string InvitationItem::getName() const {
    return "Invitations";
}

std::string InvitationItem::getDetails() const {
    std::string name = eventName.empty() ? "" : "\"" + eventName + "\", ";
    std::string sidesText = (sides == 2) ? "double-sided" : "single-sided";
    return name + size + ", " + sidesText + ", " + paper;
}

std::string InvitationItem::setOption(const std::string& key, const std::string& value) {
    if (key == "eventName") {
        eventName = trim(value);
        return "";
    }
    if (key == "sides") return setSides(value, sides);
    if (key == "size") return setSize(value, {"A6", "A5", "A4", "A3"});
    if (key == "paper") return setPaper(value);
    return "Unknown option: " + key;
}

std::map<std::string, std::string> InvitationItem::getOptions() const {
    return {{"eventName", eventName}, {"sides", std::to_string(sides)}, {"size", size}, {"paper", paper}};
}
