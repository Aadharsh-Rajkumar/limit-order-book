//
// Created by Aadharsh Rajkumar on 9/23/26.
//

module;

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>
#include <string_view>
#include <string>

module matching_engine;

using namespace std;

optional<OrderResult> MatchingEngine::submit_order(string_view instrument, Side side, double price, uint64_t quantity) {
    if (quantity == 0 or price <= 0 or instrument.empty()) {
        return std::nullopt;
    }

    uint64_t order_id = next_id++;
    vector<Trade> trades;

    while (quantity > 0) {
        OrderBook* book = exchange.get_book(instrument);
        if (book == nullptr) {
            break;
        }

        DLLNode* opposite_order = nullptr;
        if (side == Side::BUY) {
            opposite_order = book->best_ask_order();
            if (opposite_order == nullptr || opposite_order->order.price > price) {
                break;
            }
        } else {
            opposite_order = book->best_bid_order();
            if (opposite_order == nullptr || opposite_order->order.price < price) {
                break;
            }
        }

        uint64_t trade_quantity = min(quantity, opposite_order->order.quantity);
        uint64_t resting_order_id = opposite_order->order.id;
        double trade_price = opposite_order->order.price;

        if (side == Side::BUY) {
            trades.push_back({order_id,resting_order_id, trade_price, trade_quantity});
        } else {
            trades.push_back({resting_order_id,order_id, trade_price, trade_quantity});
        }

        quantity -= trade_quantity;
        opposite_order->order.quantity -= trade_quantity;
        if (opposite_order->order.quantity == 0) {
            exchange.cancel_order(resting_order_id);
        }
    }

    if (quantity > 0) {
        exchange.add_order(instrument, side, price, order_id, quantity);
    }

    return OrderResult{order_id,trades};
}

optional<string> MatchingEngine::order_instrument(uint64_t order_id) {
    return exchange.order_instrument(order_id);
}

bool MatchingEngine::cancel_order(uint64_t order_id) {
    return exchange.cancel_order(order_id);
}

vector<double> MatchingEngine::best_bids(string_view instrument) {
    return exchange.best_bids(instrument);
}

vector<double> MatchingEngine::best_asks(string_view instrument) {
    return exchange.best_asks(instrument);
}

