#include "Order.h"

Order::Order(int id, int customerId) {
    this->id = id;
    this->customerId = customerId;
}

int Order::getId() const { return id; }
int Order::getCustomerId() const { return customerId; }

void Order::addItem(std::unique_ptr<OrderItem> item) {
    items.push_back(std::move(item));
}

bool Order::removeItem(int index) {
    if (index < 0 || index >= itemCount()) return false;
    items.erase(items.begin() + index);
    return true;
}

int Order::itemCount() const {
    return items.size();
}

OrderItem* Order::getItem(int index) const {
    if (index < 0 || index >= itemCount()) return nullptr;
    return items[index].get();
}

// polymorphism: item is an OrderItem pointer, but calculatePrice() runs
// the version of the real object - MagazineItem, CardItem and so on
int Order::calculateTotal() const {
    int total = 0;
    for (const auto& item : items) {
        total += item->calculatePrice().total;
    }
    return total;
}

// the order is at the stage of its slowest item
Stage Order::overallStage() const {
    if (items.empty()) return Stage::Accepted;
    Stage slowest = Stage::Delivered;
    for (const auto& item : items) {
        if (item->getStage() < slowest) slowest = item->getStage();
    }
    return slowest;
}

int Order::readyCount() const {
    int count = 0;
    for (const auto& item : items) {
        if (item->getStage() >= Stage::Ready) count++;
    }
    return count;
}
