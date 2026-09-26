//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#pragma once

#include <cstdint>
#include <mutex>
#include <optional>
#include <string_view>
#include <vector>
#include <drogon/PubSubService.h>

import matching_engine;
import order;

using namespace std;

class exchange_service {
public:
    optional<OrderResult> submit_order(string_view instrument, Side side, double price, uint64_t quantity);
    bool cancel_order(uint64_t order_id);

    vector<double> best_bids(string_view instrument);
    vector<double> best_asks(string_view instrument);

    drogon::SubscriberID subscribe_market_data(const std::string& instrument, drogon::PubSubService<std::string>::MessageHandler handler);
    void unsubscribe_market_data(const std::string& instrument, drogon::SubscriberID subscriber_id);

private:
    MatchingEngine engine;
    std::mutex mutex;

    drogon::PubSubService<std::string> market_data;

    std::string construct_message(std::string_view instrument, const std::vector<Trade>& trades);
};


