#pragma once
#include "OrderItem.h"

// guest name cards, menu cards and thank-you cards
// one class, because they are priced the same way
// guest cards with different names use one layout, so design is paid once
class CardItem : public OrderItem {
public:
    CardItem();

    Price calculatePrice() const override;
    std::string getName() const override;
    std::string getDetails() const override;
    std::string setOption(const std::string& key, const std::string& value) override;
    std::map<std::string, std::string> getOptions() const override;

private:
    int countNames() const;

    std::string kind = "guest";  // guest, menu or thanks
    std::string guestNames;      // one name per line
    int sides = 1;
};
