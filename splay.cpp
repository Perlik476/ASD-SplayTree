#include <iostream>
#include <cassert>
#include <vector>
#include "splay.hpp"

template <typename V>
void Node<V>::rotate_right() {
    assert(parent->left == this);

    auto grandparent = parent->parent;
    auto current_parent = parent;

    if (this->right != nullptr) {
        this->right->parent = current_parent;
    }

    current_parent->left = this->right;
    current_parent->parent = this;

    this->parent = grandparent;
    this->right = current_parent;
    
    if (grandparent != nullptr) {
        if (grandparent->left == current_parent) {
            grandparent->left = this;
        }
        else {
            grandparent->right = this;
        }
    }
}

template <typename V>
void Node<V>::rotate_left() {
    assert(parent->right == this);

    auto grandparent = parent->parent;
    auto current_parent = parent;

    if (this->left != nullptr) {
        this->left->parent = current_parent;
    }

    current_parent->right = this->left;
    current_parent->parent = this;

    this->parent = grandparent;     
    this->left = current_parent;

    if (grandparent != nullptr) {
        if (grandparent->left == current_parent) {
            grandparent->left = this;
        }
        else {
            grandparent->right = this;
        }
    }
}

template <typename V>
void Node<V>::local_splay() {
    auto grandparent = parent->parent;

    if (grandparent == nullptr) { // node's parent is root
        assert(parent->get_left() == this || parent->get_right() == this);

        if (parent->get_left() == this) {
            rotate_right();
        }
        else {
            rotate_left();
        }
    }
    else {
        auto grandgrandparent = grandparent->parent;

        if (parent->get_left() == this && grandparent->get_left() == parent) {
            parent->rotate_right();
            rotate_right();
        }
        else if (parent->get_right() == this && grandparent->get_right() == parent) {
            parent->rotate_left();
            rotate_left();
        }
        else if (parent->get_right() == this && grandparent->get_left() == parent) {
            rotate_left();
            rotate_right();
        }
        else if (parent->get_left() == this && grandparent->get_right() == parent) {
            rotate_right();
            rotate_left();
        }
    }
}

template <typename V>
void Node<V>::splay() {
    while (parent != nullptr) {
        local_splay();
    }
}

template <typename V>
Node<V>::Node(V _value) : value(_value) {
    right = nullptr;
    left = nullptr;
    parent = nullptr;
}

template <typename V>
void Node<V>::set_left(Node *node) {
    left = node;
    node->parent = this;
}

template <typename V>
void Node<V>::set_right(Node *node) {
    right = node;
    node->parent = this;
}

template <typename V>
Node<V> *Node<V>::get_left() {
    return left;
}

template <typename V>
Node<V> *Node<V>::get_right() {
    return right;
}

template <typename V>
V Node<V>::get_value() {
    return value;
}

template <typename V>
void Node<V>::print() {
    std::cout << value 
        << ": left: " << (left != nullptr ? left->get_value() : -1) 
        << ", right: " << (right != nullptr ? right->get_value() : -1)
        << ", parent: " << (parent != nullptr ? parent->get_value() : -1)
        << std::endl;
}

template <typename V>
void Node<V>::_print_all() {
    print();
    if (left != nullptr) {
        left->_print_all();
    }
    if (right != nullptr) {
        right->_print_all();
    }
}

template <typename V>
void Node<V>::print_all() {
    _print_all();
    std::cout << std::endl;
}

template <typename V>
void Node<V>::set_value(V _value) {
    value = _value;
}

template <typename V>
Node<V> *Node<V>::search(V v) {
    if (v < this->value) {
        if (this->left != nullptr) {
            return this->left->search(v);
        }
        else {
            splay();
            return nullptr;
        }
    }
    else if (v > this->value) {
        if (this->right != nullptr) {
            return this->right->search(v);
        }
        else {
            splay();
            return nullptr;
        }
    }
    else {
        splay();
        return this;
    }
}

template <typename V>
Node<V> *Node<V>::insert(V v) {
    Node *node = nullptr;
    if (v < this->value) {
        if (this->left != nullptr) {
            return this->left->insert(v);
        }
        else {
            node = new Node(v);

            this->left = node;
            node->parent = this;
            node->splay();

            return node;
        }
    }
    else if (v > this->value) {
        if (this->right != nullptr) {
            return this->right->insert(v);
        }
        else {
            node = new Node(v);

            this->right = node;
            node->parent = this;
            node->splay();

            return node;
        }
    }
    else {
        splay();
        return this;
    }
}


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

    return 0;
}
