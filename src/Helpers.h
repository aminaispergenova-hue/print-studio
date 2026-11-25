#pragma once
#include <string>

// small text functions used in several classes

std::string trim(const std::string& s);
std::string toLower(std::string s);
bool contains(const std::string& text, const std::string& part);  // ignores letter case
bool toInt(const std::string& s, int& result);                     // "12" -> 12, "2.5" -> false
