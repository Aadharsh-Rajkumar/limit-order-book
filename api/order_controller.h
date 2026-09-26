//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#pragma once

#include <drogon/HttpController.h>

#include <cstdint>
#include <functional>
#include <memory>

class exchange_service;

class order_controller: public drogon::HttpController<order_controller, false> {
public:
    explicit order_controller(std::shared_ptr<exchange_service> service);

    METHOD_LIST_BEGIN
    ADD_METHOD_TO(order_controller::submit_order, "/orders", drogon::Post);
    ADD_METHOD_TO(order_controller::cancel_order, "/orders/{1}", drogon::Delete);
    METHOD_LIST_END

    void submit_order(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void cancel_order(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, uint64_t order_id);
private:
    std::shared_ptr<exchange_service> service;
};


