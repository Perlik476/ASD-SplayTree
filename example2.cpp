#include <iostream>
#include "splay.h"

int main() {
    auto splay = SplayTree<int>({2, 1, 3, 7, 10, 5});
    for (auto x : splay) {
        std::cout << x << "\n";
    }

    std::cout << splay.contains(1) << std::endl;

    splay.remove(5);
    std::cout << "ok1\n";
    splay.insert(21);
    std::cout << "ok2\n";
    splay.insert(-3);
    std::cout << "ok3\n";
    
    std::cout << splay.contains(7) << std::endl;

    for (auto x : splay) {
        std::cout << x << "\n";
    }

    return 0;
}
