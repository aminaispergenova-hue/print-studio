#include "EnvelopeItem.h"

#include "Helpers.h"
#include "Tariffs.h"

EnvelopeItem::EnvelopeItem() : OrderItem("envelope") {}

Price EnvelopeItem::calculatePrice() const {
    return makePrice(0, tariffs.envelopePrice);
}

std::string EnvelopeItem::getName() const {
    return "Envelopes";
}

std::string EnvelopeItem::getDetails() const {
    return note.empty() ? "envelopes" : note;
}

std::string EnvelopeItem::setOption(const std::string& key, const std::string& value) {
    if (key == "note") {
        note = trim(value);
        return "";
    }
    return "Unknown option: " + key;
}

std::map<std::string, std::string> EnvelopeItem::getOptions() const {
    return {{"note", note}};
}
