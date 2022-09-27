#include <iostream>
#include "splay.h"

int main() {
    auto root = std::make_shared<Node<int>>(5);
    auto right = std::make_shared<Node<int>>(7);
    auto left = std::make_shared<Node<int>>(2);
    root->set_right(right);
    root->set_left(left);
    auto left_left = std::make_shared<Node<int>>(1);
    auto left_right = std::make_shared<Node<int>>(4);
    left->set_left(left_left);
    left->set_right(left_right);
    auto left_left_left = std::make_shared<Node<int>>(0);
    left_left->set_left(left_left_left);
    root->print_all();

    auto result = root->search(0);
    result->print_all();

    result = result->search(4);
    result->print_all();

    result = result->insert(3);
    result->print_all();

    result = result->search(5);
    result->print_all();

    // result = result->remove(1);
    result->print_all();

    // result = result->remove(4);
    result->print_all();

    result = result->insert(6);
    result = result->insert(21);
    result = result->insert(37);
    result = result->insert(-2);

    result = result->search(5);
    result->print_all();

    result = result->search(21);
    result->print_all();

    result = result->search(22);
    result->print_all();

    result = result->search(-2);
    result->print_all();

    return 0;
}
