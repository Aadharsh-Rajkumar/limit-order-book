//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#include "book_data_streamer.h"

#include "exchange_service.h"

#include <drogon/drogon.h>

#include <memory>
#include <string>
#include <utility>

using namespace drogon;
using namespace std;

book_data_streamer::book_data_streamer(shared_ptr<exchange_service> service) : service(std::move(service)) {}

void book_data_streamer::handleNewConnection(const HttpRequestPtr& request, const WebSocketConnectionPtr& connection) {
    string instrument = request->getParameter("instrument");
    if (instrument.empty()) {
        connection->shutdown();
        return;
    }

    auto subscription = make_shared<Subscription>();
    subscription->instrument = instrument;
    subscription->subscriber_id = service->subscribe_market_data(instrument,[connection](const string&, const string& message) {
        if (connection->connected()) {
            connection->send(message);
        }
    });

    connection->setContext(subscription);
    Json::Value body;
    body["type"] = "book";
    body["instrument"] = instrument;
    Json::Value bids(Json::arrayValue);
    for (double price : service->best_bids(instrument)) {
        bids.append(price);
    }
    Json::Value asks(Json::arrayValue);
    for (double price : service->best_asks(instrument)) {
        asks.append(price);
    }

    body["bids"] = bids;
    body["asks"] = asks;

    Json::StreamWriterBuilder writer;
    connection->send(Json::writeString(writer, body));
}

void book_data_streamer::handleNewMessage(
    const WebSocketConnectionPtr& connection,
    string&&,
    const WebSocketMessageType&
) {
    if (!connection->connected()) {
        return;
    }
}

void book_data_streamer::handleConnectionClosed(
    const WebSocketConnectionPtr& connection
) {
    auto subscription = connection->getContext<shared_ptr<Subscription>>();
    if (!subscription) {
        return;
    }

    service->unsubscribe_market_data((*subscription)->instrument, (*subscription)->subscriber_id);
}
