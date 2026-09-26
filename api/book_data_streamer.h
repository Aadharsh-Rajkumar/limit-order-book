//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#pragma once

#include <drogon/WebSocketController.h>
#include <drogon/PubSubService.h>

#include <memory>
#include <string>

class exchange_service;

class book_data_streamer: public drogon::WebSocketController<book_data_streamer, false> {
public:
    explicit book_data_streamer(std::shared_ptr<exchange_service> service);

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/marketdata");
    WS_PATH_LIST_END

    void handleNewMessage(const drogon::WebSocketConnectionPtr& connection, std::string&& message, const drogon::WebSocketMessageType& type) override;

    void handleNewConnection(const drogon::HttpRequestPtr& request, const drogon::WebSocketConnectionPtr& connection) override;

    void handleConnectionClosed(const drogon::WebSocketConnectionPtr& connection) override;
private:
    struct Subscription {
        std::string instrument;
        drogon::SubscriberID subscriber_id;
    };

    std::shared_ptr<exchange_service> service;
};

