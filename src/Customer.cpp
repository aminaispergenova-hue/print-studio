#include "Customer.h"

Customer::Customer(int id, std::string name, std::string contact) {
    this->id = id;
    this->name = name;
    this->contact = contact;
}

int Customer::getId() const { return id; }
std::string Customer::getName() const { return name; }
std::string Customer::getContact() const { return contact; }

void Customer::addOrder(Order order) {
    orders.push_back(std::move(order));
}

std::vector<Order>& Customer::getOrders() {
    return orders;
}

Order* Customer::findOrder(int orderId) {
    for (Order& order : orders) {
        if (order.getId() == orderId) return &order;
    }
    return nullptr;
}

bool Customer::removeOrder(int orderId) {
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].getId() == orderId) {
            orders.erase(orders.begin() + i);
            return true;
        }
    }
    return false;
}
