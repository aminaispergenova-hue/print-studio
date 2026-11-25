#pragma once
#include <string>
#include <vector>

#include "Order.h"

// a customer of the studio, can have many orders
class Customer {
public:
    Customer(int id, std::string name, std::string contact);

    int getId() const;
    std::string getName() const;
    std::string getContact() const;

    void addOrder(Order order);
    std::vector<Order>& getOrders();
    Order* findOrder(int orderId);
    bool removeOrder(int orderId);

private:
    int id;
    std::string name;
    std::string contact;  // phone or Instagram
    std::vector<Order> orders;
};
