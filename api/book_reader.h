//
// Created by Aadharsh Rajkumar on 9/25/26.
//

#pragma once

#include <drogon/HttpController.h>

#include <functional>
#include <memory>

class exchange_service;

class book_reader: public drogon::HttpController<book_reader, false> {
public:
    explicit book_reader(std::shared_ptr<exchange_service> service);

    METHOD_LIST_BEGIN
    ADD_METHOD_TO(book_reader::get_book, "/book", drogon::Get);
    METHOD_LIST_END

    void get_book(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
private:
    std::shared_ptr<exchange_service> service;
};
