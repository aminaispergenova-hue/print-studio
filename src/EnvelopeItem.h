#pragma once
#include "OrderItem.h"

// envelope: no design, fixed price for one envelope
class EnvelopeItem : public OrderItem {
public:
    EnvelopeItem();

    Price calculatePrice() const override;
    std::string getName() const override;
    std::string getDetails() const override;
    std::string setOption(const std::string& key, const std::string& value) override;
    std::map<std::string, std::string> getOptions() const override;

private:
    std::string note;
};
