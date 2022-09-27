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
//    std::cout << left.get_right()->get_value();

    Node<char> n = Node('a');
    Node<std::vector<int>> n2 = Node(std::vector<int>());

    return 0;
}
