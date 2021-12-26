#include <iostream>
#include <cassert>
#include <vector>

template <typename V>
class Node {
    using parents_t = std::vector<Node *>;

    Node *right, *left;
    V value;

    void rotate_right(Node *parent, Node *grand_parent) {
        std::cout << this->get_value() << "->rotate_right(" << parent->get_value() << ", "
            << (grand_parent != nullptr ? grand_parent->get_value() : -1) << ")" << std::endl;
        assert(parent->left == this);
//        std::cout << "parent->left: " << parent->get_value() << ": " << (parent->left != nullptr ? parent->left->get_value() : -1) << std::endl;
        parent->left = this->right;
//        std::cout << "parent->left: " << parent->get_value() << ": " << (parent->left != nullptr ? parent->left->get_value() : -1) << std::endl;
        this->right = parent;
        if (grand_parent != nullptr) {
//            assert(grand_parent->left == parent || grand_parent->right == parent);
            if (grand_parent->left == parent) {
                grand_parent->left = this;
            }
            else {
                grand_parent->right = this;
            }
        }
    }

    void rotate_left(Node *parent, Node *grand_parent) {
        std::cout << this->get_value() << "->rotate_left(" << parent->get_value() << ", "
                  << (grand_parent != nullptr ? grand_parent->get_value() : -1) << ")" << std::endl;
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

    size_t static local_splay(Node *node, parents_t parents, size_t index) {
        std::cout << "local_splay(" << node->get_value() << ", " << parents.size() << ", " << index << ") " << std::endl;
        if (index == 0) { // ojciec node'a to root
            parents[index]->print();
            assert(parents[index]->get_left() == node || parents[index]->get_right() == node);
            if (parents[index]->get_left() == node) {
                node->rotate_right(parents[index], nullptr);
            }
            else {
                node->rotate_left(parents[index], nullptr);
            }
            return 1;
        }
        else {
            if (parents[index]->get_left() == node && parents[index - 1]->get_left() == parents[index]) {
                std::cout << "double_right" << std::endl;
                parents[index]->rotate_right(parents[index - 1], index - 1 > 0 ? parents[index - 2] : nullptr);
                node->rotate_right(parents[index], index - 1 > 0 ? parents[index - 2] : nullptr);
            }
            else if (parents[index]->get_right() == node && parents[index - 1]->get_right() == parents[index]) {
                std::cout << "double_left" << std::endl;
                parents[index]->rotate_left(parents[index - 1], index - 1 > 0 ? parents[index - 2] : nullptr);
                node->rotate_left(parents[index], index - 1 > 0 ? parents[index - 2] : nullptr);
            }
            else if (parents[index]->get_right() == node && parents[index - 1]->get_left() == parents[index]) {
                std::cout << "left_right" << std::endl;
                node->rotate_left(parents[index], parents[index - 1]);
                node->rotate_right(parents[index - 1], index - 1 > 0 ? parents[index - 2] : nullptr);
            }
            else if (parents[index]->get_left() == node && parents[index - 1]->get_right() == parents[index]) {
                std::cout << "right_left" << std::endl;
                node->rotate_right(parents[index], parents[index - 1]);
                node->rotate_left(parents[index - 1], index - 1 > 0 ? parents[index - 2] : nullptr);
            }
            return 2;
        }
    }

    void static splay(Node *node, parents_t &parents, size_t index) {
        std::cout << "splay" << std::endl;
        while (index + 1 != 0) {
            std::cout << std::endl;
            parents[0]->print_all();
            std::cout << std::endl;

            index -= local_splay(node, parents, index);
        }
        std::cout << "splay: end" << std::endl;
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

    void print_all() {
        print();
        if (left != nullptr) {
            left->print_all();
        }
        if (right != nullptr) {
            right->print_all();
        }
    }

    void set_value(V _value) {
        value = _value;
    }

    Node *search(V v) {
        parents_t parents;
        return search(v, parents);
    }

    Node *search(V v, parents_t &parents) {
        Node *node = nullptr;
        std::cout << "current: " << this->get_value() << std::endl;
        if (v < this->value) {
            if (this->left != nullptr) {
                parents.push_back(this);
                return this->left->search(v, parents);
            }
            else {
                splay(this, parents, parents.size() - 1);
                return nullptr;
            }
        }
        else if (v > this->value) {
            if (this->right != nullptr) {
                parents.push_back(this);
                return this->right->search(v, parents);
            }
            else {
                splay(this, parents, parents.size() - 1);
                return nullptr;
            }
        }
        else {
            splay(this, parents, parents.size() - 1);
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
//    std::cout << left.get_right()->get_value();

    return 0;
}
