#include <iostream>
#include <vector>
#include <memory>
#include <stack>

template<typename V>
class SplayTree {

    class Node : public std::enable_shared_from_this<Node> {
        using node_ptr_t = std::shared_ptr<Node>;
        using node_weakptr_t = std::weak_ptr<Node>;

        node_ptr_t right, left;
        node_weakptr_t parent;
        V value;
        size_t subtree_size = 1;

        node_ptr_t get_ptr() {
            return this->shared_from_this();
        }

        node_ptr_t get_parent() {
            return parent.lock();
        }

        void update_subtree_size() {
            subtree_size = 1 + get_subtree_size(right) + get_subtree_size(left);
        }

        void rotate_right() {
            auto grandparent = get_parent()->get_parent();
            auto current_parent = get_parent();
            auto this_ptr = get_ptr();

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
                grandparent->update_subtree_size();
            }
        }

        void rotate_left() {
            auto grandparent = get_parent()->get_parent();
            auto current_parent = get_parent();
            auto this_ptr = get_ptr();

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
                grandparent->update_subtree_size();
            }
        }

        void local_splay() {
            auto this_ptr = get_ptr();
            auto grandparent = get_parent()->get_parent();

            if (grandparent == nullptr) {
                if (get_parent()->get_left() == get_ptr()) {
                    rotate_right();
                }
                else {
                    rotate_left();
                }
            }
            else {
                auto grandgrandparent = grandparent->get_parent();
                auto _parent = get_parent();

                if (get_parent()->get_left() == get_ptr() && grandparent->get_left() == get_parent()) {
                    get_parent()->rotate_right();
                    rotate_right();
                }
                else if (get_parent()->get_right() == get_ptr() && grandparent->get_right() == get_parent()) {
                    get_parent()->rotate_left();
                    rotate_left();
                }
                else if (get_parent()->get_right() == get_ptr() && grandparent->get_left() == get_parent()) {
                    rotate_left();
                    rotate_right();
                }
                else if (get_parent()->get_left() == get_ptr() && grandparent->get_right() == get_parent()) {
                    rotate_right();
                    rotate_left();
                }
            }
        }

        void splay() {
            while (get_parent() != nullptr) {
                local_splay();
            }
        }

    public:
        explicit Node(V _value) : value(_value) {
            right = nullptr;
            left = nullptr;
            parent = node_weakptr_t();
        }

        void set_left(node_ptr_t node) {
            left = node;
            if (node != nullptr) {
                node->parent = get_ptr();
            }
            update_subtree_size();
        }

        void set_right(node_ptr_t node) {
            right = node;
            if (node != nullptr) {
                node->parent = get_ptr();
            }
            update_subtree_size();
        }

        node_ptr_t get_left() {
            return left;
        }

        node_ptr_t get_right() {
            return right;
        }

        static size_t get_subtree_size(node_ptr_t node) {
            return node ? node->subtree_size : 0;
        }

        size_t get_subtree_size() {
            return subtree_size;
        }

        V &get_value() {
            return value;
        }

        void print() {
            std::cout << value
                      << ": left: " << (left != nullptr ? left->get_value() : -1)
                      << ", right: " << (right != nullptr ? right->get_value() : -1)
                      << ", parent: " << (get_parent() != nullptr ? get_parent()->get_value() : -1)
                      << ", size=" << get_subtree_size()
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

        node_ptr_t search(V v, SplayTree<V> *splay_tree) {
            auto this_ptr = get_ptr();

            if (v < value) {
                if (left != nullptr) {
                    return left->search(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->search(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else {
                splay();
                splay_tree->root = get_ptr();
                return get_ptr();
            }
        }

        node_ptr_t insert(V v, SplayTree<V> *splay_tree) {
            auto this_ptr = get_ptr();

            if (v < value) {
                if (left != nullptr) {
                    return left->insert(v, splay_tree);
                }
                else {
                    auto node = std::make_shared<Node>(v);

                    set_left(node);
                    node->splay();
                    splay_tree->root = node;

                    return node;
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->insert(v, splay_tree);
                }
                else {
                    auto node = std::make_shared<Node>(v);

                    set_right(node);
                    node->splay();
                    splay_tree->root = node;

                    return node;
                }
            }
            else {
                splay();
                splay_tree->root = get_ptr();
                return get_ptr();
            }
        }

        node_ptr_t remove(V v, SplayTree<V> *splay_tree) {
            auto this_ptr = get_ptr();

            if (v < value) {
                if (left != nullptr) {
                    return left->remove(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->remove(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else {
                node_ptr_t new_root = get_parent();

                if (left == nullptr && right == nullptr) {
                    if (get_parent() != nullptr) {
                        if (get_parent()->left == get_ptr()) {
                            get_parent()->set_left(nullptr);
                        }
                        else {
                            get_parent()->set_right(nullptr);
                        }
                    }
                    else {
                        new_root = nullptr;
                    }
                }
                else if ((left != nullptr && right == nullptr) || (right != nullptr && left == nullptr)) {
                    node_ptr_t child = left == nullptr ? right : left;

                    if (get_parent() == nullptr) {
                        new_root = child;
                        child->remove_parent();
                    }
                    else if (get_parent()->left == get_ptr()) {
                        get_parent()->set_left(child);
                    }
                    else {
                        get_parent()->set_right(child);
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
                        node->get_parent()->set_right(node->left);
                    }

                    new_root = get_ptr();
                }


                if (new_root != nullptr) {
                    new_root->splay();
                }
                splay_tree->root = new_root;

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

        void get_previous(std::stack<node_ptr_t> &traversal) {
            if (left != nullptr) {
                auto node = left;

                while (node->right != nullptr) {
                    traversal.push(node);
                    node = node->right;
                }
                traversal.push(node);
            }
        }

        node_ptr_t unpin_left_subtree() {
            auto node = left;
            if (node != nullptr) {
                node->parent = node_weakptr_t();
            }
            set_left(nullptr);

            return node;
        }

        node_ptr_t unpin_right_subtree() {
            auto node = right;
            if (node != nullptr) {
                node->parent = node_weakptr_t();
            }
            set_right(nullptr);

            return node;
        }

        void remove_parent() {
            parent = node_weakptr_t();
        }
    };

    using node_ptr_t = std::shared_ptr<Node>;

    node_ptr_t root;

    template<bool increasing_direction>
    class Iterator {
        std::stack<node_ptr_t> traversal;

        void next() {
            node_ptr_t node = traversal.top();
            traversal.pop();
            if (increasing_direction) {
                node->get_next(traversal);
            }
            else {
                node->get_previous(traversal);
            }
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using difference_tag = std::ptrdiff_t;
        using value_type = V;
        using pointer = V *;
        using reference = const V &;

        explicit Iterator(std::stack<node_ptr_t> traversal) : traversal(traversal) {}

        explicit Iterator(const SplayTree *splay) {
            auto node = splay->root;

            if (increasing_direction) {
                while (node != nullptr) {
                    traversal.push(node);
                    node = node->get_left();
                }
            }
            else {
                while (node != nullptr) {
                    traversal.push(node);
                    node = node->get_right();
                }
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
            next();
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            next();
            return temp;
        }
    };

public:
    SplayTree() {
        root = nullptr;
    }

    explicit SplayTree(node_ptr_t root) : root(root) {}

    SplayTree(std::initializer_list<V> list) {
        for (V value : list) {
            insert(value);
        }
    }

    Iterator<true> begin() const {
        return Iterator<true>(this);
    }

    Iterator<true> end() const {
        return Iterator<true>(std::stack<node_ptr_t>());
    }

    Iterator<false> rbegin() const {
        return Iterator<false>(this);
    }

    Iterator<false> rend() const {
        return Iterator<false>(std::stack<node_ptr_t>());
    }

    void insert(V value) {
        if (root == nullptr) {
            root = std::make_shared<Node>(value);
        }
        else {
            root->insert(value, this);
        }
    }

    bool contains(V value) const {
        if (root == nullptr) {
            return false;
        }
        root->search(value, this);
        V found = root->get_value();
        return found == value;
    }

    size_t size() const {
        return Node::get_subtree_size(root);
    }

    void erase(V value) {
        if (root == nullptr) {
            return;
        }
        else {
            root->remove(value, this);
        }
    }

    SplayTree<V> erase_less(V value) {
        root->search(value, this);
        auto result = SplayTree(root->unpin_left_subtree());

        auto v = root->get_value();
        if (v < value) {
            erase(v);
            result.insert(v);
        }

        return result;
    }

    SplayTree<V> erase_greater(V value) {
        root->search(value, this);
        auto result = SplayTree(root->unpin_right_subtree());

        auto v = root->get_value();
        if (v > value) {
            erase(v);
            result.insert(v);
        }

        return result;
    }

    void clear() {
        root = nullptr;
    }

    bool empty() const {
        return root == nullptr;
    }

    void print() {
        root->print_all();
    }
};