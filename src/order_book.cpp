//
// Created by Aadharsh Rajkumar on 9/23/26.
//

module;

#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

module order_book;

using namespace std;

vector<double> OrderBook::best_bids() {
    vector<double> best_bids;
    auto it = bids.begin();
    while (it != bids.end() and best_bids.size() < 5) {
        best_bids.push_back(it->first);
        ++it;
    }
    return best_bids;
}

vector<double> OrderBook::best_asks() {
    vector<double> best_asks;
    auto it = asks.begin();
    while (it != asks.end() and best_asks.size() < 5) {
        best_asks.push_back(it->first);
        ++it;
    }
    return best_asks;
}

vector<double> Exchange::best_bids(string_view instrument) {
    auto it = books.find(string(instrument));
    if (it == books.end()) {
        return {};
    }
    return it->second.best_bids();
}

vector<double> Exchange::best_asks(string_view instrument) {
    auto it = books.find(string(instrument));
    if (it == books.end()) {
        return {};
    }
    return it->second.best_asks();
}

void OrderBook::add_order(Side side, double price, uint64_t id, uint64_t quantity) {
    Order order{id, side, price, quantity};

    if (side == Side::BUY) {
        DLLNode* node = bids[price].push_back(order);
        orders[id] = node;
    } else {
        DLLNode* node = asks[price].push_back(order);
        orders[id] = node;
    }
}

bool OrderBook::cancel_order(uint64_t order_id) {
    auto it = orders.find(order_id);
    if (it == orders.end()) {
        return false;
    }
    DLLNode* node = it->second;
    Order& order = node->order;

    if (order.side == Side::BUY) {
        auto price_it = bids.find(order.price);
        price_it->second.remove(node);
        if (price_it->second.empty()) {
            bids.erase(price_it);
        }
    } else {
        auto price_it = asks.find(order.price);
        price_it->second.remove(node);
        if (price_it->second.empty()) {
            asks.erase(price_it);
        }
    }

    orders.erase(it);
    return true;
}

void Exchange::add_order(
    string_view instrument,
    Side side,
    double price,
    uint64_t id,
    uint64_t quantity
) {
    string instrument_name(instrument);
    auto it = books.find(instrument_name);

    if (it == books.end()) {
        auto [new_it, _] =
            books.try_emplace(
                instrument_name,
                instrument_name
            );

        it = new_it;
    }

    OrderBook& book = it->second;
    book.add_order(side, price, id, quantity);
    order_books[id] = &book;
}

bool Exchange::cancel_order(uint64_t order_id) {
    auto it = order_books.find(order_id);
    if (it == order_books.end()) {
        return false;
    }

    OrderBook* book = it->second;
    if (!book->cancel_order(order_id)) {
        return false;
    }

    order_books.erase(it);
    if (book->empty()) {
        books.erase(string(book->get_instrument()));
    }

    return true;
}

DLLNode* OrderBook::best_bid_order() {
    if (bids.empty()) {
        return nullptr;
    }
    return bids.begin()->second.front();
}

DLLNode* OrderBook::best_ask_order() {
    if (asks.empty()) {
        return nullptr;
    }
    return asks.begin()->second.front();
}

OrderBook* Exchange::get_book(string_view instrument) {
    auto it = books.find(string(instrument));
    if (it == books.end()) {
        return nullptr;
    }
    return &it->second;
}

optional<string> Exchange::order_instrument(uint64_t order_id) {
    auto it = order_books.find(order_id);

    if (it == order_books.end()) {
        return nullopt;
    }

    return string(it->second->get_instrument());
}