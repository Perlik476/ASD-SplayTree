#include <iostream>
#include "splay.h"

int main() {
    auto splay = SplayTree<int>({2, 1, 3, 7, 10, 5});
    for (auto x : splay) {
        std::cout << "for: " << x << "\n";
    }

    std::cout << "1 ? " << splay.contains(1) << std::endl;

    splay.remove(5);
    splay.insert(11);
    splay.insert(0);
    splay.insert(6);
    splay.insert(4);
    splay.insert(8);
    splay.insert(9);
    splay.remove(3);

    std::cout << "7 ? " << splay.contains(7) << std::endl;
    std::cout << "3 ? " << splay.contains(3) << std::endl;

    for (auto x : splay) {
        std::cout << x << "\n";
    }

    return 0;
}
