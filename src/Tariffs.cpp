#include "Tariffs.h"

#include "lib/json.hpp"

using json = nlohmann::json;

Tariffs tariffs;

std::string loadTariffs(const std::string& jsonText) {
    try {
        json j = json::parse(jsonText);
        Tariffs t;

        t.magazinePagePrice = j["design"]["magazinePerPage"].get<int>();
        t.newspaperPagePrice = j["design"]["newspaperPerPage"].get<int>();
        t.cardSidePrice = j["design"]["cardPerSide"].get<int>();
        t.invitationSidePrice = j["design"]["invitationPerSide"].get<int>();
        t.posterDesignPrice = j["design"]["poster"].get<int>();
        t.envelopePrice = j["envelope"].get<int>();

        for (auto& [size, price] : j["printPerSide"].items()) t.printPerSide[size] = price.get<int>();
        for (auto& [paper, price] : j["paperPerSheet"].items()) t.paperPerSheet[paper] = price.get<int>();

        // the rows must go from the smallest quantity to the biggest
        for (auto& row : j["discounts"]) {
            t.discountFrom.push_back(row["fromQuantity"].get<int>());
            t.discountPercent.push_back(row["percent"].get<int>());
        }

        tariffs = t;
        return "";
    } catch (std::exception& e) {
        return std::string("Error in tariffs.json: ") + e.what();
    }
}
