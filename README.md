# C++ Exchange

An in-memory limit order book and matching engine built with C++20 and Drogon (built to support multiple symbols).

## Features
- Limit order submission and cancellation
- Price-time priority matching
- Partial fills and multi-order matching
- REST API for orders and book data
- WebSocket market data streaming
- Thread-safe shared exchange state with a single lock

## Endpoints
- `POST /orders` — submit a limit order
- `DELETE /orders/{id}` — cancel an order
- `GET /book?instrument=AAPL` — view the top 5 bid/ask levels
- `WS /marketdata?instrument=AAPL` — stream book and trade updates

## Build

```bash
cmake -S . -B cmake-build-debug -G Ninja
cmake --build cmake-build-debug
```

## Run Server

```bash
./cmake-build-debug/TGT_C___Exchange
```

## Run Tests
```bash
./cmake-build-debug/matching_engine_tests
```
