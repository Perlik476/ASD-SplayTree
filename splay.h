#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <stack>
#include <map>
#include <functional>
#include <variant>
#include <optional>

template<class V, class... FunctionTypes>
class SplayTree {
public:
    template <class T>
    class Function {
        using function_t = std::function<const T(const V &, const T &, const T &)>;

        std::string name;
        function_t function;
        T default_value;
        T default_value_null;

    public:
        constexpr Function(std::string name, function_t &&function, T default_value, T default_value_null)
                : name(std::move(name)), function(std::move(function)), default_value(default_value),
                  default_value_null(default_value_null) {}

        Function() = default;

        ~Function() = default;

        Function &operator =(const Function &other) {
            name = other.name;
            function = other.function;
            default_value = other.default_value;
            default_value_null = other.default_value_null;
            return *this;
        }

        const T operator ()(const V &value, const T &left, const T &right) const {
            return function(value, left, right);
        }

        const function_t &get_function() const {
            return function;
        }

        const std::string &get_name() const {
            return name;
        }

        const T &get_default() const {
            return default_value;
        }

        const T &get_default_null() const {
            return default_value_null;
        }
    };

private:
    using polymorphic_function_types_t = std::variant<int, FunctionTypes...>;
    using polymorphic_function_t = std::variant<Function<int>, Function<FunctionTypes>...>;

    class Node : public std::enable_shared_from_this<Node> {
        using node_ptr_t = std::shared_ptr<Node>;
        using node_weakptr_t = std::weak_ptr<Node>;

        node_ptr_t right, left;
        node_weakptr_t parent;
        V value;
        size_t subtree_size = 1;

        std::map<std::string, polymorphic_function_types_t> function_values;

        node_ptr_t get_ptr() {
            return this->shared_from_this();
        }

        std::shared_ptr<const Node> get_ptr() const {
            return this->shared_from_this();
        }

        node_ptr_t get_parent() {
            return parent.lock();
        }

        void update(const SplayTree &splay) {
            subtree_size = 1 + get_subtree_size(right) + get_subtree_size(left);

            for (auto &[name, function] : splay.functions) {
                std::visit([&](auto &&func) {
                    function_values[name] = func(value, get_function_value(left, func), get_function_value(right, func));
                }, function);
            }
        }

        void rotate_right(const SplayTree &splay_tree) {
            auto grandparent = get_parent()->get_parent();
            auto current_parent = get_parent();
            auto this_ptr = get_ptr();

            current_parent->set_left(right, splay_tree);
            set_right(current_parent, splay_tree);

            parent = grandparent;

            if (grandparent != nullptr) {
                if (grandparent->left == current_parent) {
                    grandparent->left = get_ptr();
                }
                else {
                    grandparent->right = get_ptr();
                }
                grandparent->update(splay_tree);
            }
        }

        void rotate_left(const SplayTree &splay_tree) {
            auto grandparent = get_parent()->get_parent();
            auto current_parent = get_parent();
            auto this_ptr = get_ptr();

            current_parent->set_right(left, splay_tree);
            set_left(current_parent, splay_tree);

            parent = grandparent;

            if (grandparent != nullptr) {
                if (grandparent->left == current_parent) {
                    grandparent->left = get_ptr();
                }
                else {
                    grandparent->right = get_ptr();
                }
                grandparent->update(splay_tree);
            }
        }

        void local_splay(const SplayTree &splay_tree) {
            auto this_ptr = get_ptr();
            auto grandparent = get_parent()->get_parent();

            if (grandparent == nullptr) {
                if (get_parent()->get_left() == get_ptr()) {
                    rotate_right(splay_tree);
                }
                else {
                    rotate_left(splay_tree);
                }
            }
            else {
                auto grandgrandparent = grandparent->get_parent();
                auto _parent = get_parent();

                if (get_parent()->get_left() == get_ptr() && grandparent->get_left() == get_parent()) {
                    get_parent()->rotate_right(splay_tree);
                    rotate_right(splay_tree);
                }
                else if (get_parent()->get_right() == get_ptr() && grandparent->get_right() == get_parent()) {
                    get_parent()->rotate_left(splay_tree);
                    rotate_left(splay_tree);
                }
                else if (get_parent()->get_right() == get_ptr() && grandparent->get_left() == get_parent()) {
                    rotate_left(splay_tree);
                    rotate_right(splay_tree);
                }
                else if (get_parent()->get_left() == get_ptr() && grandparent->get_right() == get_parent()) {
                    rotate_right(splay_tree);
                    rotate_left(splay_tree);
                }
            }
        }

        void splay(const SplayTree &splay_tree) {
            while (get_parent() != nullptr) {
                local_splay(splay_tree);
            }
        }

    public:
        explicit Node(V _value) : value(_value) {
            right = nullptr;
            left = nullptr;
            parent = node_weakptr_t();
        }

        void set_left(node_ptr_t node, const SplayTree &splay_tree) {
            left = node;
            if (node != nullptr) {
                node->parent = get_ptr();
            }
            update(splay_tree);
        }

        void set_right(node_ptr_t node, const SplayTree &splay_tree) {
            right = node;
            if (node != nullptr) {
                node->parent = get_ptr();
            }
            update(splay_tree);
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

        const V &get_value() const {
            return value;
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

        std::shared_ptr<const Node> search_no_splay(V v, const SplayTree &splay_tree) const {
            auto this_ptr = get_ptr();

            if (v < value) {
                if (left != nullptr) {
                    return left->search_no_splay(v, splay_tree);
                }
                else {
                    return get_ptr();
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->search_no_splay(v, splay_tree);
                }
                else {
                    return get_ptr();
                }
            }
            else {
                return get_ptr();
            }
        }

        node_ptr_t search(V v, SplayTree &splay_tree) {
            auto node_const = search_no_splay(v, splay_tree);
            node_ptr_t node = ((Node &)(*node_const)).get_ptr();

            node->splay(splay_tree);
            splay_tree.root = node;

            return node;
        }

        node_ptr_t insert(node_ptr_t node, SplayTree &splay_tree) {
            auto this_ptr = get_ptr();
            V v = node->get_value();

            if (v < value) {
                if (left != nullptr) {
                    return left->insert(v, splay_tree);
                }
                else {
                    set_left(node, splay_tree);
                    node->splay(splay_tree);
                    splay_tree.root = node;

                    return node;
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->insert(v, splay_tree);
                }
                else {
                    set_right(node, splay_tree);
                    node->splay(splay_tree);
                    splay_tree.root = node;

                    return node;
                }
            }
            else {
                splay(splay_tree);
                splay_tree.root = get_ptr();
                return get_ptr();
            }
        }

        node_ptr_t insert(V v, SplayTree &splay_tree) {
            return insert(std::make_shared<Node>(v), splay_tree);
        }

        node_ptr_t remove(V v, SplayTree &splay_tree) {
            auto this_ptr = get_ptr();

            if (v < value) {
                if (left != nullptr) {
                    return left->remove(v, splay_tree);
                }
                else {
                    splay(splay_tree);
                    splay_tree.root = get_ptr();
                    return get_ptr();
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->remove(v, splay_tree);
                }
                else {
                    splay(splay_tree);
                    splay_tree.root = get_ptr();
                    return get_ptr();
                }
            }
            else {
                node_ptr_t new_root = get_parent();

                if (left == nullptr && right == nullptr) {
                    if (get_parent() != nullptr) {
                        if (get_parent()->left == get_ptr()) {
                            get_parent()->set_left(nullptr, splay_tree);
                        }
                        else {
                            get_parent()->set_right(nullptr, splay_tree);
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
                        get_parent()->set_left(child, splay_tree);
                    }
                    else {
                        get_parent()->set_right(child, splay_tree);
                    }
                }
                else {
                    node_ptr_t node = left;
                    while (node->right != nullptr) {
                        node = node->right;
                    }

                    set_value(node->get_value());
                    if (node == left) {
                        set_left(node->left, splay_tree);
                    }
                    else {
                        node->get_parent()->set_right(node->left, splay_tree);
                    }

                    new_root = get_ptr();
                }


                if (new_root != nullptr) {
                    new_root->splay(splay_tree);
                }
                splay_tree.root = new_root;

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

        node_ptr_t unpin_left_subtree(const SplayTree &splay_tree) {
            auto node = left;
            if (node != nullptr) {
                node->parent = node_weakptr_t();
            }
            set_left(nullptr, splay_tree);

            return node;
        }

        node_ptr_t unpin_right_subtree(const SplayTree &splay_tree) {
            auto node = right;
            if (node != nullptr) {
                node->parent = node_weakptr_t();
            }
            set_right(nullptr, splay_tree);

            return node;
        }

        void remove_parent() {
            parent = node_weakptr_t();
        }

        template <class T>
        static const T get_function_value(node_ptr_t node, const Function<T> &function) {
            if (node == nullptr) {
                return function.get_default_null();
            }
            else if (!node->function_values.contains(function.get_name())) {
                node->function_values[function.get_name()] = function.get_default();
            }
            return std::get<T>(node->function_values[function.get_name()]);
        }
    };

    using node_ptr_t = std::shared_ptr<Node>;

    node_ptr_t root;

    template<bool increasing_direction>
    class InternalIterator {
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
        using value_type = Node;
        using pointer = node_ptr_t;
        using reference = const Node &;

        explicit InternalIterator(std::stack<node_ptr_t> traversal) : traversal(traversal) {}

        explicit InternalIterator(const SplayTree *splay) {
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

        InternalIterator(const InternalIterator &other) : InternalIterator(other.traversal) {}

        bool operator==(const InternalIterator &other) const {
            if (this->traversal.empty() == other.traversal.empty()) {
                return this->traversal.empty() == true || this->traversal.top() == other.traversal.top();
            }
            return false;
        }

        bool operator!=(const InternalIterator &other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return *traversal.top();
        }

        pointer operator->() {
            return traversal.top();
        }

        InternalIterator &operator++() {
            next();
            return *this;
        }

        InternalIterator operator++(int) {
            InternalIterator temp = *this;
            next();
            return temp;
        }
    };

    template<bool increasing_direction>
    class Iterator {
        InternalIterator<increasing_direction> iterator;

    public:
        using iterator_category = std::input_iterator_tag;
        using difference_tag = std::ptrdiff_t;
        using value_type = V;
        using pointer = V *;
        using reference = const V &;

        Iterator(InternalIterator<increasing_direction> iterator) : iterator(iterator) {}

        Iterator(const Iterator &other) : Iterator(other.iterator) {}

        bool operator==(const Iterator &other) const {
            return iterator == other.iterator;
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return (*iterator).get_value();
        }

        pointer operator->() {
            return &(iterator->get_value());
        }

        Iterator &operator++() {
            iterator++;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            iterator++;
            return temp;
        }
    };

    InternalIterator<true> internal_begin() const {
        return InternalIterator<true>(this);
    }

    InternalIterator<true> internal_end() const {
        return InternalIterator<true>(std::stack<node_ptr_t>());
    }

    InternalIterator<false> internal_rbegin() const {
        return InternalIterator<false>(this);
    }

    InternalIterator<false> internal_rend() const {
        return InternalIterator<false>(std::stack<node_ptr_t>());
    }

    node_ptr_t _search(V v) {
        return root->search(v, *this);
    }

    auto _search_no_splay(V v) const {
        return root->search_no_splay(v, *this);
    }

    node_ptr_t _insert(V v) {
        return root->insert(v, *this);
    }

    node_ptr_t _remove(V v) {
        return root->remove(v, *this);
    }

    explicit SplayTree(node_ptr_t root) : root(root) {}

    std::map<std::string, polymorphic_function_t> functions;

public:

    SplayTree() {
        root = nullptr;
    }

    SplayTree(std::initializer_list<V> values) {
        for (V value : values) {
            insert(value);
        }
    }

    explicit constexpr SplayTree(std::initializer_list<polymorphic_function_t> functions) {
        for (auto function : functions) {
            this->functions[std::visit([](auto &&func) { return func.get_name(); }, function)] = function;
        }
    }

    explicit SplayTree(std::initializer_list<V> values,
                       std::initializer_list<polymorphic_function_t> functions) : SplayTree(functions) {
        for (V value : values) {
            insert(value);
        }
    }

    Iterator<true> begin() const {
        return Iterator<true>(internal_begin());
    }

    Iterator<true> end() const {
        return Iterator<true>(internal_end());
    }

    Iterator<false> rbegin() const {
        return Iterator<false>(internal_rbegin());
    }

    Iterator<false> rend() const {
        return Iterator<false>(internal_rend());
    }

    void insert(V value) {
        if (root == nullptr) {
            root = std::make_shared<Node>(value);
        }
        else {
            root->insert(value, *this);
        }
    }

    bool contains(V value) {
        if (root == nullptr) {
            return false;
        }
        _search(value);
        V found = root->get_value();
        return found == value;
    }

    bool contains(V value) const {
        if (root == nullptr) {
            return false;
        }
        auto node = _search_no_splay(value);
        V found = node->get_value();
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
            _remove(value);
        }
    }

    SplayTree erase_less(V value) {
        _search(value);
        auto result = SplayTree(root->unpin_left_subtree(*this));

        auto v = root->get_value();
        if (v < value) {
            erase(v);
            result.insert(v);
        }

        return result;
    }

    SplayTree erase_greater(V value) {
        _search(value);
        auto result = SplayTree(root->unpin_right_subtree(*this));

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

    [[nodiscard]] bool empty() const {
        return root == nullptr;
    }

    void merge(SplayTree &other) {
        for (auto x : other) {
            if (!contains(x)) {
                insert(x);
                other.erase(x);
            }
        }
    }

    Iterator<true> find(const V value) {
        _search(value);
        return root->get_value() == value ?
            Iterator<true>(InternalIterator<true>(std::stack<node_ptr_t>({root}))) : end();
    }

    Iterator<true> find(const V value) const {
        for (auto it = begin(); it != end(); it++) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    void swap(SplayTree &other) {
        std::swap(root, other.root);
    }

    size_t count(const V value) {
        return find(value) != end();
    }

    size_t count(const V value) const {
        return find(value) != end();
    }

    Iterator<true> lower_bound(const V value) {
        _search(value);
        if (root->get_value() >= value) {
            return Iterator<true>(InternalIterator<true>(std::stack<node_ptr_t>({root})));
        }
        else {
            if (root->get_right() == nullptr) {
                return end();
            }

            node_ptr_t node = root->get_right();
            std::stack<node_ptr_t> traversal({ root, root->get_right() });

            while (node->get_left() != nullptr) {
                node = node->get_left();
                traversal.push(node);
            }

            return Iterator<true>(InternalIterator<true>(traversal));
        }
    }

    Iterator<true> upper_bound(const V value) {
        _search(value);
        if (root->get_value() > value) {
            return Iterator<true>(InternalIterator<true>(std::stack<node_ptr_t>({root})));
        }
        else {
            if (root->get_right() == nullptr) {
                return end();
            }

            node_ptr_t node = root->get_right();
            std::stack<node_ptr_t> traversal({ root, root->get_right() });

            while (node->get_left() != nullptr) {
                node = node->get_left();
                traversal.push(node);
            }

            return Iterator<true>(InternalIterator<true>(traversal));
        }
    }

    template<class T>
    T get_value(const std::string &function_name) const {
        auto function_it = functions.find(function_name);
        if (function_it == functions.end()) {
            throw std::invalid_argument("No function named" + function_name + "found.");
        }

        auto &polymorphic_func = function_it->second;
        auto &func = std::get<Function<T>>(polymorphic_func);
        return Node::template get_function_value<T>(root, func);
    }

    void print() {
        root->print_all();
    }
};