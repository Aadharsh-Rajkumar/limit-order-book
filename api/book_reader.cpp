//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#include "book_reader.h"

#include "exchange_service.h"

#include <json/json.h>
#include <string>
#include <utility>

using namespace drogon;
using namespace std;

book_reader::book_reader(std::shared_ptr<exchange_service> service) : service(std::move(service)) {}

void book_reader::get_book(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback) {
    string instrument = req->getParameter("instrument");
    if (instrument.empty()) {
        Json::Value body;
        body["error"] = "Missing instrument";
        auto response = HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(k400BadRequest);
        callback(response);
        return;
    }

    auto bids = service->best_bids(instrument);
    auto asks = service->best_asks(instrument);

    Json::Value body;
    body["instrument"] = instrument;
    Json::Value bid_array(Json::arrayValue);
    for (double price : bids) {
        bid_array.append(price);
    }
    Json::Value ask_array(Json::arrayValue);
    for (double price : asks) {
        ask_array.append(price);
    }
    body["bids"] = bid_array;
    body["asks"] = ask_array;

    auto response = HttpResponse::newHttpJsonResponse(body);
    callback(response);
}