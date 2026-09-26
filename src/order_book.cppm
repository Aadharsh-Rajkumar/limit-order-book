//
// Created by Aadharsh Rajkumar on 9/23/26.
//

module;

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>

export module order_book;

import order;

using namespace std;

export class DLLNode {
public:
    Order order;
    DLLNode* prev{};
    DLLNode* next{};

    DLLNode() = default;

    explicit DLLNode(const Order& order) : order(order) {}
};

class Iterator {
public:
    explicit Iterator(DLLNode* node)
        : current(node) {}

    Order& operator*() {
        return current->order;
    }

    Order* operator->() {
        return &current->order;
    }

    Iterator& operator++() {
        current = current->next;
        return *this;
    }

    bool operator!=(const Iterator& other) const {
        return current != other.current;
    }
private:
    DLLNode* current;
};

class DoublyLinkedList {
public:
    DoublyLinkedList() {
        head = new DLLNode();
        tail = new DLLNode();

        head->next = tail;
        tail->prev = head;
    }

    ~DoublyLinkedList() {
        while (!empty()) {
            pop_front();
        }
        delete head;
        delete tail;
    }

    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    DLLNode* push_back(const Order& order) {
        DLLNode* node = new DLLNode(order);
        node->prev = tail->prev;
        node->next = tail;
        tail->prev->next = node;
        tail->prev = node;

        return node;
    }

    void pop_front() {
        if (empty()) {
            return;
        }
        DLLNode* node = head->next;
        head->next = node->next;
        node->next->prev = head;

        delete node;
    }

    bool empty() const {
        return head->next == tail;
    }

    DLLNode* front() {
        if (!empty()) {
            return head->next;
        }
        return nullptr;
    }

    Order& front_order() {
        if (!empty()) {
            return head->next->order;
        }
        throw std::out_of_range("Cannot access front of empty list");
    }

    void remove(DLLNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        delete node;
    }

    Iterator begin() {
        return Iterator(head->next);
    }

    Iterator end() {
        return Iterator(tail);
    }

private:
    DLLNode* head{};
    DLLNode* tail{};
};

export class OrderBook {
public:
    explicit OrderBook(string instrument): instrument(std::move(instrument)) {}

    void add_order(Side side, double price, uint64_t id, uint64_t quantity);
    bool cancel_order(uint64_t order_id);

    vector<double> best_bids();
    vector<double> best_asks();

    DLLNode* best_bid_order();
    DLLNode* best_ask_order();

    bool empty() const {
        return orders.empty();
    }

    string_view get_instrument() const {
        return instrument;
    }

private:
    string instrument;
    unordered_map<uint64_t, DLLNode*> orders;
    std::map<double, DoublyLinkedList, greater<>> bids;
    std::map<double, DoublyLinkedList> asks;
};

export class Exchange {
public:
    void add_order(string_view instrument, Side side, double price, uint64_t id, uint64_t quantity);

    bool cancel_order(uint64_t order_id);

    optional<string> order_instrument(uint64_t order_id);

    vector<double> best_bids(string_view instrument);
    vector<double> best_asks(string_view instrument);

    OrderBook* get_book(string_view instrument);

    const unordered_map<string, OrderBook>& get_books() const {
        return books;
    }

private:
    unordered_map<string, OrderBook> books;
    unordered_map<uint64_t, OrderBook*> order_books;
};