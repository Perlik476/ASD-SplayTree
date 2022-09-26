#include <iostream>
#include <cassert>
#include <vector>

template <typename V>
class Node {
    Node *right, *left, *parent;
    V value;

    void rotate_right();
    void rotate_left();

    void local_splay();
    void splay();

    void _print_all();

public:
    explicit Node(V _value);

    void set_left(Node *node);
    void set_right(Node *node);

    Node *get_left();
    Node *get_right();

    V get_value();
    void set_value(V _value);

    void print();
    void print_all();

    Node *search(V v);

    Node *insert(V v);
};