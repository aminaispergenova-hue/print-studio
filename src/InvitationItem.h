#pragma once
#include "OrderItem.h"

// invitation: design per side, one sheet per copy
class InvitationItem : public OrderItem {
public:
    InvitationItem();

    Price calculatePrice() const override;
    std::string getName() const override;
    std::string getDetails() const override;
    std::string setOption(const std::string& key, const std::string& value) override;
    std::map<std::string, std::string> getOptions() const override;

private:
    std::string eventName;
    int sides = 1;
};
