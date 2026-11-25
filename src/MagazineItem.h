#pragma once
#include "OrderItem.h"

// personal magazine: design per page, pages are printed on both sides
class MagazineItem : public OrderItem {
public:
    MagazineItem();

    Price calculatePrice() const override;
    std::string getName() const override;
    std::string getDetails() const override;
    std::string setOption(const std::string& key, const std::string& value) override;
    std::map<std::string, std::string> getOptions() const override;

private:
    std::string title;
    int pages = 12;
};
