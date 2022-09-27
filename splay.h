#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

template <typename V>
class Node : public std::enable_shared_from_this<Node<V>> {
    using node_ptr_t = std::shared_ptr<Node<V>>;

    node_ptr_t right, left, parent;
    V value;

    node_ptr_t get_ptr() {
        return this->shared_from_this();
    }

    inline void rotate_right() {
        assert(parent->left == get_ptr());

        auto grandparent = parent->parent;
        auto current_parent = parent;

        current_parent->set_left(this->right);
        this->set_right(current_parent);

        this->parent = grandparent;
        
        if (grandparent != nullptr) {
            if (grandparent->left == current_parent) {
                grandparent->left = get_ptr();
            }
            else {
                grandparent->right = get_ptr();
            }
        }
    }

    inline void rotate_left() {
        assert(parent->right == get_ptr());

        auto grandparent = parent->parent;
        auto current_parent = parent;

        current_parent->set_right(this->left);
        this->set_left(current_parent);

        this->parent = grandparent;     

        if (grandparent != nullptr) {
            if (grandparent->left == current_parent) {
                grandparent->left = get_ptr();
            }
            else {
                grandparent->right = get_ptr();
            }
        }
    }

    inline void local_splay() {
        auto grandparent = parent->parent;

        if (grandparent == nullptr) {
            assert(parent->get_left() == get_ptr() || parent->get_right() == get_ptr());

            if (parent->get_left() == get_ptr()) {
                rotate_right();
            }
            else {
                rotate_left();
            }
        }
        else {
            auto grandgrandparent = grandparent->parent;

            if (parent->get_left() == get_ptr() && grandparent->get_left() == parent) {
                parent->rotate_right();
                rotate_right();
            }
            else if (parent->get_right() == get_ptr() && grandparent->get_right() == parent) {
                parent->rotate_left();
                rotate_left();
            }
            else if (parent->get_right() == get_ptr() && grandparent->get_left() == parent) {
                rotate_left();
                rotate_right();
            }
            else if (parent->get_left() == get_ptr() && grandparent->get_right() == parent) {
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

    inline void set_left(node_ptr_t node) {
        left = node;
        if (node != nullptr) {
            node->parent = get_ptr();
        }
    }

    inline void set_right(node_ptr_t node) {
        right = node;
        if (node != nullptr) {
            node->parent = get_ptr();
        }
    }

    inline node_ptr_t get_left() {
        return left;
    }

    inline node_ptr_t get_right() {
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

    inline node_ptr_t search(V v) {
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
            return get_ptr();
        }
    }

    inline node_ptr_t insert(V v) {
        node_ptr_t node = nullptr;
        if (v < this->value) {
            if (this->left != nullptr) {
                return this->left->insert(v);
            }
            else {
                node = std::make_shared<Node<V>>(v);

                this->set_left(node);

                node->splay();

                return node;
            }
        }
        else if (v > this->value) {
            if (this->right != nullptr) {
                return this->right->insert(v);
            }
            else {
                node = std::make_shared<Node<V>>(v);

                this->set_right(node);
                
                node->splay();

                return node;
            }
        }
        else {
            splay();
            return get_ptr();
        }
    }
};

template<typename V>
class SplayTree {
    std::shared_ptr<Node<V>> root;
};