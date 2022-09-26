#include <iostream>
#include <cassert>
#include <vector>

template <typename V>
class Node {
    using parents_t = std::vector<Node *>;

    Node *right, *left;
    V value;

    void rotate_right(Node *parent, Node *grand_parent);

    void rotate_left(Node *parent, Node *grand_parent);

    size_t static local_splay(Node *node, parents_t parents, size_t index);

    void static splay(Node *node, parents_t &parents, size_t index);

public:
    explicit Node(V _value);

    void set_left(Node *node);

    void set_right(Node *node);

    Node *get_left();

    Node *get_right();

    V get_value();

    void print();

    void print_all();

    void set_value(V _value);

    Node *search(V v);

    Node *search(V v, parents_t &parents);

    Node *insert(V v);

    Node *insert(V v, parents_t &parents);
};