//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#include "order_controller.h"

#include "exchange_service.h"

#include <json/json.h>

#include <cstdint>
#include <string>
#include <utility>

#include "order_controller.h"

#include "exchange_service.h"

#include <json/json.h>

#include <cstdint>
#include <string>
#include <utility>

import order;

using namespace drogon;
using namespace std;

order_controller::order_controller(shared_ptr<exchange_service> service) : service(std::move(service)) {}

void order_controller::submit_order(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (json == nullptr) {
        Json::Value body;
        body["error"] = "Request body must be valid JSON";
        auto response = HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(k400BadRequest);
        callback(response);
        return;
    }

    if (!json->isMember("instrument") || !json->isMember("side") || !json->isMember("price") || !json->isMember("quantity")) {
        Json::Value body;
        body["error"] = "Missing required field";
        auto response = HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(k400BadRequest);
        callback(response);
        return;
    }

    string instrument = (*json)["instrument"].asString();
    string side_string = (*json)["side"].asString();
    double price = (*json)["price"].asDouble();
    uint64_t quantity = (*json)["quantity"].asUInt64();

    Side side;
    if (side_string == "BUY") {
        side = Side::BUY;
    } else if (side_string == "SELL") {
        side = Side::SELL;
    } else {
        Json::Value body;
        body["error"] = "Side must be BUY or SELL";
        auto response = HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(k400BadRequest);
        callback(response);
        return;
    }

    auto result = service->submit_order(instrument, side, price, quantity);

    if (!result) {
        Json::Value body;
        body["error"] = "Invalid order";
        auto response = HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(k400BadRequest);
        callback(response);
        return;
    }

    Json::Value body;
    body["order_id"] = Json::UInt64(result->order_id);
    Json::Value trades(Json::arrayValue);

    for (const auto& trade : result->trades) {
        Json::Value trade_json;
        trade_json["buy_order_id"] = Json::UInt64(trade.buy_order_id);
        trade_json["sell_order_id"] = Json::UInt64(trade.sell_order_id);
        trade_json["price"] = trade.price;
        trade_json["quantity"] = Json::UInt64(trade.quantity);
        trades.append(trade_json);
    }

    body["trades"] = trades;
    auto response = HttpResponse::newHttpJsonResponse(body);

    callback(response);
}

void order_controller::cancel_order(const HttpRequestPtr&, function<void(const HttpResponsePtr&)>&& callback, uint64_t order_id) {
    if (!service->cancel_order(order_id)) {
        Json::Value body;
        body["error"] = "Order not found";
        auto response = HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(k404NotFound);
        callback(response);
        return;
    }

    Json::Value body;
    body["order_id"] = Json::UInt64(order_id);
    body["cancelled"] = true;
    auto response = HttpResponse::newHttpJsonResponse(body);
    callback(response);
}