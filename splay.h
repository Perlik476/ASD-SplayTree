#include <iostream>
#include <memory>
#include <stack>
#include <map>
#include <functional>
#include <variant>

template<class V, class Compare = std::less<V>, class FunctionType = int>
class SplayTree {
public:
    class Function {
        using function_t = std::function<const FunctionType(const V &, const FunctionType &, const FunctionType &)>;

        function_t function;
        FunctionType default_value_null;

    public:
        constexpr Function(function_t &&function, FunctionType default_value_null)
                : function(std::move(function)), default_value_null(default_value_null) {}

        Function() = default;

        ~Function() = default;

        Function &operator =(const Function &other) {
            function = other.function;
            default_value_null = other.default_value_null;
            return *this;
        }

        explicit operator bool() const {
            return static_cast<bool>(function);
        }

        FunctionType operator ()(const V &value, const FunctionType &left, const FunctionType &right) const {
            return function(value, left, right);
        }

        const function_t &get_function() const {
            return function;
        }

        const FunctionType &get_default_null() const {
            return default_value_null;
        }
    };

private:
    class Node;

    template<bool increasing_direction>
    class InternalIterator;

    using node_ptr_t = std::shared_ptr<Node>;
    using const_node_ptr_t = std::shared_ptr<const Node>;
    using traversal_t = std::stack<const_node_ptr_t>;

    class Node : public std::enable_shared_from_this<Node> {
        using node_ptr_t = std::shared_ptr<Node>;
        using node_weakptr_t = std::weak_ptr<Node>;

        node_ptr_t right, left;
        node_weakptr_t parent;
        V value;
        size_t subtree_size = 1;

         FunctionType function_value;

        node_ptr_t get_ptr() {
            return this->shared_from_this();
        }

        const_node_ptr_t get_ptr() const {
            return this->shared_from_this();
        }

        node_ptr_t get_parent() {
            return parent.lock();
        }

        void update(const SplayTree &splay) {
            subtree_size = 1 + get_subtree_size(right) + get_subtree_size(left);

            auto func = splay.function;
            if (func) {
                function_value = func(value, get_function_value(left, func), get_function_value(right, func));
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

        InternalIterator<true> search_no_splay(V v, const SplayTree &splay_tree, traversal_t &traversal) const {
            auto this_ptr = get_ptr();
            traversal.push(this_ptr);

            if (Compare{}(v, value)) {
                if (left != nullptr) {
                    return left->search_no_splay(v, splay_tree, traversal);
                }
                else {
                    return InternalIterator<true>(traversal);
                }
            }
            else if (Compare{}(value, v)) {
                if (right != nullptr) {
                    return right->search_no_splay(v, splay_tree, traversal);
                }
                else {
                    return InternalIterator<true>(traversal);
                }
            }
            else {
                return InternalIterator<true>(traversal);
            }
        }

        InternalIterator<true> search_no_splay(V v, const SplayTree &splay_tree) const {
            auto traversal = traversal_t();
            return search_no_splay(v, splay_tree, traversal);
        }

        node_ptr_t search(V v, SplayTree &splay_tree) {
            auto node_it = search_no_splay(v, splay_tree);
            auto node = ((Node &)(*node_it)).get_ptr();

            node->splay(splay_tree);
            splay_tree.root = node;

            return node;
        }

        node_ptr_t insert(node_ptr_t node, SplayTree &splay_tree) {
            auto this_ptr = get_ptr();
            V v = node->get_value();

            if (Compare{}(v, value)) {
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
            else if (Compare{}(value, v)) {
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

            if (Compare{}(v, value)) {
                if (left != nullptr) {
                    return left->remove(v, splay_tree);
                }
                else {
                    splay(splay_tree);
                    splay_tree.root = get_ptr();
                    return get_ptr();
                }
            }
            else if (Compare{}(value, v)) {
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

        void get_next(traversal_t &traversal) const {
            if (right != nullptr) {
                auto node = right;

                while (node->left != nullptr) {
                    traversal.push(node);
                    node = node->left;
                }
                traversal.push(node);
            }
        }

        void get_previous(traversal_t &traversal) const {
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

        static const FunctionType get_function_value(node_ptr_t node, const Function &function) {
            if (node == nullptr) {
                return function.get_default_null();
            }
            return node->function_value;
        }
    };

    node_ptr_t root;

    template<bool increasing_direction>
    class InternalIterator {
        traversal_t traversal;

        void next() {
            auto node = traversal.top();
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
        using value_type = const Node;
        using pointer = const_node_ptr_t;
        using reference = const Node &;

        explicit InternalIterator(traversal_t traversal) : traversal(traversal) {}

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
        using value_type = const V;
        using pointer = const V *;
        using reference = const V &;

        Iterator(InternalIterator<increasing_direction> iterator) : iterator(iterator) {}

        Iterator(traversal_t traversal) : iterator(InternalIterator<increasing_direction>(traversal)) {}

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

        pointer operator->() const {
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
        return InternalIterator<true>(traversal_t());
    }

    InternalIterator<false> internal_rbegin() const {
        return InternalIterator<false>(this);
    }

    InternalIterator<false> internal_rend() const {
        return InternalIterator<false>(traversal_t());
    }

    auto _search(V v) {
        return root->search(v, *this);
    }

    auto _search_no_splay(V v) const {
        return root->search_no_splay(v, *this);
    }

    auto _insert(V v) {
        return root->insert(v, *this);
    }

    auto _remove(V v) {
        return root->remove(v, *this);
    }

    explicit SplayTree(node_ptr_t root) : root(root) {}

     Function function;

public:

    SplayTree() {
        root = nullptr;
    }

    SplayTree(std::initializer_list<V> values) {
        for (V value : values) {
            insert(value);
        }
    }

    explicit constexpr SplayTree( Function function) : function(function) {}

    SplayTree(std::initializer_list<V> values,
                        Function function) : SplayTree(function) {
        insert(values);
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

    Iterator<true> insert(const V &value) {
        if (root == nullptr) {
            root = std::make_shared<Node>(value);
        }
        else {
            root->insert(value, *this);
        }

        return Iterator<true>(traversal_t({ root }));
    }

    void insert(std::initializer_list<V> values) {
        for (auto value : values) {
            insert(value);
        }
    }

    bool contains(const V &value) {
        if (root == nullptr) {
            return false;
        }

        _search(value);
        V found = root->get_value();

        return !Compare{}(found, value) && !Compare{}(value, found);
    }

    bool contains(const V &value) const {
        if (root == nullptr) {
            return false;
        }

        auto node = _search_no_splay(value);
        V found = node->get_value();

        return !Compare{}(found, value) && !Compare{}(value, found);
    }

    size_t size() const {
        return Node::get_subtree_size(root);
    }

    bool erase(const V &value) {
        if (!contains(value)) {
            return false;
        }

        _remove(value);
        return true;
    }

    Iterator<true> erase(const Iterator<true> &pos) {
        if (pos == end()) {
            return end();
        }

        auto value = *pos;

        if (erase(value)) {
            auto it = Iterator<true>(traversal_t({ root }));
            if (Compare{}(*it, value)) {
                it++;
            }
            return it;
        }
        else {
            return end();
        }
    }

    SplayTree erase_less(const V &value) {
        _search(value);
        auto result = SplayTree(root->unpin_left_subtree(*this));

        auto v = root->get_value();
        if (Compare{}(v, value)) {
            erase(v);
            result.insert(v);
        }

        return result;
    }

    SplayTree erase_greater(const V &value) {
        _search(value);
        auto result = SplayTree(root->unpin_right_subtree(*this));

        auto v = root->get_value();
        if (Compare{}(value, v)) {
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

    Iterator<true> find(const V &value) {
        _search(value);
        return !Compare{}(root->get_value(), value) && !Compare{}(value, root->get_value()) ?
            Iterator<true>(traversal_t({root})) : end();
    }

    Iterator<true> find(const V &value) const {
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

    size_t count(const V &value) {
        return find(value) != end();
    }

    size_t count(const V &value) const {
        return find(value) != end();
    }

    Iterator<true> lower_bound(const V &value) {
        _search(value);
        if (!Compare{}(root->get_value(), value)) {
            return Iterator<true>(traversal_t({root}));
        }
        else {
            if (root->get_right() == nullptr) {
                return end();
            }

            node_ptr_t node = root->get_right();
            traversal_t traversal({ root, root->get_right() });

            while (node->get_left() != nullptr) {
                node = node->get_left();
                traversal.push(node);
            }

            return Iterator<true>(traversal);
        }
    }

    Iterator<true> upper_bound(const V &value) {
        _search(value);
        if (Compare{}(value, root->get_value())) {
            return Iterator<true>(traversal_t({root}));
        }
        else {
            if (root->get_right() == nullptr) {
                return end();
            }

            node_ptr_t node = root->get_right();
            traversal_t traversal({ root, root->get_right() });

            while (node->get_left() != nullptr) {
                node = node->get_left();
                traversal.push(node);
            }

            return Iterator<true>(traversal);
        }
    }

     FunctionType get_function_value() const {
        return Node::get_function_value(root, function);
    }

    SplayTree &operator =(const SplayTree &other) {
        function = other.function;
        root = other.root;

        return *this;
    }

    void print() {
        root->print_all();
    }
};