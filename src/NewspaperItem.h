#pragma once
#include "OrderItem.h"

// newspaper: priced like the magazine (design per page, both sides)
class NewspaperItem : public OrderItem {
public:
    NewspaperItem();

    Price calculatePrice() const override;
    std::string getName() const override;
    std::string getDetails() const override;
    std::string setOption(const std::string& key, const std::string& value) override;
    std::map<std::string, std::string> getOptions() const override;

private:
    std::string title;
    int pages = 4;
};
