#pragma once
#include <memory>
#include <string>
#include <vector>

#include "OrderItem.h"

// one order of a customer, it can have different products inside
class Order {
public:
    Order(int id, int customerId);

    int getId() const;
    int getCustomerId() const;

    void addItem(std::unique_ptr<OrderItem> item);
    bool removeItem(int index);
    int itemCount() const;
    OrderItem* getItem(int index) const;  // nullptr if there is no such item

    int calculateTotal() const;
    Stage overallStage() const;
    int readyCount() const;

    std::string title;    // project or occasion
    std::string dueDate;  // "2026-12-20"
    std::string createdDate;

private:
    int id;
    int customerId;
    std::vector<std::unique_ptr<OrderItem>> items;  // pointers to the base class
};
