//
// Created by Aadharsh Rajkumar on 9/23/26.
//

module;

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

export module matching_engine;

import order;
import order_book;

using namespace std;

export struct Trade {
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double price;
    uint64_t quantity;
};

export struct OrderResult {
    uint64_t order_id;
    vector<Trade> trades;
};

export class MatchingEngine {
public:
    optional<OrderResult> submit_order(string_view instrument, Side side, double price, uint64_t quantity);
    bool cancel_order(uint64_t order_id);

    vector<double> best_bids(string_view instrument);
    vector<double> best_asks(string_view instrument);

    optional<string> order_instrument(uint64_t order_id);
private:
    Exchange exchange;
    uint64_t next_id = 1;
};