#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Customer.h"

// makes an item by its type name: "magazine" -> MagazineItem
std::unique_ptr<OrderItem> createItem(const std::string& type);

// the whole studio: all customers with their orders
// and the new order that is being filled in now (draft)
// the web page calls only these methods, each one returns JSON text
class Studio {
public:
    // tariffs and saving
    std::string loadTariffs(const std::string& jsonText);
    std::string loadData(const std::string& jsonText);
    std::string exportData();

    // customers
    std::string findCustomers(const std::string& text);
    std::string addCustomer(const std::string& name, const std::string& contact);
    std::string getCustomer(int customerId);

    // new order (draft)
    std::string startDraft(int customerId);
    std::string getDraft();
    std::string setDraftInfo(const std::string& title, const std::string& dueDate);
    std::string addDraftItem(const std::string& type);
    std::string removeDraftItem(int index);
    std::string setDraftItemOption(int index, const std::string& key, const std::string& value);
    std::string setDraftItemQuantity(int index, const std::string& text);
    std::string saveDraft(const std::string& today);

    // saved orders
    std::string getOrder(int orderId);
    std::string setItemStage(int orderId, int index, int stage);
    std::string deleteOrder(int orderId);
    std::string listOrders(const std::string& stageFilter, const std::string& customerText);
    std::string stageNames();

private:
    Customer* findCustomer(int customerId);
    Order* findOrder(int orderId, Customer*& owner);
    std::string draftResult(int index, const std::string& error);

    std::vector<Customer> customers;
    std::unique_ptr<Order> draft;
    int nextCustomerId = 1;
    int nextOrderId = 1001;
};
