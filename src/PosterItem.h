#pragma once
#include "OrderItem.h"

// poster: fixed design price, printed on one side
class PosterItem : public OrderItem {
public:
    PosterItem();

    Price calculatePrice() const override;
    std::string getName() const override;
    std::string getDetails() const override;
    std::string setOption(const std::string& key, const std::string& value) override;
    std::map<std::string, std::string> getOptions() const override;

private:
    std::string theme;
};
