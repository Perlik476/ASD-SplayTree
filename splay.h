#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <stack>

template <typename V>
class Node : public std::enable_shared_from_this<Node<V>> {
    using node_ptr_t = std::shared_ptr<Node<V>>;

    node_ptr_t right, left, parent;
    V value;

    node_ptr_t get_ptr() {
        return this->shared_from_this();
    }

    void rotate_right() {
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

    void rotate_left() {
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

    void local_splay() {
        auto grandparent = parent->parent;
        auto ptr = get_ptr();

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

    void splay() {
        while (parent != nullptr) {
            local_splay();
        }
    }

public:
    explicit Node(V _value) : value(_value) {
        right = nullptr;
        left = nullptr;
        parent = nullptr;
    }

    void set_left(node_ptr_t node) {
        left = node;
        if (node != nullptr) {
            node->parent = get_ptr();
        }
    }

    void set_right(node_ptr_t node) {
        right = node;
        if (node != nullptr) {
            node->parent = get_ptr();
        }
    }

    node_ptr_t get_left() {
        return left;
    }

    node_ptr_t get_right() {
        return right;
    }

    V &get_value() {
        return value;
    }

    void print() {
        std::cout << value 
            << ": left: " << (left != nullptr ? left->get_value() : -1) 
            << ", right: " << (right != nullptr ? right->get_value() : -1)
            << ", parent: " << (parent != nullptr ? parent->get_value() : -1)
            << std::endl;
    }

    void _print_all() {
        print();
        if (left != nullptr) {
            left->_print_all();
        }
        if (right != nullptr) {
            right->_print_all();
        }
    }

    void print_all() {
        _print_all();
        std::cout << std::endl;
    }

    void set_value(V _value) {
        value = _value;
    }

    node_ptr_t search(V v) {
        if (v < value) {
            if (left != nullptr) {
                return left->search(v);
            }
            else {
                splay();
                return get_ptr();
            }
        }
        else if (v > value) {
            if (right != nullptr) {
                return right->search(v);
            }
            else {
                splay();
                return get_ptr();
            }
        }
        else {
            splay();
            return get_ptr();
        }
    }

    node_ptr_t insert(V v) {
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

    node_ptr_t remove(V v) {
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

    void get_next(std::stack<node_ptr_t> &traversal) {
        if (right != nullptr) {
            auto node = right;
            
            while (node->left != nullptr) {
                traversal.push(node);
                node = node->left;
            }
            traversal.push(node);
        }
    }

    node_ptr_t unpin_left_subtree() {
        auto node = left;
        if (node != nullptr) {
            node->parent = nullptr;
        }
        left = nullptr;

        return node;
    }

    node_ptr_t unpin_right_subtree() {
        auto node = right;
        if (node != nullptr) {
            node->parent = nullptr;
        }
        right = nullptr;

        return node;
    }
};

template<typename V>
class SplayTree {
    using node_ptr_t = std::shared_ptr<Node<V>>;

    node_ptr_t root;

    class Iterator {
        std::stack<node_ptr_t> traversal;

    public:
        using iterator_category = std::input_iterator_tag;
        using difference_tag = std::ptrdiff_t;
        using value_type = V;
        using pointer = V *;
        using reference = const V &;

        explicit Iterator(std::stack<node_ptr_t> traversal) : traversal(traversal) {}

        explicit Iterator(SplayTree *splay) {
            auto node = splay->root;

            while (node != nullptr) {
                traversal.push(node);
                node = node->get_left();
            }
        }

        Iterator(const Iterator &other) : Iterator(other.traversal) {}

        bool operator==(const Iterator &other) const {
            if (this->traversal.empty() == other.traversal.empty()) {
                return this->traversal.empty() == true || this->traversal.top() == other.traversal.top();
            }
            return false;
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return traversal.top()->get_value();
        }

        pointer operator->() {
            return &(traversal.top()->get_value());
        }

        Iterator &operator++() {
            node_ptr_t node = traversal.top();
            traversal.pop();
            node->get_next(traversal);
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;

            node_ptr_t node = traversal.top();
            traversal.pop();
            node->get_next(traversal);

            return temp;
        }
    };

public:
    SplayTree() {
        root = nullptr;
    }

    explicit SplayTree(node_ptr_t root) : root(root) {}

    explicit SplayTree(std::initializer_list<V> list) {
        for (V value : list) {
            insert(value);
        }
    }

    Iterator begin() {
        return Iterator(this);
    }

    Iterator end() {
        return Iterator(std::stack<node_ptr_t>());
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
        V found = root->get_value();
        return found == value;
    }

    void remove(V value) {
        if (root == nullptr) {
            return;
        }
        else {
            root = root->remove(value);
        }
    }

    SplayTree<V> remove_less(V value) {
        root = root->search(value);
        return SplayTree(root->unpin_left_subtree());
    }

    SplayTree<V> remove_greater(V value) {
        root = root->search(value);
        return SplayTree(root->unpin_right_subtree());
    }

    void print() {
        root->print_all();
    }
};