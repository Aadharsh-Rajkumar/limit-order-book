//
// Created by Aadharsh Rajkumar on 9/23/26.
//

module;

#include <cstdint>

export module order;

using namespace std;

export enum class Side {
    BUY,
    SELL
};

export struct Order {
    uint64_t id;
    Side side;
    double price;
    uint64_t quantity;
};