// connects the C++ class Studio to JavaScript (Emscripten embind)
// so in app.js we can write studio.addDraftItem("magazine")
#include <emscripten/bind.h>

#include "Studio.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(studio) {
    class_<Studio>("Studio")
        .constructor<>()
        .function("loadTariffs", &Studio::loadTariffs)
        .function("loadData", &Studio::loadData)
        .function("exportData", &Studio::exportData)
        .function("findCustomers", &Studio::findCustomers)
        .function("addCustomer", &Studio::addCustomer)
        .function("getCustomer", &Studio::getCustomer)
        .function("startDraft", &Studio::startDraft)
        .function("getDraft", &Studio::getDraft)
        .function("setDraftInfo", &Studio::setDraftInfo)
        .function("addDraftItem", &Studio::addDraftItem)
        .function("removeDraftItem", &Studio::removeDraftItem)
        .function("setDraftItemOption", &Studio::setDraftItemOption)
        .function("setDraftItemQuantity", &Studio::setDraftItemQuantity)
        .function("saveDraft", &Studio::saveDraft)
        .function("getOrder", &Studio::getOrder)
        .function("setItemStage", &Studio::setItemStage)
        .function("deleteOrder", &Studio::deleteOrder)
        .function("listOrders", &Studio::listOrders)
        .function("stageNames", &Studio::stageNames);
}
