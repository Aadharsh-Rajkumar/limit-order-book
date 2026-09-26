#include <drogon/drogon.h>

#include <memory>
#include <iostream>

#include "api/book_data_streamer.h"
#include "api/book_reader.h"
#include "api/exchange_service.h"
#include "api/order_controller.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    auto exchange = std::make_shared<exchange_service>();

    drogon::app().registerController(
        std::make_shared<order_controller>(exchange)
    );

    drogon::app().registerController(
        std::make_shared<book_reader>(exchange)
    );

    drogon::app().registerController(
        std::make_shared<book_data_streamer>(exchange)
    );

    std::cout << "Controllers registered. Starting server...\n";

    drogon::app()
        .addListener("127.0.0.1", 8080)
        .setThreadNum(4)
        .run();

    return 0;
}