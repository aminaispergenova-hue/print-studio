#include "Helpers.h"

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// makes letters small, also Russian ones (in UTF-8 they take 2 bytes)
std::string toLower(std::string s) {
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = s[i];
        if (c >= 'A' && c <= 'Z') {
            s[i] = c + 32;
        } else if (c == 0xD0 && i + 1 < s.size()) {
            unsigned char next = s[i + 1];
            if (next >= 0x90 && next <= 0x9F) {          // А..П -> а..п
                s[i + 1] = next + 0x20;
            } else if (next >= 0xA0 && next <= 0xAF) {   // Р..Я -> р..я
                s[i] = 0xD1;
                s[i + 1] = next - 0x20;
            } else if (next == 0x81) {                   // Ё -> ё
                s[i] = 0xD1;
                s[i + 1] = 0x91;
            }
            i++;
        }
    }
    return s;
}

bool contains(const std::string& text, const std::string& part) {
    return toLower(text).find(toLower(trim(part))) != std::string::npos;
}

bool toInt(const std::string& s, int& result) {
    std::string t = trim(s);
    if (t.empty() || t.size() > 7) return false;
    size_t start = (t[0] == '-') ? 1 : 0;
    if (start == t.size()) return false;
    for (size_t i = start; i < t.size(); i++) {
        if (t[i] < '0' || t[i] > '9') return false;
    }
    result = std::stoi(t);
    return true;
}
