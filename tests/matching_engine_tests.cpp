//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#include <catch2/catch_test_macros.hpp>

#include <cstdint>

import matching_engine;
import order;

TEST_CASE("Price priority") {
    MatchingEngine engine;

    auto first = engine.submit_order("AAPL", Side::SELL,101.0,100);

    auto best = engine.submit_order("AAPL", Side::SELL,100.0,100);

    REQUIRE(first.has_value());
    REQUIRE(best.has_value());

    auto result = engine.submit_order("AAPL", Side::BUY,105.0,50);

    REQUIRE(result.has_value());
    REQUIRE(result->trades.size() == 1);

    CHECK(result->trades[0].sell_order_id == best->order_id);
    CHECK(result->trades[0].price == 100.0);
    CHECK(result->trades[0].quantity == 50);
}

TEST_CASE("Same price uses FIFO") {
    MatchingEngine engine;

    auto first = engine.submit_order("AAPL", Side::SELL,100.0,100);

    auto second = engine.submit_order("AAPL",Side::SELL,100.0,100);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());

    auto result = engine.submit_order("AAPL", Side::BUY,100.0,50);

    REQUIRE(result.has_value());
    REQUIRE(result->trades.size() == 1);
    CHECK(result->trades[0].sell_order_id == first->order_id);
    CHECK(result->trades[0].quantity == 50);
}

TEST_CASE("Partial fill") {
    MatchingEngine engine;

    auto resting = engine.submit_order("AAPL", Side::SELL,100.0,100);

    REQUIRE(resting.has_value());

    auto first_fill = engine.submit_order("AAPL", Side::BUY,100.0,40);

    REQUIRE(first_fill.has_value());
    REQUIRE(first_fill->trades.size() == 1);
    CHECK(first_fill->trades[0].sell_order_id == resting->order_id);
    CHECK(first_fill->trades[0].quantity == 40);

    auto second_fill = engine.submit_order("AAPL",Side::BUY,100.0,60);

    REQUIRE(second_fill.has_value());
    REQUIRE(second_fill->trades.size() == 1);
    CHECK(second_fill->trades[0].sell_order_id == resting->order_id);
    CHECK(second_fill->trades[0].quantity == 60);
}

TEST_CASE("Using multiple opposite orders") {
    MatchingEngine engine;

    auto first = engine.submit_order("AAPL", Side::SELL,100.0,30);

    auto second = engine.submit_order("AAPL",Side::SELL,101.0,40);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());

    auto result = engine.submit_order("AAPL",Side::BUY,105.0,100);

    REQUIRE(result.has_value());
    REQUIRE(result->trades.size() == 2);
    CHECK(result->trades[0].sell_order_id == first->order_id);
    CHECK(result->trades[0].price == 100.0);
    CHECK(result->trades[0].quantity == 30);
    CHECK(result->trades[1].sell_order_id == second->order_id);
    CHECK(result->trades[1].price == 101.0);
    CHECK(result->trades[1].quantity == 40);

    auto bids = engine.best_bids("AAPL");
    REQUIRE(bids.size() == 1);
    CHECK(bids[0] == 105.0);
}

TEST_CASE("Cancellation") {
    MatchingEngine engine;

    auto resting = engine.submit_order("AAPL", Side::SELL, 100.0,100);

    REQUIRE(resting.has_value());
    CHECK(engine.cancel_order(resting->order_id));

    auto result = engine.submit_order("AAPL",Side::BUY,100.0,50);

    REQUIRE(result.has_value());
    CHECK(result->trades.empty());

    auto asks = engine.best_asks("AAPL");
    CHECK(asks.empty());
}

TEST_CASE("Invalid cancellation") {
    MatchingEngine engine;
    CHECK_FALSE(engine.cancel_order(999999));
}

TEST_CASE("Invalid orders") {
    MatchingEngine engine;

    CHECK_FALSE(engine.submit_order("AAPL",Side::BUY,100.0,0).has_value());
    CHECK_FALSE(engine.submit_order("AAPL",Side::BUY,0.0,100).has_value());
    CHECK_FALSE(engine.submit_order("",Side::BUY,100.0,100).has_value());
}