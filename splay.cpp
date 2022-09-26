#include <iostream>
#include <cassert>
#include <vector>

template <typename V>
class Node {
    using parents_t = std::vector<Node *>;

    Node *right, *left;
    V value;

    void rotate_right(Node *parent, Node *grand_parent) {
        assert(parent->left == this);

        parent->left = this->right;
        this->right = parent;

        if (grand_parent != nullptr) {
            if (grand_parent->left == parent) {
                grand_parent->left = this;
            }
            else {
                grand_parent->right = this;
            }
        }
    }

    void rotate_left(Node *parent, Node *grand_parent) {
        assert(parent->right == this);

        parent->right = this->left;
        this->left = parent;

        if (grand_parent != nullptr) {
            assert(grand_parent->left == parent || grand_parent->right == parent);
            if (grand_parent->left == parent) {
                grand_parent->left = this;
            }
            else {
                grand_parent->right = this;
            }
        }
    }

    size_t local_splay(parents_t parents, size_t parent_index) {
        if (parent_index == 0) { // node's parent is root
            auto parent = parents[parent_index];

            assert(parent->get_left() == this || parent->get_right() == this);

            if (parent->get_left() == this) {
                rotate_right(parent, nullptr);
            }
            else {
                rotate_left(parent, nullptr);
            }
            return 1;
        }
        else {
            auto parent = parents[parent_index];
            auto grandparent = parents[parent_index - 1];
            auto grandgrandparent = parent_index - 1 > 0 ? parents[parent_index - 2] : nullptr;

            if (parent->get_left() == this && grandparent->get_left() == parent) {
                parent->rotate_right(grandparent, parent_index - 1 > 0 ? grandgrandparent : nullptr);
                rotate_right(parent, grandgrandparent);
            }
            else if (parent->get_right() == this && grandparent->get_right() == parent) {
                parent->rotate_left(grandparent, parent_index - 1 > 0 ? grandgrandparent : nullptr);
                rotate_left(parent, grandgrandparent);
            }
            else if (parent->get_right() == this && grandparent->get_left() == parent) {
                rotate_left(parent, grandparent);
                rotate_right(grandparent, grandgrandparent);
            }
            else if (parent->get_left() == this && grandparent->get_right() == parent) {
                rotate_right(parent, grandparent);
                rotate_left(grandparent, grandgrandparent);
            }
            return 2;
        }
    }

    void splay(parents_t &parents) {
        size_t index = parents.size() - 1;
        while (index + 1 != 0) {
            index -= local_splay(parents, index);
        }
    }

public:
    explicit Node(V _value) : value(_value) {
        right = nullptr;
        left = nullptr;
    }

    void set_left(Node *node) {
        left = node;
    }

    void set_right(Node *node) {
        right = node;
    }

    Node *get_left() {
        return left;
    }

    Node *get_right() {
        return right;
    }

    V get_value() {
        return value;
    }

    void print() {
        std::cout << value << ": left: " << (left != nullptr ? left->get_value() : -1) << ", right: "
            << (right != nullptr ? right->get_value() : -1) << std::endl;
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

    Node *search(V v) {
        parents_t parents;
        parents.reserve(30);
        return search(v, parents);
    }

    Node *search(V v, parents_t &parents) {
        if (v < this->value) {
            if (this->left != nullptr) {
                parents.push_back(this);
                return this->left->search(v, parents);
            }
            else {
                splay(parents);
                return nullptr;
            }
        }
        else if (v > this->value) {
            if (this->right != nullptr) {
                parents.push_back(this);
                return this->right->search(v, parents);
            }
            else {
                splay(parents);
                return nullptr;
            }
        }
        else {
            splay(parents);
            return this;
        }
    }

    Node *insert(V v) {
        parents_t parents;
        parents.reserve(30);
        return insert(v, parents);
    }

    Node *insert(V v, parents_t &parents) {
        Node *node = nullptr;
        if (v < this->value) {
            if (this->left != nullptr) {
                parents.push_back(this);
                return this->left->insert(v, parents);
            }
            else {
                parents.push_back(this);
                node = new Node(v);
                this->left = node;
                node->splay(parents);
                return node;
            }
        }
        else if (v > this->value) {
            if (this->right != nullptr) {
                parents.push_back(this);
                return this->right->insert(v, parents);
            }
            else {
                parents.push_back(this);
                node = new Node(v);
                this->right = node;
                node->splay(parents);
                return node;
            }
        }
        else {
            splay(parents);
            return this;
        }
    }
};


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
