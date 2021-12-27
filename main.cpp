#include <iostream>
#include <cassert>
#include <vector>

const int debug = 0;
const int debug2 = 1;

template <typename V>
class Node;

template <typename V>
size_t get_max_subsequence(std::vector<V> seq);

template<typename V>
size_t get_size_of_subtree(Node<V> *node) {
    return node == nullptr ? 0 : node->give_size_of_subtree();
}

template<typename V>
void update_size_of_subtree(Node<V> *node) {
    if (node != nullptr)
        node->_update_size_of_subtree();
}

template <typename V>
class Node {
    using parents_t = std::vector<Node *>;

    Node *right, *left;
    V value;
    size_t size_of_subtree;

    void rotate_right(Node *parent, Node *grand_parent) {
        if (debug) {std::cout << this->get_value() << "->rotate_right(" << parent->get_value() << ", "
            << (grand_parent != nullptr ? grand_parent->get_value() : -1) << ")" << std::endl;}
        assert(parent->left == this);
        parent->left = this->right;
        parent->_update_size_of_subtree();
        this->right = parent;
        this->_update_size_of_subtree();
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

    void rotate_left(Node *parent, Node *grand_parent) {
        if (debug) {std::cout << this->get_value() << "->rotate_left(" << parent->get_value() << ", "
                  << (grand_parent != nullptr ? grand_parent->get_value() : -1) << ")" << std::endl;}
        assert(parent->right == this);
        parent->right = this->left;
        parent->_update_size_of_subtree();
        this->left = parent;
        this->_update_size_of_subtree();
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

    size_t static local_splay(Node *node, parents_t parents, size_t parents_index) {
        if (debug) std::cout << "local_splay(" << node->get_value() << ", " << parents.size() << ", " << parents_index << ") " << std::endl;
        if (parents_index == 0) { // ojciec node'a to root
            if (debug) parents[parents_index]->print();
            assert(parents[parents_index]->get_left() == node || parents[parents_index]->get_right() == node);
            if (parents[parents_index]->get_left() == node) {
                node->rotate_right(parents[parents_index], nullptr);
            }
            else {
                node->rotate_left(parents[parents_index], nullptr);
            }
            return 1;
        }
        else {
            Node *parent = parents[parents_index];
            Node *grand_parent = parents[parents_index - 1];
            Node *grand_grand_parent = parents_index - 1 > 0 ? parents[parents_index - 2] : nullptr;

            if (parents[parents_index]->get_left() == node && parents[parents_index - 1]->get_left() == parents[parents_index]) {
                if (debug) std::cout << "double_right" << std::endl;
                parent->rotate_right(grand_parent, grand_grand_parent);
                node->rotate_right(parent, grand_grand_parent);
            }
            else if (parents[parents_index]->get_right() == node && parents[parents_index - 1]->get_right() == parents[parents_index]) {
                if (debug) std::cout << "double_left" << std::endl;
                parent->rotate_left(grand_parent, grand_grand_parent);
                node->rotate_left(parent, grand_grand_parent);
            }
            else if (parents[parents_index]->get_right() == node && parents[parents_index - 1]->get_left() == parents[parents_index]) {
                if (debug) std::cout << "left_right" << std::endl;
                node->rotate_left(parent, grand_parent);
                node->rotate_right(grand_parent, grand_grand_parent);
            }
            else if (parents[parents_index]->get_left() == node && parents[parents_index - 1]->get_right() == parents[parents_index]) {
                if (debug) std::cout << "right_left" << std::endl;
                node->rotate_right(parent, grand_parent);
                node->rotate_left(grand_parent, grand_grand_parent);
            }
            return 2;
        }
    }

    void static splay(Node *node, parents_t &parents, size_t parents_index) {
        if (debug) std::cout << "splay" << std::endl;
        if (debug) {
            std::cout << std::endl;
            if (parents.size() > 0) parents[0]->print_all();
            std::cout << std::endl;
        }
        while (parents_index + 1 != 0) {
            if (debug) {
                std::cout << std::endl;
                if (parents.size() > 0) parents[0]->print_all();
                std::cout << std::endl;
            }

            parents_index -= local_splay(node, parents, parents_index);
        }
        if (debug) std::cout << "splay: end" << std::endl;
    }

    void destroy() {
        if (left != nullptr) {
            left->destroy();
        }
        if (right != nullptr) {
            right->destroy();
        }
        delete this;
    }

public:
    explicit Node(V _value) : value(_value) {
        right = nullptr;
        left = nullptr;
        size_of_subtree = 1;
    }

    ~Node() {
        destroy();
    }

    void _update_size_of_subtree() {
        size_of_subtree = 1 + get_size_of_subtree(right) + get_size_of_subtree(left);
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
            << (right != nullptr ? right->get_value() : -1) << ", subtree: " << size_of_subtree << std::endl;
    }

    void print_all() {
        print_all2();
        std::cout << std::endl;
    }

    void print_all2() {
        print();
        if (left != nullptr) {
            left->print_all2();
        }
        if (right != nullptr) {
            right->print_all2();
        }
    }

    void print_sequence() {
        if (left != nullptr)
            left->print_sequence();
        std::cout << this->value << ", ";
        if (right != nullptr)
            right->print_sequence();
    }

    void set_value(V _value) {
        value = _value;
    }

    size_t give_size_of_subtree() {
        return size_of_subtree;
    }

    Node *search(size_t position) {
        parents_t parents;
        parents.reserve(30);
        return search(position, parents);
    }

    Node *search(size_t position, parents_t &parents) {
        assert(position >= 1);
        if (debug) std::cout << "current: " << this->get_value() << std::endl;
        if (position < get_size_of_subtree(this->left) + 1) {
            if (this->left != nullptr) {
                parents.push_back(this);
                return this->left->search(position, parents);
            }
            else {
                splay(this, parents, parents.size() - 1);
                return nullptr;
            }
        }
        else if (position > get_size_of_subtree(this->left) + 1) {
            if (this->right != nullptr) {
                parents.push_back(this);
                return this->right->search(position - get_size_of_subtree(this->left) - 1, parents);
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

    Node *insert(V v, size_t position) {
        parents_t parents;
        parents.reserve(20);
        return insert(v, position, parents);
    }

    Node *insert(V v, size_t position, parents_t &parents) {
        assert(position >= 1);
        Node *node = nullptr;
        if (debug) std::cout << "current: " << this->get_value() << ", " << get_size_of_subtree(this->left) + 1 << std::endl;
        if (position < get_size_of_subtree(this->left) + 1) {
            this->size_of_subtree++; // zakładamy, że elementu nie ma w drzewie
            if (this->left != nullptr) {
                parents.push_back(this);
                return this->left->insert(v, position, parents);
            }
            else {
                parents.push_back(this);
                node = new Node(v);
                this->left = node;
                splay(node, parents, parents.size() - 1);
                return node;
            }
        }
        else if (position > get_size_of_subtree(this->left) + 1) {
            this->size_of_subtree++; // zakładamy, że elementu nie ma w drzewie
            if (this->right != nullptr) {
                parents.push_back(this);
                return this->right->insert(v, position - get_size_of_subtree(this->left) - 1, parents);
            }
            else {
                parents.push_back(this);
                node = new Node(v);
                this->right = node;
                splay(node, parents, parents.size() - 1);
                return node;
            }
        }
        else {
            node = new Node(v);
            node->right = this;
            node->left = this->left;
            this->left = nullptr;
            this->_update_size_of_subtree();
            node->_update_size_of_subtree();
            if (!parents.empty()) {
                if (parents[parents.size() - 1]->get_left() == this)
                    parents[parents.size() - 1]->left = node;
                else
                    parents[parents.size() - 1]->right = node;
            }
            node->print_all();
            splay(node, parents, parents.size() - 1);
            return node;
        }
    }

    Node *move(size_t pos_begin, size_t pos_end, size_t pos_after) {
        if (pos_end - pos_begin + 1 == this->size_of_subtree) {
            return this;
        }

        Node *temp = this->search(pos_end);

        Node *left_side, *right_side;

        right_side = temp->right;
        temp->right = nullptr;
        update_size_of_subtree(right_side);
        update_size_of_subtree(temp);

        temp = temp->search(pos_begin);

        left_side = temp->left;
        temp->left = nullptr;
        update_size_of_subtree(left_side);
        update_size_of_subtree(temp);

        if (debug2) {
            std::cout << "LEFT: ";
            if (left_side != nullptr) left_side->print_sequence();
            std::cout << std::endl << "TEMP: ";
            temp->print_sequence();
            std::cout << std::endl << "RIGHT: ";
            if (right_side != nullptr) right_side->print_sequence();
            std::cout << std::endl;
        }

        Node *new_node;
        if (left_side != nullptr) {
            new_node = left_side;
            new_node = new_node->search(pos_begin - 1);
            new_node->right = right_side;
            update_size_of_subtree(new_node);
        }
        else { // (right_side != nullptr)
            new_node = right_side;
        }

        if (debug2) {
            std::cout << "NEW: ";
            if (new_node != nullptr) new_node->print_sequence();
            std::cout << std::endl << "TEMP: ";
            temp->print_sequence();
            std::cout << std::endl;
        }

        if (pos_after == new_node->size_of_subtree + 1) {
            new_node = new_node->search(pos_after - 1);
            new_node->right = temp;
            update_size_of_subtree(new_node);
            return new_node;
        }

        new_node = new_node->search(pos_after);
        left_side = new_node->left;
        new_node->left = temp;
        update_size_of_subtree(new_node);

        if (debug2) {
            std::cout << "NEW: ";
            if (new_node != nullptr) new_node->print_sequence();
            std::cout << std::endl << "LEFT: ";
            if (left_side != nullptr) left_side->print_sequence();
            std::cout << std::endl;
        }

        new_node = new_node->search(1);
        new_node->left = left_side;
        update_size_of_subtree(new_node);

        if (debug2) {
            std::cout << "NEW: ";
            new_node->print_sequence();
            std::cout << std::endl;
        }

        return new_node;
    }

    void rotate_recursive() {
        std::swap(left, right);
        if (left != nullptr)
            left->rotate_recursive();
        if (right != nullptr)
            right->rotate_recursive();
    }

    Node *rotate(size_t pos_begin, size_t pos_end) {
        if (pos_end - pos_begin + 1 == this->size_of_subtree) {
            this->rotate_recursive();
            return this;
        }
        size_t pos_after = pos_begin;

        Node *temp = this->search(pos_end);

        Node *left_side, *right_side;

        right_side = temp->right;
        temp->right = nullptr;
        update_size_of_subtree(right_side);
        update_size_of_subtree(temp);

        temp = temp->search(pos_begin);

        left_side = temp->left;
        temp->left = nullptr;
        update_size_of_subtree(left_side);
        update_size_of_subtree(temp);

        temp->rotate_recursive();

        if (debug2) {
            std::cout << "LEFT: ";
            if (left_side != nullptr) left_side->print_sequence();
            std::cout << std::endl << "TEMP: ";
            temp->print_sequence();
            std::cout << std::endl << "RIGHT: ";
            if (right_side != nullptr) right_side->print_sequence();
            std::cout << std::endl;
        }

        Node *new_node;
        if (left_side != nullptr) {
            new_node = left_side;
            new_node = new_node->search(pos_begin - 1);
            new_node->right = right_side;
            update_size_of_subtree(new_node);
        }
        else { // (right_side != nullptr)
            new_node = right_side;
        }

        if (debug2) {
            std::cout << "NEW: ";
            if (new_node != nullptr) new_node->print_sequence();
            std::cout << std::endl << "TEMP: ";
            temp->print_sequence();
            std::cout << std::endl;
        }

        if (pos_after == new_node->size_of_subtree + 1) {
            new_node = new_node->search(pos_after - 1);
            new_node->right = temp;
            update_size_of_subtree(new_node);
            return new_node;
        }

        new_node = new_node->search(pos_after);
        left_side = new_node->left;
        new_node->left = temp;
        update_size_of_subtree(new_node);

        if (debug2) {
            std::cout << "NEW: ";
            if (new_node != nullptr) new_node->print_sequence();
            std::cout << std::endl << "LEFT: ";
            if (left_side != nullptr) left_side->print_sequence();
            std::cout << std::endl;
        }

        new_node = new_node->search(1);
        new_node->left = left_side;
        update_size_of_subtree(new_node);

        if (debug2) {
            std::cout << "NEW: ";
            new_node->print_sequence();
            std::cout << std::endl;
        }

        return new_node;
    }

    std::vector<V> get_sequence() {
        std::vector<V> seq;
        get_sequence(seq);
        return seq;
    }

    void get_sequence(std::vector<V> &seq) {
        if (left != nullptr)
            left->get_sequence(seq);
        seq.push_back(value);
        if (right != nullptr)
            right->get_sequence(seq);
    }

    size_t max_subsequence(size_t pos_begin, size_t pos_end) {
        std::vector<V> seq = get_sequence();
        std::cout << "ok" << std::endl;
        for (auto &v : seq) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
        return get_max_subsequence(seq);
    }
};

template <typename V>
size_t get_max_subsequence(std::vector<V> seq) {
    if (seq.empty())
        return 0;

    size_t how_many_max = 1;
    size_t how_many_current = 1;
    for (size_t i = 1; i < seq.size(); i++) {
        if (seq[i] == seq[i - 1]) {
            how_many_current++;
            if (how_many_current > how_many_max) {
                how_many_max = how_many_current;
            }
        }
        else {
            how_many_current = 1;
        }
    }
    return how_many_max;
}



int main() {
    auto *root = new Node(5);
    root = root->insert(7, 1);
    root = root->insert(2, 2);
    root = root->insert(1, 3);
    root = root->insert(4, 4);
    root = root->insert(0, 5);

    root->print_all();

    std::cout << "SEARCH XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD:" << std::endl;

    Node<int> *result = root->search(5);
    result->print_all();
    result->print_sequence();

    std::cout << "INSERT XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD:" << std::endl;

    result = result->insert(10, 3);
    result->print_all();
    result->print_sequence();

    std::cout << "MOVE XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << std::endl;

    result = result->move(1, 7, 1);
    result->print_sequence();

    std::cout << "ROTATE XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << std::endl;

    result = result->rotate(1, 7);
    result->print_sequence();

    result = result->insert(0, 3);
    result = result->insert(0, 1);
    result = result->insert(2, 6);

    result->print_sequence();

    std::cout << "MAX SUBSEQUENCE XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << std::endl;

    std::cout << result->max_subsequence(2, 6) << std::endl;

    std::cout << std::endl << "DONE" << std::endl;

//    result = result->search(4);
//    result->print_all();
//
//    result = result->insert(3);
//    result->print_all();
//
//    result = result->insert(10);
//    result = result->insert(21);
//    result = result->insert(37);
//    result = result->search(7);
//    result->print_all();
//
//    int M = 100000;
//    srand(time(0));
//    for (int i = 1; i < M; i++) {
//        result = result->insert(rand()%M);
//    }
//    result->print_all();
//    result = result->search(40);
//    result->print_all();
//    std::cout << left.get_right()->get_value();

    return 0;
}
