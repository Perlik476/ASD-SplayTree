#include <iostream>
#include "../splay.h"
#include <set>
#include <utility>
#include <functional>
#include "assert.h"
#include <any>
#include <limits>

template <typename T>
void print(const std::set<T> &set) {
    std::cout << "set size = " << set.size() << std::endl;
    for (auto x : set) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T>
void print(const SplayTree<T> &splay) {
    std::cout << "splay size = " << splay.size() << std::endl;
    for (auto x : splay) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T>
bool equals(const std::set<T> &set, const SplayTree<T> &splay) {
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

void test_correctness_basic() {
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
        splay.erase(x);
        assert(set.contains(x) ? !equals(set, splay) : equals(set, splay));

        set.erase(x);
        assert(equals(set, splay));
    }
}

void test_split_basic() {
    std::vector<int> values = {2, 1, 3, 7, 4, 2, 0, 6, 9, 4, 7, 6};

    SplayTree<int> splay;

    for (auto x : values) {
        splay.insert(x);
    }

    int value = 6;
    std::set<int> set_less, set_rest;
    for (auto x : values) {
        if (x < value) {
            set_less.insert(x);
        }
        else {
            set_rest.insert(x);
        }
    }

    auto splay_less = splay.erase_less(value);

    assert(equals(set_less, splay_less));
    assert(equals(set_rest, splay));

    value = 0;
    std::set<int> set_greater;
    set_rest.clear();
    for (auto x : values) {
        if (x > value) {
            set_greater.insert(x);
        }
        else {
            set_rest.insert(x);
        }
    }

    splay.clear();
    for (auto x : values) {
        splay.insert(x);
    }
    auto splay_greater = splay.erase_greater(value);

    assert(equals(set_rest, splay));
    assert(equals(set_greater, splay_greater));

    values = {2, 1, 3, 7};
    splay.clear();
    for (auto x : values) {
        splay.insert(x);
    }
    splay_greater = splay.erase_greater(0);

    assert(splay_greater.size() == 4);
    assert(splay.empty());
}

void test_merge_basic() {
    SplayTree<int> splay1 = {2, 1, 3, 7};
    SplayTree<int> splay2 = {4, 7, 6, 2};

    splay1.merge(splay2);

    assert(equals({1, 2, 3, 4, 6, 7}, splay1));
    assert(equals({2, 7}, splay2));
}

void test_find_basic() {
    SplayTree<int> splay = {6, 9, 4, 2, 1};

    auto it = splay.find(6);
    assert(it != splay.end());
    assert(*it == 6);

    it = splay.find(3);
    assert(it == splay.end());

    it = splay.find(2);
    assert(it != splay.end());
    assert(*it == 2);

    for (int i = 0; i < 4; i++) {
        assert(it != splay.end());
        it++;
    }
    assert(it == splay.end());

    it = splay.find(2);

    auto const_it2= std::as_const(splay).find(10);
    assert(const_it2 == splay.end());

    auto const_it = std::as_const(splay).find(9);
    assert(const_it != splay.end());
    assert(*const_it == 9);

    for (int i = 0; i < 4; i++) {
        assert(it != splay.end());
        it++;
    }
    assert(it == splay.end());
}

void test_swap_basic() {
    SplayTree<int> splay1 = {2, 1, 3, 7};
    SplayTree<int> splay2 = {4, 7, 6};

    splay1.swap(splay2);

    assert(equals({4, 7, 6}, splay1));
    assert(equals({2, 1, 3, 7}, splay2));
}

void test_contains_basic() {
    SplayTree<int> splay = {1, 4, 9, 12, 5, 17};

    assert(splay.contains(1));
    assert(splay.contains(9));
    assert(!splay.contains(6));

    const SplayTree<int> splay2 = splay;

    assert(splay2.contains(1));
    assert(splay2.contains(9));
    assert(!splay2.contains(6));
}

void test_lower_upper_bound_basic() {
    SplayTree<int> splay = {2, 1, 3, 7, 4, 6, 9};

    auto it = splay.lower_bound(4);
    assert(*it == 4);

    it = splay.upper_bound(4);
    assert(*it == 6);

    it = splay.lower_bound(8);
    assert(*it == 9);

    it = splay.upper_bound(8);
    assert(*it == 9);

    it = splay.lower_bound(2);
    assert(*it == 2);

    it = splay.upper_bound(2);
    assert(*it == 3);

    it = splay.lower_bound(12);
    assert(it == splay.end());

    it = splay.upper_bound(12);
    assert(it == splay.end());

    it = splay.lower_bound(9);
    assert(*it == 9);

    it = splay.upper_bound(9);
    assert(it == splay.end());

    it = splay.lower_bound(0);
    assert(*it == 1);

    it = splay.upper_bound(0);
    assert(*it == 1);
}

void test_function_basic() {
    SplayTree<int>::Function even_counter = { [](int v, int left, int right) { return (v % 2 == 0) + left + right; }, 0 };

    SplayTree<int> splay({ 2, 1, 3, 7, 6, 9, 4, 2 }, even_counter);
    assert(splay.get_function_value() == 3);

    using splay_pairs_t = SplayTree<int, std::less<>, std::pair<int, int>>;
    splay_pairs_t::Function min_max = {
            [](int v, std::pair<int, int> left, std::pair<int, int> right) -> std::pair<int, int> {
                return { std::max(v, std::max(left.first, right.first)),
                         std::min(v, std::min(left.second, right.second)) };
            },
            { std::numeric_limits<int>::min(), std::numeric_limits<int>::max() } };

    splay_pairs_t splay_pairs = { { 2, 1, 3, 7, 6, 9, 4, 2 }, min_max };
    assert(splay_pairs.get_function_value().first == 9 && splay_pairs.get_function_value().second == 1);


    using splay_str_t = SplayTree<int, std::less<>, std::string>;
    splay_str_t::Function to_str = {
            [](int v, const std::string &left, const std::string &right) {
                std::stringstream ss;
                ss << left << v << right;
                return ss.str();
            }, "" };

    splay_str_t splay_to_str = { { 2, 1, 3, 7, 6, 9, 4, 2 }, to_str };
    assert(splay_to_str.get_function_value() == "1234679");

    auto splay_to_str_less = splay_to_str.erase_less(4);
    assert(splay_to_str_less.get_function_value() == "123");
    assert(splay_to_str.get_function_value() == "4679");

    splay_to_str.insert(1);
    assert(splay_to_str.get_function_value() == "14679");

    splay_to_str_less.insert(8);
    assert(splay_to_str_less.get_function_value() == "1238");
}

void test_comparer_basic() {
    SplayTree<int, std::greater<>> splay({2, 1, 3, 7});
    auto it = splay.begin();
    assert(*it++ == 7);
    assert(*it++ == 3);
    assert(*it++ == 2);
    assert(*it++ == 1);
    assert(it == splay.end());

    it = splay.find(4);
    assert(it == splay.end());

    it = splay.upper_bound(3);
    assert(*it == 2);

    it = splay.lower_bound(3);
    assert(*it == 3);
}

void test_insert_iterator() {
    SplayTree<int> splay = {2, 1, 3, 7, 4};

    auto it = splay.insert(6);
    assert(*it++ == 6);
    assert(*it++ == 7);
    assert(it == splay.end());

    it = splay.insert(2);
    assert(*it == 2);
}

void test_erase_iterator() {
    SplayTree<int> splay = {2, 1, 3, 7, 4};

    auto it = splay.begin();
    it++; it++; it++;
    it = splay.erase(it);
    assert(*it == 7);

    it = splay.erase(it);
    assert(it == splay.end());

    it = splay.erase(it);
    assert(it == splay.end());
}

class Test {
    const std::function<void()> test;
    std::string name;

public:
    Test(const std::function<void()> &test, std::string &&name) : test(test), name(std::move(name)) {}

    void operator ()() {
        std::cout << "'" << name << "' test running: ";
        try {
            test();
            std::cout << "passed!" << std::endl;
        } catch (const std::exception &e) {
            std::cout << "FAILED: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "FAILED" << std::endl;
        }
    }
};

int main() {
    auto tests = {
            Test(test_correctness_basic, "correctness basic"),
            Test(test_split_basic, "split basic"),
            Test(test_merge_basic, "merge basic"),
            Test(test_find_basic, "find basic"),
            Test(test_swap_basic, "swap basic"),
            Test(test_contains_basic, "contains basic"),
            Test(test_lower_upper_bound_basic, "lower/upper bound basic"),
            Test(test_function_basic, "function basic"),
            Test(test_comparer_basic, "comparer basic"),
            Test(test_insert_iterator, "insert iterator")
    };

    for (auto test : tests) {
        test();
    }

    return 0;
}