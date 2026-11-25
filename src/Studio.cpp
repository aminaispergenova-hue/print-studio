#include "Studio.h"

#include <algorithm>

#include "CardItem.h"
#include "EnvelopeItem.h"
#include "Helpers.h"
#include "InvitationItem.h"
#include "MagazineItem.h"
#include "NewspaperItem.h"
#include "PosterItem.h"
#include "Tariffs.h"
#include "lib/json.hpp"

using json = nlohmann::json;

std::unique_ptr<OrderItem> createItem(const std::string& type) {
    if (type == "magazine") return std::make_unique<MagazineItem>();
    if (type == "newspaper") return std::make_unique<NewspaperItem>();
    if (type == "poster") return std::make_unique<PosterItem>();
    if (type == "invitation") return std::make_unique<InvitationItem>();
    if (type == "card") return std::make_unique<CardItem>();
    if (type == "envelope") return std::make_unique<EnvelopeItem>();
    return nullptr;
}

// ---------- turning objects into JSON for the web page ----------

static std::string error(const std::string& text) {
    return json{{"error", text}}.dump();
}

static json priceJson(const Price& p) {
    return {{"design", p.design}, {"printOne", p.printOne}, {"quantity", p.quantity},
            {"printAll", p.printAll}, {"discountPercent", p.discountPercent},
            {"discount", p.discount}, {"total", p.total}};
}

static json itemJson(const OrderItem& item, int index) {
    return {{"index", index},
            {"type", item.getType()},
            {"name", item.getName()},
            {"details", item.getDetails()},
            {"options", item.getOptions()},
            {"quantity", item.getQuantity()},
            {"stage", (int)item.getStage()},
            {"stageName", stageName(item.getStage())},
            {"price", priceJson(item.calculatePrice())}};
}

static json orderShortJson(const Order& order) {
    return {{"id", order.getId()},
            {"customerId", order.getCustomerId()},
            {"title", order.title},
            {"dueDate", order.dueDate},
            {"createdDate", order.createdDate},
            {"stage", (int)order.overallStage()},
            {"stageName", stageName(order.overallStage())},
            {"itemCount", order.itemCount()},
            {"readyCount", order.readyCount()},
            {"total", order.calculateTotal()}};
}

static json orderJson(const Order& order) {
    json result = orderShortJson(order);
    result["items"] = json::array();
    for (int i = 0; i < order.itemCount(); i++) {
        result["items"].push_back(itemJson(*order.getItem(i), i));
    }
    return result;
}

static json customerShortJson(Customer& c) {
    return {{"id", c.getId()}, {"name", c.getName()}, {"contact", c.getContact()},
            {"orderCount", (int)c.getOrders().size()}};
}

// ---------- tariffs and saving ----------

std::string Studio::loadTariffs(const std::string& jsonText) {
    std::string problem = ::loadTariffs(jsonText);
    if (!problem.empty()) return error(problem);
    return json{{"ok", true}}.dump();
}

// reads the text that exportData() made before
std::string Studio::loadData(const std::string& jsonText) {
    try {
        json data = json::parse(jsonText);
        std::vector<Customer> loaded;

        for (auto& c : data["customers"]) {
            Customer customer(c["id"].get<int>(), c["name"].get<std::string>(), c.value("contact", ""));
            for (auto& o : c["orders"]) {
                Order order(o["id"].get<int>(), customer.getId());
                order.title = o.value("title", "");
                order.dueDate = o.value("dueDate", "");
                order.createdDate = o.value("createdDate", "");
                for (auto& i : o["items"]) {
                    std::unique_ptr<OrderItem> item = createItem(i["type"].get<std::string>());
                    if (!item) continue;
                    item->setQuantity(i["quantity"].get<int>());
                    item->setStage((Stage)i["stage"].get<int>());
                    for (auto& [key, value] : i["options"].items()) {
                        item->setOption(key, value.get<std::string>());
                    }
                    order.addItem(std::move(item));
                }
                customer.addOrder(std::move(order));
            }
            loaded.push_back(std::move(customer));
        }

        customers = std::move(loaded);
        nextCustomerId = data["nextCustomerId"].get<int>();
        nextOrderId = data["nextOrderId"].get<int>();
        draft.reset();
        return json{{"ok", true}}.dump();
    } catch (std::exception& e) {
        return error(std::string("Could not read saved data: ") + e.what());
    }
}

// everything as JSON text, the page keeps it in the browser (localStorage)
std::string Studio::exportData() {
    json list = json::array();
    for (Customer& c : customers) {
        json orders = json::array();
        for (Order& o : c.getOrders()) {
            json items = json::array();
            for (int i = 0; i < o.itemCount(); i++) {
                OrderItem* item = o.getItem(i);
                items.push_back({{"type", item->getType()},
                                 {"quantity", item->getQuantity()},
                                 {"stage", (int)item->getStage()},
                                 {"options", item->getOptions()}});
            }
            orders.push_back({{"id", o.getId()}, {"title", o.title}, {"dueDate", o.dueDate},
                              {"createdDate", o.createdDate}, {"items", items}});
        }
        list.push_back({{"id", c.getId()}, {"name", c.getName()}, {"contact", c.getContact()},
                        {"orders", orders}});
    }
    json data = {{"nextCustomerId", nextCustomerId}, {"nextOrderId", nextOrderId}, {"customers", list}};
    return data.dump();
}

// ---------- customers ----------

std::string Studio::findCustomers(const std::string& text) {
    json result = json::array();
    for (Customer& c : customers) {
        if (contains(c.getName(), text) || contains(c.getContact(), text)) {
            result.push_back(customerShortJson(c));
        }
    }
    return result.dump();
}

// one name = one customer: if the name is already there, the old customer is returned
std::string Studio::addCustomer(const std::string& name, const std::string& contact) {
    std::string cleanName = trim(name);
    if (cleanName.empty()) return error("Enter the customer's name");

    for (Customer& c : customers) {
        if (toLower(c.getName()) == toLower(cleanName)) {
            return json{{"ok", true}, {"id", c.getId()}, {"existing", true}}.dump();
        }
    }

    customers.push_back(Customer(nextCustomerId, cleanName, trim(contact)));
    nextCustomerId++;
    return json{{"ok", true}, {"id", customers.back().getId()}, {"existing", false}}.dump();
}

std::string Studio::getCustomer(int customerId) {
    Customer* c = findCustomer(customerId);
    if (!c) return error("Customer not found");

    json result = customerShortJson(*c);
    result["orders"] = json::array();
    for (Order& o : c->getOrders()) {
        result["orders"].push_back(orderShortJson(o));
    }
    return result.dump();
}

// ---------- new order (draft) ----------

std::string Studio::startDraft(int customerId) {
    if (!findCustomer(customerId)) return error("Customer not found");
    draft = std::make_unique<Order>(nextOrderId, customerId);
    return getDraft();
}

std::string Studio::getDraft() {
    if (!draft) return error("No order is open");
    json result = orderJson(*draft);
    Customer* c = findCustomer(draft->getCustomerId());
    result["customerName"] = c ? c->getName() : "";
    result["customerContact"] = c ? c->getContact() : "";
    return result.dump();
}

std::string Studio::setDraftInfo(const std::string& title, const std::string& dueDate) {
    if (!draft) return error("No order is open");
    draft->title = trim(title);
    draft->dueDate = trim(dueDate);
    return json{{"ok", true}}.dump();
}

std::string Studio::addDraftItem(const std::string& type) {
    if (!draft) return error("No order is open");
    std::unique_ptr<OrderItem> item = createItem(type);
    if (!item) return error("Unknown product type");
    draft->addItem(std::move(item));
    return getDraft();
}

std::string Studio::removeDraftItem(int index) {
    if (!draft) return error("No order is open");
    draft->removeItem(index);
    return getDraft();
}

std::string Studio::setDraftItemOption(int index, const std::string& key, const std::string& value) {
    OrderItem* item = draft ? draft->getItem(index) : nullptr;
    if (!item) return error("Item not found");
    return draftResult(index, item->setOption(key, value));
}

// the page sends the text from the field, for example "10", "0" or "2.5"
std::string Studio::setDraftItemQuantity(int index, const std::string& text) {
    OrderItem* item = draft ? draft->getItem(index) : nullptr;
    if (!item) return error("Item not found");

    int number = 0;
    bool ok = toInt(text, number) && item->setQuantity(number);

    std::string problem;
    if (!ok) {
        problem = "Quantity must be a whole number greater than zero. The last correct value is kept: " +
                  std::to_string(item->getQuantity()) + ".";
    }
    return draftResult(index, problem);
}

std::string Studio::saveDraft(const std::string& today) {
    if (!draft) return error("No order is open");
    if (draft->itemCount() == 0) return error("Add at least one item to the order");
    if (draft->title.empty()) return error("Enter the project title or occasion");
    if (draft->dueDate.size() != 10) return error("Choose the due date");

    Customer* c = findCustomer(draft->getCustomerId());
    if (!c) return error("Customer not found");

    int orderId = draft->getId();
    draft->createdDate = today;
    c->addOrder(std::move(*draft));
    draft.reset();
    nextOrderId++;
    return json{{"ok", true}, {"orderId", orderId}}.dump();
}

// ---------- saved orders ----------

std::string Studio::getOrder(int orderId) {
    Customer* owner = nullptr;
    Order* order = findOrder(orderId, owner);
    if (!order) return error("Order not found");

    json result = orderJson(*order);
    result["customerName"] = owner->getName();
    result["customerContact"] = owner->getContact();
    return result.dump();
}

std::string Studio::setItemStage(int orderId, int index, int stage) {
    Customer* owner = nullptr;
    Order* order = findOrder(orderId, owner);
    OrderItem* item = order ? order->getItem(index) : nullptr;
    if (!item || stage < 0 || stage > (int)Stage::Delivered) return error("Could not change the status");

    item->setStage((Stage)stage);
    return getOrder(orderId);
}

std::string Studio::deleteOrder(int orderId) {
    Customer* owner = nullptr;
    Order* order = findOrder(orderId, owner);
    if (!order) return error("Order not found");

    owner->removeOrder(orderId);
    return json{{"ok", true}, {"customerId", owner->getId()}}.dump();
}

// stageFilter: "active" = everything except Delivered, "all", or a stage number "0".."5"
// the nearest due date goes first
std::string Studio::listOrders(const std::string& stageFilter, const std::string& customerText) {
    json result = json::array();
    for (Customer& c : customers) {
        if (!contains(c.getName(), customerText)) continue;

        for (Order& o : c.getOrders()) {
            int stage = (int)o.overallStage();
            bool show = stageFilter == "all" ||
                        (stageFilter == "active" && o.overallStage() != Stage::Delivered) ||
                        stageFilter == std::to_string(stage);
            if (!show) continue;

            json row = orderShortJson(o);
            row["customerName"] = c.getName();
            row["customerContact"] = c.getContact();
            result.push_back(row);
        }
    }

    std::sort(result.begin(), result.end(), [](const json& a, const json& b) {
        return a["dueDate"].get<std::string>() < b["dueDate"].get<std::string>();
    });
    return result.dump();
}

std::string Studio::stageNames() {
    json names = json::array();
    for (int s = 0; s <= (int)Stage::Delivered; s++) {
        names.push_back(stageName((Stage)s));
    }
    return names.dump();
}

// ---------- private helpers ----------

Customer* Studio::findCustomer(int customerId) {
    for (Customer& c : customers) {
        if (c.getId() == customerId) return &c;
    }
    return nullptr;
}

Order* Studio::findOrder(int orderId, Customer*& owner) {
    for (Customer& c : customers) {
        Order* o = c.findOrder(orderId);
        if (o) {
            owner = &c;
            return o;
        }
    }
    return nullptr;
}

// answer after changing one item of the draft: error (if any), the item and the new total
std::string Studio::draftResult(int index, const std::string& problem) {
    json result = {{"ok", problem.empty()},
                   {"error", problem},
                   {"item", itemJson(*draft->getItem(index), index)},
                   {"total", draft->calculateTotal()}};
    return result.dump();
}
