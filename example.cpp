#include <iostream>
#include "splay.h"

int main() {
    Node<int> root = Node(5);
    Node<int> right = Node(7);
    Node<int> left = Node(2);
    root.set_right(&right);
    root.set_left(&left);
    Node<int> left_left = Node(1);
    Node<int> left_right = Node(4);
    left.set_left(&left_left);
    left.set_right(&left_right);
    Node<int> left_left_left = Node(0);
    left_left.set_left(&left_left_left);
    root.print_all();

    Node<int> *result = root.search(0);
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
