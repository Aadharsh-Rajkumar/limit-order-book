//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#include "exchange_service.h"

#include <json/json.h>
#include <string>
#include <utility>

optional<OrderResult> exchange_service::submit_order(string_view instrument, Side side, double price, uint64_t quantity) {
    optional<OrderResult> result;
    string message;

    lock_guard lock(mutex);

    result = engine.submit_order(instrument, side, price, quantity);
    if (!result) {
        return nullopt;
    }
    message = construct_message(instrument, result->trades);

    market_data.publish(string(instrument),message);
    return result;
}

bool exchange_service::cancel_order(uint64_t order_id) {
    string instrument;
    string message;

    lock_guard lock(mutex);

    auto instrument_result = engine.order_instrument(order_id);
    if (!instrument_result) {
        return false;
    }
    instrument = *instrument_result;
    if (!engine.cancel_order(order_id)) {
        return false;
    }

    message = construct_message(instrument,{});
    market_data.publish(instrument, message);
    return true;
}

vector<double> exchange_service::best_bids(string_view instrument) {
    lock_guard lock(mutex);
    return engine.best_bids(instrument);
}

vector<double> exchange_service::best_asks(string_view instrument) {
    lock_guard lock(mutex);
    return engine.best_asks(instrument);
}

drogon::SubscriberID exchange_service::subscribe_market_data(const string& instrument, drogon::PubSubService<string>::MessageHandler handler) {
    return market_data.subscribe(instrument,std::move(handler));
}

void exchange_service::unsubscribe_market_data(const string& instrument, drogon::SubscriberID subscriber_id) {
    market_data.unsubscribe(instrument, subscriber_id);
}

string exchange_service::construct_message(string_view instrument, const vector<Trade>& trades) {
    Json::Value body;
    body["type"] = "market_data";
    body["instrument"] = string(instrument);

    Json::Value bids(Json::arrayValue);
    for (double price : engine.best_bids(instrument)) {
        bids.append(price);
    }
    Json::Value asks(Json::arrayValue);
    for (double price : engine.best_asks(instrument)) {
        asks.append(price);
    }
    body["bids"] = bids;
    body["asks"] = asks;

    Json::Value trade_array(Json::arrayValue);
    for (const auto& trade : trades) {
        Json::Value trade_json;
        trade_json["buy_order_id"] = Json::UInt64(trade.buy_order_id);
        trade_json["sell_order_id"] = Json::UInt64(trade.sell_order_id);
        trade_json["price"] = trade.price;
        trade_json["quantity"] = Json::UInt64(trade.quantity);
        trade_array.append(trade_json);
    }
    body["trades"] = trade_array;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, body);
}