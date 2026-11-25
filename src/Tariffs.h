#pragma once
#include <map>
#include <string>
#include <vector>

// all prices of the studio (in tenge)
// they are read from web/data/tariffs.json, so a price can be changed without changing the code
struct Tariffs {
    int magazinePagePrice = 0;      // design, per page
    int newspaperPagePrice = 0;     // design, per page
    int cardSidePrice = 0;          // design, per side
    int invitationSidePrice = 0;    // design, per side
    int posterDesignPrice = 0;
    int envelopePrice = 0;

    std::map<std::string, int> printPerSide;   // "A4" -> 300
    std::map<std::string, int> paperPerSheet;  // "kraft" -> 100

    std::vector<int> discountFrom;     // 1, 5, 10, 30
    std::vector<int> discountPercent;  // 0, 5, 10, 20
};

extern Tariffs tariffs;

std::string loadTariffs(const std::string& jsonText);  // returns "" if ok, otherwise error text
