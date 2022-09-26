#include <iostream>
#include <cassert>
#include <vector>

template <typename V>
class Node {
    Node *right, *left, *parent;
    V value;

    inline void rotate_right() {
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

    inline void rotate_left() {
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

    inline void local_splay() {
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

    inline void splay() {
        while (parent != nullptr) {
            local_splay();
        }
    }

public:
    inline explicit Node(V _value) : value(_value) {
        right = nullptr;
        left = nullptr;
        parent = nullptr;
    }

    inline void set_left(Node *node) {
        left = node;
        node->parent = this;
    }

    inline void set_right(Node *node) {
        right = node;
        node->parent = this;
    }

    inline Node *get_left() {
        return left;
    }

    inline Node *get_right() {
        return right;
    }

    inline V get_value() {
        return value;
    }

    inline void print() {
        std::cout << value 
            << ": left: " << (left != nullptr ? left->get_value() : -1) 
            << ", right: " << (right != nullptr ? right->get_value() : -1)
            << ", parent: " << (parent != nullptr ? parent->get_value() : -1)
            << std::endl;
    }

    inline void _print_all() {
        print();
        if (left != nullptr) {
            left->_print_all();
        }
        if (right != nullptr) {
            right->_print_all();
        }
    }

    inline void print_all() {
        _print_all();
        std::cout << std::endl;
    }

    inline void set_value(V _value) {
        value = _value;
    }

    inline Node *search(V v) {
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

    inline Node *insert(V v) {
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
};