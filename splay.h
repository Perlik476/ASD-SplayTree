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

        current_parent->set_left(right);
        set_right(current_parent);

        parent = grandparent;
        
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

        current_parent->set_right(left);
        set_left(current_parent);

        parent = grandparent;     

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
        if (v < value) {
            if (left != nullptr) {
                return left->search(v);
            }
            else {
                splay();
                return nullptr;
            }
        }
        else if (v > value) {
            if (right != nullptr) {
                return right->search(v);
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

        if (v < value) {
            if (left != nullptr) {
                return left->insert(v);
            }
            else {
                node = std::make_shared<Node<V>>(v);

                set_left(node);
                node->splay();

                return node;
            }
        }
        else if (v > value) {
            if (right != nullptr) {
                return right->insert(v);
            }
            else {
                node = std::make_shared<Node<V>>(v);

                set_right(node);
                node->splay();

                return node;
            }
        }
        else {
            splay();
            return get_ptr();
        }
    }

    inline node_ptr_t remove(V v) {
        if (v < value) {
            if (left != nullptr) {
                return left->remove(v);
            }
            else {
                splay();

                return get_ptr();
            }
        }
        else if (v > value) {
            if (right != nullptr) {
                return right->remove(v);
            }
            else {
                splay();

                return get_ptr();
            }
        }
        else {
            node_ptr_t new_root = parent;

            if (left == nullptr && right == nullptr) {
                if (parent != nullptr) {
                    if (parent->left == get_ptr()) {
                        parent->left = nullptr;
                    }
                    else {
                        parent->right = nullptr;
                    }
                }
            }
            else if ((left != nullptr && right == nullptr) || (right != nullptr && left == nullptr)) {
                node_ptr_t child = left == nullptr ? right : left;

                if (parent == nullptr) {
                    new_root = child;
                }
                else if (parent->left == get_ptr()) {
                    parent->set_left(child);
                }
                else {
                    parent->set_right(child);
                }
            }
            else {
                node_ptr_t node = left;
                while (node->right != nullptr) {
                    node = node->right;
                }

                set_value(node->get_value());
                if (node == left) {
                    set_left(node->left);
                }
                else {
                    node->parent->set_right(node->left);
                }
            }

            if (new_root != nullptr) {
                new_root->splay();
            }

            return new_root;
        }
    }
};

template<typename V>
class SplayTree {
    using node_ptr_t = std::shared_ptr<Node<V>>;

    node_ptr_t root;

public:
    SplayTree() {
        root = nullptr;
    }

    SplayTree(std::initializer_list<V> list) {
        for (V value : list) {
            insert(value);
        }
    }

    void insert(V value) {
        if (root == nullptr) {
            root = std::make_shared<Node<V>>(value);
        }
        else {
            root = root->insert(value);
        }
    }

    bool contains(V value) {
        if (root == nullptr) {
            return false;
        }

        root = root->search(value);
        return root->get_value() == value;
    }

    void remove(V value) {
        if (root == nullptr) {
            return;
        }
        else {
            root = root->remove(value);
        }
    }

    // class Iterator : public std::iterator<std::bidirectional_iterator_tag, V, std::ptrdiff_t, V *, V &> {

    // }
};