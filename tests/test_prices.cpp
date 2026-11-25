// checks of the price rules without the browser
// run:  bash build.sh test
#include <fstream>
#include <iostream>
#include <sstream>

#include "CardItem.h"
#include "MagazineItem.h"
#include "Order.h"
#include "Studio.h"
#include "Tariffs.h"
#include "lib/json.hpp"

using json = nlohmann::json;

int failed = 0;

void check(std::string name, int result, int expected) {
    if (result == expected) {
        std::cout << "  OK    " << name << ": " << result << "\n";
    } else {
        std::cout << "  FAIL  " << name << ": " << result << " (expected " << expected << ")\n";
        failed++;
    }
}

int main() {
    std::ifstream file("web/data/tariffs.json");
    std::stringstream text;
    text << file.rdbuf();
    std::string problem = loadTariffs(text.str());
    if (!problem.empty()) {
        std::cout << problem << "\n";
        return 1;
    }

    std::cout << "Control example\n";
    MagazineItem magazine;  // 12 pages, A4, glossy
    check("1 magazine", magazine.calculatePrice().total, 15600);
    magazine.setQuantity(2);
    check("2 same magazines (design once)", magazine.calculatePrice().total, 19200);

    CardItem cards;
    cards.setOption("size", "A5");
    cards.setQuantity(10);
    check("10 cards: printing before discount", cards.calculatePrice().printAll, 2000);
    check("10 cards: discount", cards.calculatePrice().discount, 200);
    check("10 cards: total", cards.calculatePrice().total, 2600);

    std::cout << "Polymorphism: one order with different items\n";
    Order order(1, 1);
    order.addItem(createItem("magazine"));
    std::unique_ptr<OrderItem> tenCards = createItem("card");
    tenCards->setOption("size", "A5");
    tenCards->setQuantity(10);
    order.addItem(std::move(tenCards));
    check("1 magazine + 10 cards", order.calculateTotal(), 18200);

    std::cout << "Encapsulation: quantity\n";
    check("setQuantity(0) is refused", cards.setQuantity(0), false);
    check("quantity is still 10", cards.getQuantity(), 10);
    check("setQuantity(-3) is refused", cards.setQuantity(-3), false);

    std::cout << "Discount borders\n";
    int quantities[] = {1, 4, 5, 9, 10, 29, 30, 100};
    int percents[] = {0, 0, 5, 5, 10, 10, 20, 20};
    for (int i = 0; i < 8; i++) {
        cards.setQuantity(quantities[i]);
        check("quantity " + std::to_string(quantities[i]) + ", %", cards.getDiscountPercent(), percents[i]);
    }

    std::cout << "Paper and sides\n";
    CardItem kraft;
    kraft.setOption("size", "A4");
    kraft.setOption("paper", "kraft");
    kraft.setOption("sides", "2");
    check("kraft A4 double-sided, 1 copy", kraft.calculatePrice().printOne, 700);
    CardItem matte;  // A6
    matte.setOption("paper", "matte");
    check("matte A6 single-sided, 1 copy", matte.calculatePrice().printOne, 50);

    std::cout << "Studio: customers, saving and loading\n";
    Studio studio;
    studio.addCustomer("Айгерим", "+7 701 000 00 01");
    json again = json::parse(studio.addCustomer("  айгерим ", "@aigerim"));
    check("same name -> same customer ID", again["id"].get<int>(), 1);
    check("only one customer in the list", json::parse(studio.findCustomers("")).size(), 1);

    studio.startDraft(1);
    studio.setDraftInfo("Birthday", "2026-12-20");
    studio.addDraftItem("magazine");
    studio.addDraftItem("card");
    studio.setDraftItemOption(1, "size", "A5");
    json bad = json::parse(studio.setDraftItemQuantity(1, "2.5"));
    check("quantity '2.5' is refused", bad["ok"].get<bool>(), false);
    studio.setDraftItemQuantity(1, "10");
    json saved = json::parse(studio.saveDraft("2026-11-25"));

    Studio reopened;
    reopened.loadData(studio.exportData());
    json order2 = json::parse(reopened.getOrder(saved["orderId"].get<int>()));
    check("order total after loading", order2["total"].get<int>(), 18200);

    std::cout << "Deleting an order\n";
    json deleted = json::parse(reopened.deleteOrder(saved["orderId"].get<int>()));
    check("delete returns the customer", deleted["customerId"].get<int>(), 1);
    check("order is gone", json::parse(reopened.getOrder(saved["orderId"].get<int>())).contains("error"), true);

    if (failed == 0) std::cout << "\nALL TESTS PASSED\n";
    else std::cout << "\n" << failed << " TESTS FAILED\n";
    return failed == 0 ? 0 : 1;
}
