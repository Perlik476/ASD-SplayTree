#include <iostream>
#include "../splay.h"
#include <set>
#include <assert.h>

template <typename T>
void print(const std::set<T> &set) {
    std::cout << "set size = " << set.size() << std::endl;
    for (auto x : set) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T>
void print(SplayTree<T> &splay) {
    std::cout << "splay size = " << splay.size() << std::endl;
    for (auto x : splay) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T>
bool equals(const std::set<T> &set, SplayTree<T> &splay) {
    if (set.size() != splay.size()) {
        return false;
    }

    auto set_it = set.begin();
    auto splay_it = splay.begin();

    for (; set_it != set.end() && splay_it != splay.end(); set_it++, splay_it++) {
        if (*set_it != *splay_it) {
            return false;
        }
    }

    if (set_it != set.end() || splay_it != splay.end()) {
        return false;
    }
    return true;
}

void test_correctness() {
    std::set<int> set;
    SplayTree<int> splay;
    assert(equals(set, splay));

    set.insert(1);
    assert(!equals(set, splay));

    splay.insert(2);
    assert(!equals(set, splay));

    set.insert(2);
    assert(!equals(set, splay));

    splay.insert(1);
    assert(equals(set, splay));

    std::vector<int> values = {3, 4, 7, 1, 10, 5, 5, 12, 6, 2, 9};
    for (int x : values) {
        splay.insert(x);
        assert(set.contains(x) ? equals(set, splay) : !equals(set, splay));

        set.insert(x);
        assert(equals(set, splay));
    }

    values = {2, 1, 3, 7, 11, 12, 2, 2, 4};
    for (int x : values) {
        splay.remove(x);

        assert(set.contains(x) ? !equals(set, splay) : equals(set, splay));

        set.erase(x);

        assert(equals(set, splay));
    }
}

int main() {
    test_correctness();

    return 0;
}