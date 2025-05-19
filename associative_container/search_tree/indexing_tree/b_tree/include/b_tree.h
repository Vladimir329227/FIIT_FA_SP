#include <iterator>
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>
#include <concepts>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>
#include <queue>

#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type_const;

    template <typename TKey, typename TValue, typename TCompare, size_t T>
    friend void print_tree(const B_tree<TKey, TValue, TCompare, T>& tree);

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration


    struct btree_node
    {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node*, maximum_keys_in_node + 2> _pointers;

        btree_node() noexcept = default;
//        std::vector<tree_data_type, pp_allocator<tree_data_type>> _keys;
//        std::vector<btree_node*, pp_allocator<btree_node*>> _pointers;

//        btree_node(pp_allocator<value_type> al);
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    btree_node* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit B_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    B_tree(const B_tree& other);

    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);

    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;

    // endregion five declaration

    // region iterators declaration

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);

    };

    class btree_const_iterator final
    {
        std::stack<std::pair<const btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(
                const std::stack<std::pair<const btree_node**, size_t>>& path = std::stack<std::pair<const btree_node**, size_t>>(),size_t index = 0);

    };

    class btree_reverse_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;
        size_t reverse_index() const noexcept;
        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;
        size_t reverse_index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<const btree_node**, size_t>>& path = std::stack<std::pair<const btree_node**, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

    // endregion iterators declaration

    // region element access declaration

    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;

    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    // endregion element access declaration
    // region iterator begins declaration

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    btree_iterator lower_bound(const tkey& key);
    btree_const_iterator lower_bound(const tkey& key) const;

    btree_iterator upper_bound(const tkey& key);
    btree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    template <typename ...Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    btree_iterator insert_or_assign(const tree_data_type& data);
    btree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    btree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);


    btree_iterator erase(const tkey& key);

    // endregion modifiers declaration

    void split_child(
            btree_node* parent,
            size_t index,
            std::stack<std::pair<btree_node**, size_t>>& path
    );
    btree_node* find_max_in_subtree(btree_node* node);
    void fix_underflow(btree_node* node);

    btree_node* allocate_node();
    void deallocate_node(btree_node* node) noexcept;

};

template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}

/*
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept
{

}
*/

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger* B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return _allocator;
}

// region constructors implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
        : compare(cmp),
          _allocator(alloc),
          _logger(logger),
          _root(nullptr),
          _size(0) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        pp_allocator<value_type> alloc,\
        const compare& comp,
        logger* logger)
        : compare(comp),
          _allocator(alloc),
          _logger(logger),
          _root(nullptr),
          _size(0) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
        : compare(cmp),
          _allocator(alloc),
          _logger(logger),
          _root(nullptr),
          _size(0)
{
    for (auto it = begin; it != end; ++it)
    {
        insert(*it);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
        : B_tree(data.begin(), data.end(), cmp, alloc, logger) {}

// endregion constructors implementation

// region five implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    if (!_root) return;

    std::stack<btree_node*> nodes_stack;
    nodes_stack.push(_root);

    while (!nodes_stack.empty()) {
        btree_node* current = nodes_stack.top();
        nodes_stack.pop();

        for (auto child : current->_pointers) {
            if (child) nodes_stack.push(child);
        }

        deallocate_node(current); // Используем deallocate_node
    }

    _root = nullptr;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other)
        : compare(static_cast<const compare&>(other)),
          logger_guardant(other),
          _allocator(other._allocator),
          _logger(other._logger),
          _root(nullptr),
          _size(other._size)
{
    if (!other._root) return;

    // Создаем корневой узел
    _root = _allocator.template allocate_object<btree_node>();
    _root->_keys = other._root->_keys;

    // Рекурсивное копирование дочерних узлов
    std::stack<std::pair<btree_node*, const btree_node*>> stack;
    stack.emplace(_root, other._root);

    while (!stack.empty()) {
        auto [dst_node, src_node] = stack.top();
        stack.pop();

        for (size_t i = 0; i < src_node->_pointers.size(); ++i) {
            if (src_node->_pointers[i]) {
                btree_node* new_child = _allocator.template allocate_object<btree_node>();
                new_child->_keys = src_node->_pointers[i]->_keys;

                dst_node->_pointers[i] = new_child;
                stack.emplace(new_child, src_node->_pointers[i]);
            }
        }
    }
}
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)
{
    if (this != &other) {
        // Создаем временную копию
        B_tree temp(other);

        // Обмениваем состояние
        swap(temp._root, _root);
        std::swap(temp._size, _size);
        std::swap(temp._logger, _logger);
        swap(temp._allocator, _allocator);
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept
        : compare(std::move(other)),
          logger_guardant(std::move(other)),
          _allocator(std::move(other._allocator)),
          _logger(other._logger),
          _root(other._root),
          _size(other._size)
{
    other._root = nullptr;
    other._size = 0;
    other._logger = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept
{
    if (this != &other)
    {
        // Освобождаем текущие ресурсы
        clear();

        // Перемещаем базовые классы
        compare::operator=(std::move(other));
        logger_guardant::operator=(std::move(other));

        // Перехватываем ресурсы
        _allocator = std::move(other._allocator);
        _logger = other._logger;
        _root = other._root;
        _size = other._size;

        // Обнуляем исходный объект
        other._root = nullptr;
        other._size = 0;
        other._logger = nullptr;
    }
    return *this;
}

// endregion five implementation

// region iterators implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)
        : _path(std::move(path)), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    auto& non_const_pair = (*_path.top().first)->_keys[_index];
    return *reinterpret_cast<value_type*>(&non_const_pair);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    return &(**this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    btree_node* current = *_path.top().first;

    // Если есть правый потомок
    if (!current->_pointers.empty() && current->_pointers[_index + 1])
    {
        // Идем в правого потомка и до минимального элемента
        _path.push({&current->_pointers[_index + 1], 0});
        while (!(*_path.top().first)->_pointers.empty() && (*_path.top().first)->_pointers[0])
        {
            _path.push({&(*_path.top().first)->_pointers[0], 0});
        }
        _index = 0;
    }
    else
    {
        // Двигаемся вверх пока не найдем следующий ключ
        while (!_path.empty())
        {
            _index++;
            if (_index < (*_path.top().first)->_keys.size())
            {
                return *this;
            }
            _path.pop();
        }
        // Достигли конца
        _index = 0;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    self tmp = *this;
    ++*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    if (_path.empty()) return *this;

    btree_node* current = *_path.top().first;

    // Если есть левый потомок
    if (!current->_pointers.empty() && current->_pointers[_index])
    {
        // Идем в левого потомка и до максимального элемента
        _path.push({&current->_pointers[_index], (*_path.top().first)->_keys.size() - 1});
        while (!(*_path.top().first)->_pointers.empty() &&
               (*_path.top().first)->_pointers.back())
        {
            _path.push({&(*_path.top().first)->_pointers.back(),
                        (*_path.top().first)->_keys.size() - 1});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    else
    {
        // Двигаемся вверх пока не найдем предыдущий ключ
        while (!_path.empty())
        {
            if (_index > 0)
            {
                _index--;
                return *this;
            }
            _index = _path.top().second;
            _path.pop();
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    self tmp = *this;
    --*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<const btree_node**, size_t>>& path,
        size_t index
) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept
{
    std::stack<std::pair<const btree_node**, size_t>> temp_stack;
    auto src_stack = it._path;

    while (!src_stack.empty()) {
        auto [node_ptr, idx] = src_stack.top();
        temp_stack.push({const_cast<const btree_node**>(node_ptr), idx});
        src_stack.pop();
    }

    while (!temp_stack.empty()) {
        _path.push(temp_stack.top());
        temp_stack.pop();
    }
    _index = it._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    assert(!_path.empty() && "Dereferencing end iterator");
    const btree_node* node = *_path.top().first;
    assert(node && "Dereferencing invalid iterator");
    assert(_index < node->_keys.size() && "Index out of range");
    return node->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    return &(**this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    if (_path.empty()) {
        throw std::logic_error("Incrementing invalid iterator");
    }

    const btree_node* node = *_path.top().first;
    size_t& current_index = _path.top().second;

    // Если есть правый потомок, идем к минимальному элементу в нем
    if (current_index < node->_pointers.size() - 1) {
        const btree_node* child = node->_pointers[current_index + 1];
        while (!child->_pointers.empty()) {
            _path.emplace(&child, 0);
            child = child->_pointers[0];
        }
        _path.emplace(&child, 0);
        _index = 0;
    } else {
        // Возвращаемся к родителю
        _path.pop();
        if (!_path.empty()) {
            _index = _path.top().second + 1;
        } else {
            _index = 0; // Конец обхода
        }
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    self tmp = *this;
    ++*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    if (_path.empty()) return *this;

    const btree_node* current = *_path.top().first;

    if (!current->_pointers.empty() && current->_pointers[_index]) {
        _path.push({const_cast<const btree_node**>(&current->_pointers[_index]),
                    (*_path.top().first)->_keys.size() - 1});
        while (!(*_path.top().first)->_pointers.empty() &&
               (*_path.top().first)->_pointers.back()) {
            _path.push({const_cast<const btree_node**>(&(*_path.top().first)->_pointers.back()),
                        (*_path.top().first)->_keys.size() - 1});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    } else {
        while (!_path.empty()) {
            if (_index > 0) {
                _index--;
                return *this;
            }
            _index = _path.top().second;
            _path.pop();
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    self tmp = *this;
    --*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)
        : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const btree_iterator& it) noexcept
        : _path(it._path), _index(it._index)
{
    // Реверсируем порядок обхода
    if (!_path.empty())
    {
        _index = (*_path.top().first)->_keys.size() - _index - 1;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    std::stack<std::pair<btree_node**, size_t>> path_copy = _path;
    size_t idx = _index;

    if (!path_copy.empty())
    {
        idx = (*path_copy.top().first)->_keys.size() - idx - 1;
    }

    return btree_iterator(path_copy, idx);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    return (*_path.top().first)->_keys[reverse_index()];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    return &(**this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    btree_node* current = *_path.top().first;

    // Логика обратная обычному итератору
    if (!current->_pointers.empty() && current->_pointers[reverse_index()])
    {
        _path.push({&current->_pointers[reverse_index()], 0});
        while (!(*_path.top().first)->_pointers.empty() &&
               (*_path.top().first)->_pointers.back())
        {
            _path.push({&(*_path.top().first)->_pointers.back(), 0});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    else
    {
        while (!_path.empty())
        {
            if (_index > 0)
            {
                --_index;
                return *this;
            }
            _path.pop();
            if (!_path.empty())
            {
                _index = (*_path.top().first)->_keys.size() - 1;
            }
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    self tmp = *this;
    ++*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    btree_node* current = *_path.top().first;

    // Обратная логика обычному --
    if (!current->_pointers.empty() && current->_pointers[reverse_index() + 1])
    {
        _path.push({&current->_pointers[reverse_index() + 1], 0});
        while (!(*_path.top().first)->_pointers.empty() &&
               (*_path.top().first)->_pointers[0])
        {
            _path.push({&(*_path.top().first)->_pointers[0], 0});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    else
    {
        while (!_path.empty())
        {
            _index++;
            if (_index < (*_path.top().first)->_keys.size())
            {
                return *this;
            }
            _path.pop();
        }
        _index = 0;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    self tmp = *this;
    --*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reverse_index() const noexcept
{
    return (*_path.top().first)->_keys.size() - _index - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const std::stack<std::pair<const btree_node**, size_t>>& path, size_t index)
        : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const btree_reverse_iterator& it) noexcept
{
    // Конвертация указателей в константные
    std::stack<std::pair<const btree_node**, size_t>> temp_stack;
    auto original_stack = it._path;

    while (!original_stack.empty()) {
        auto& entry = original_stack.top();
        temp_stack.push({const_cast<const btree_node**>(entry.first), entry.second});
        original_stack.pop();
    }

    // Восстановление порядка
    while (!temp_stack.empty()) {
        _path.push(temp_stack.top());
        temp_stack.pop();
    }
    _index = it._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    std::stack<std::pair<const btree_node**, size_t>> path_copy = _path;
    size_t idx = _index;

    if (!path_copy.empty()) {
        idx = (*path_copy.top().first)->_keys.size() - idx - 1;
    }

    return btree_const_iterator(path_copy, idx);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    return (*_path.top().first)->_keys[reverse_index()];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    return &(**this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    const btree_node* current = *_path.top().first;

    if (!current->_pointers.empty() && current->_pointers[reverse_index()]) {
        _path.push({const_cast<const btree_node**>(&current->_pointers[reverse_index()]), 0});
        while (!(*_path.top().first)->_pointers.empty() &&
               (*_path.top().first)->_pointers.back()) {
            _path.push({const_cast<const btree_node**>(
                                &(*_path.top().first)->_pointers.back()), 0});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    } else {
        while (!_path.empty()) {
            if (_index > 0) {
                --_index;
                return *this;
            }
            _path.pop();
            if (!_path.empty()) {
                _index = (*_path.top().first)->_keys.size() - 1;
            }
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    self tmp = *this;
    ++*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    const btree_node* current = *_path.top().first;

    if (!current->_pointers.empty() && current->_pointers[reverse_index() + 1]) {
        _path.push({const_cast<const btree_node**>(
                            &current->_pointers[reverse_index() + 1]), 0});
        while (!(*_path.top().first)->_pointers.empty() &&
               (*_path.top().first)->_pointers[0]) {
            _path.push({const_cast<const btree_node**>(
                                &(*_path.top().first)->_pointers[0]), 0});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    } else {
        while (!_path.empty()) {
            _index++;
            if (_index < (*_path.top().first)->_keys.size()) return *this;
            _path.pop();
        }
        _index = 0;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    self tmp = *this;
    --*this;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    return (*_path.top().first)->_pointers.empty();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reverse_index() const noexcept
{
    return (*_path.top().first)->_keys.size() - _index - 1;
}

// endregion iterators implementation

// region element access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)
{
    auto it = find(key);
    if (it == end())
    {
        throw std::out_of_range("Key not found in B-tree");
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const
{
    auto it = find(key);
    if (it == end())
    {
        throw std::out_of_range("Key not found in B-tree");
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)
{
    return insert_or_assign({key, tvalue()})->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    return insert_or_assign({std::move(key), tvalue()})->second;
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node** current = &_root;

    while (*current && !(*current)->_pointers.empty())
    {
        path.emplace(current, 0);
        current = &(*current)->_pointers[0];
    }

    return btree_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    return btree_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    return cbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    return cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    std::stack<std::pair<const btree_node**, size_t>> path;
    const btree_node* current = _root;
    size_t index = 0;

    if (!current) {
        return btree_const_iterator();
    }

    // Спуск к самому левому узлу
    while (!current->_pointers.empty()) {
        path.emplace(&current, 0);
        current = current->_pointers[0];
    }

    path.emplace(&current, 0);
    return btree_const_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    return btree_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    auto it = end();
    if (_root) --it;
    return btree_reverse_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    return btree_reverse_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    return crbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    return crend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    auto it = cend();
    if (_root) --it;
    return btree_const_reverse_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    return btree_const_reverse_iterator();
}

// endregion iterator begins implementation

// region lookup implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node** current = &_root;
    size_t index = 0;

    while (*current)
    {
        auto& keys = (*current)->_keys;
        auto it = std::lower_bound(keys.begin(), keys.end(), key,
                                   [this](const auto& a, const auto& b) {
                                       return compare_keys(a.first, b);
                                   });

        index = it - keys.begin();
        path.emplace(current, index);

        if (it != keys.end() && !compare_keys(key, it->first))
        {
            return btree_iterator(path, index);
        }

        if ((*current)->_pointers.empty()) break;
        current = &(*current)->_pointers[index];
    }

    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    return const_cast<B_tree*>(this)->find(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)
{
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node** current = &_root;
    size_t index = 0;

    while (*current)
    {
        auto& keys = (*current)->_keys;
        auto it = std::lower_bound(keys.begin(), keys.end(), key,
                                   [this](const auto& a, const auto& b) {
                                       return compare_keys(a.first, b);
                                   });

        index = it - keys.begin();
        path.emplace(current, index);

        if (!(*current)->_pointers.empty())
        {
            current = &(*current)->_pointers[index];
        }
        else
        {
            if (it != keys.end()) return btree_iterator(path, index);
            break;
        }
    }

    while (!path.empty())
    {
        auto& [node_ptr, idx] = path.top();
        if (idx < (*node_ptr)->_keys.size())
            return btree_iterator(path, idx);
        path.pop();
    }

    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const
{
    return const_cast<B_tree*>(this)->lower_bound(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)
{
    auto it = lower_bound(key);
    if (it != end() && !compare_keys(key, it->first))
        ++it;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const
{
    return const_cast<B_tree*>(this)->upper_bound(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    return find(key) != end();
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    if (!_root) return;

    std::stack<btree_node*> stack;
    stack.push(_root);

    while (!stack.empty())
    {
        auto node = stack.top();
        stack.pop();

        for (auto child : node->_pointers)
            if (child) stack.push(child);

        _allocator.deallocate_object(node);
    }

    _root = nullptr;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data)
{
    return emplace(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    return emplace(std::move(data));
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::split_child(
        btree_node* parent,
        size_t index,
        std::stack<std::pair<btree_node**, size_t>>& path
) {
    if (parent == _root && parent->_pointers.empty()) {
        // Разделение корня-листа
        btree_node* new_root = allocate_node();
        new (new_root) btree_node();
        btree_node* left = allocate_node();
        new (left) btree_node();
        btree_node* right = allocate_node();
        new (right) btree_node();

        const size_t split_pos = parent->_keys.size() / 2;

        left->_keys.assign(parent->_keys.begin(), parent->_keys.begin() + split_pos);
        right->_keys.assign(parent->_keys.begin() + split_pos + 1, parent->_keys.end());

        new_root->_keys.push_back(std::move(parent->_keys[split_pos]));
        new_root->_pointers.push_back(left);
        new_root->_pointers.push_back(right);

        deallocate_node(_root);
        _root = new_root;
        return;
    }

    btree_node* child = parent->_pointers[index];
    const size_t split_pos = t - 1;
    btree_node* new_node = allocate_node();
    new (new_node) btree_node();

    new_node->_keys.assign(
            child->_keys.begin() + split_pos + 1,
            child->_keys.end()
    );
    child->_keys.resize(split_pos);

    if (!child->_pointers.empty()) {
        new_node->_pointers.assign(
                child->_pointers.begin() + split_pos + 1,
                child->_pointers.end()
        );
        child->_pointers.resize(split_pos + 1);
    }

    parent->_keys.insert(parent->_keys.begin() + index, std::move(child->_keys[split_pos]));
    parent->_pointers.insert(parent->_pointers.begin() + index + 1, new_node);

    if (parent->_keys.size() > maximum_keys_in_node) {
        if (parent == _root) {
            btree_node* new_root = allocate_node();
            new (new_root) btree_node();
            new_root->_pointers.push_back(parent);
            _root = new_root;
            split_child(new_root, 0, path);
        } else {
            auto [grand_parent_ptr, parent_idx] = path.top();
            path.pop();
            split_child(*grand_parent_ptr, parent_idx, path);
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    tree_data_type data(std::forward<Args>(args)...);
    std::stack<std::pair<btree_node**, size_t>> path;
    btree_node** current = &_root;
    size_t index = 0;

    if (_root == nullptr) {
        _root = allocate_node(); // Используем allocate_node
        new (_root) btree_node(); // Явный вызов конструктора
        _root->_keys.emplace_back(std::move(data));
        _size = 1;

        std::stack<std::pair<btree_node**, size_t>> temp_path;
        temp_path.emplace(&_root, 0);
        return {btree_iterator(temp_path, 0), true};
    }

    while (*current) {
        auto& keys = (*current)->_keys;
        auto it = std::lower_bound(
                keys.begin(), keys.end(), data.first,
                [this](const tree_data_type& a, const tkey& b) {
                    return compare::operator()(a.first, b);
                }
        );

        index = it - keys.begin();
        path.emplace(current, index);

        if (it != keys.end() && it->first == data.first) {
            return {btree_iterator(path, index), false};
        }

        if (!(*current)->_pointers.empty()) {
            current = &(*current)->_pointers[index];
        } else {
            break;
        }
    }

    btree_node* leaf = *current;
    auto insert_pos = std::lower_bound(
            leaf->_keys.begin(), leaf->_keys.end(), data,
            [this](const tree_data_type& a, const tree_data_type& b) {
                return compare::operator()(a.first, b.first);
            }
    );

    leaf->_keys.emplace(insert_pos, std::move(data));
    _size++;

    if (leaf->_keys.size() > maximum_keys_in_node) {
        if (path.empty()) {
            split_child(_root, 0, path);
        } else {
            auto [parent_ptr, child_idx] = path.top();
            split_child(*parent_ptr, child_idx, path);
        }
    }

    std::stack<std::pair<btree_node**, size_t>> it_path;
    current = &_root;
    while (*current) {
        auto& keys = (*current)->_keys;
        auto it = std::lower_bound(
                keys.begin(), keys.end(), data.first,
                [this](const tree_data_type& a, const tkey& b) {
                    return compare::operator()(a.first, b);
                }
        );
        size_t pos = it - keys.begin();
        it_path.emplace(current, pos);
        if ((*current)->_pointers.empty()) break;
        current = &(*current)->_pointers[pos];
    }

    return {btree_iterator(it_path, index), true};
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    auto [it, inserted] = insert(data);
    if (!inserted) it->second = data.second;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    auto [it, inserted] = insert(std::move(data));
    if (!inserted) it->second = std::move(data.second);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    tree_data_type data(std::forward<Args>(args)...);
    auto [it, inserted] = emplace(std::move(data));
    if (!inserted) it->second = std::move(data.second);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    if (pos == end()) return end();

    // Сохраняем следующий итератор перед удалением
    auto next_it = pos;
    ++next_it;

    // Получаем узел и индекс из итератора
    auto& [node_ptr, index] = pos._path.top();
    btree_node* node = *node_ptr;

    // Удаление ключа
    node->_keys.erase(node->_keys.begin() + index);
    _size--;

    // Исправление структуры дерева
    while (node != _root && node->_keys.size() < minimum_keys_in_node)
    {
        // Поиск родителя и индекса текущего узла
        pos._path.pop();
        auto& [parent_ptr, child_idx] = pos._path.top();
        btree_node* parent = *parent_ptr;

        // Попытка заимствования у левого соседа
        bool borrowed = false;
        if (child_idx > 0)
        {
            btree_node* left_sib = parent->_pointers[child_idx - 1];
            if (left_sib->_keys.size() > minimum_keys_in_node)
            {
                // Заимствуем ключ из левого соседа
                node->_keys.insert(node->_keys.begin(),
                                   std::move(parent->_keys[child_idx - 1]));
                parent->_keys[child_idx - 1] =
                        std::move(left_sib->_keys.back());
                left_sib->_keys.pop_back();

                if (!left_sib->_pointers.empty()) {
                    node->_pointers.insert(node->_pointers.begin(),
                                           left_sib->_pointers.back());
                    left_sib->_pointers.pop_back();
                }
                borrowed = true;
            }
        }

        // Попытка заимствования у правого соседа
        if (!borrowed && child_idx < parent->_pointers.size() - 1)
        {
            btree_node* right_sib = parent->_pointers[child_idx + 1];
            if (right_sib->_keys.size() > minimum_keys_in_node)
            {
                // Заимствуем ключ из правого соседа
                node->_keys.push_back(std::move(parent->_keys[child_idx]));
                parent->_keys[child_idx] =
                        std::move(right_sib->_keys.front());
                right_sib->_keys.erase(right_sib->_keys.begin());

                if (!right_sib->_pointers.empty()) {
                    node->_pointers.push_back(right_sib->_pointers.front());
                    right_sib->_pointers.erase(right_sib->_pointers.begin());
                }
                borrowed = true;
            }
        }

        // Слияние с соседом при необходимости
        if (!borrowed)
        {
            size_t merge_idx = (child_idx > 0) ? child_idx - 1 : child_idx;
            btree_node* left = parent->_pointers[merge_idx];
            btree_node* right = parent->_pointers[merge_idx + 1];

            // Перенос ключа из родителя
            left->_keys.push_back(std::move(parent->_keys[merge_idx]));

            // Объединение ключей и указателей
            left->_keys.insert(left->_keys.end(),
                               std::make_move_iterator(right->_keys.begin()),
                               std::make_move_iterator(right->_keys.end()));
            left->_pointers.insert(left->_pointers.end(),
                                   std::make_move_iterator(right->_pointers.begin()),
                                   std::make_move_iterator(right->_pointers.end()));

            // Удаление правого узла и обновление родителя
            parent->_keys.erase(parent->_keys.begin() + merge_idx);
            parent->_pointers.erase(parent->_pointers.begin() + merge_idx + 1);
            _allocator.deallocate_object(right);

            // Переходим к проверке родителя
            node = parent;
        }
        else
        {
            break;
        }
    }

    // Обновление корня при необходимости
    if (_root->_keys.empty() && !_root->_pointers.empty())
    {
        _root = _root->_pointers[0];
    }

    return next_it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    // Конвертируем const_iterator в iterator
    return erase(btree_iterator(pos._path, pos._index));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    while (beg != en)
    {
        beg = erase(beg);
    }
    return beg;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    return erase(btree_iterator(beg), btree_iterator(en));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    auto it = find(key);
    if (it == end()) return end();

    btree_node* node = *it._path.top().first;
    size_t index = it._index;

    if (node->_pointers.empty()) {
        node->_keys.erase(node->_keys.begin() + index);
        _size--;
        fix_underflow(node);
    } else {
        btree_node* predecessor = find_max_in_subtree(node->_pointers[index]);
        node->_keys[index] = predecessor->_keys.back();
        predecessor->_keys.pop_back();
        _size--;
        fix_underflow(predecessor);
    }

    return ++it;
}


// Вспомогательные методы
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_node*
B_tree<tkey, tvalue, compare, t>::find_max_in_subtree(btree_node* node)
{
    while (!node->_pointers.empty()) {
        node = node->_pointers.back();
    }
    return node;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::fix_underflow(btree_node* node)
{
    while (node != _root && node->_keys.size() < minimum_keys_in_node) {
        // Находим родителя и индекс текущего узла
        btree_node* parent = nullptr;
        size_t child_idx = 0;

        // Поиск родителя (требуется дополнительный стек или обход)
        // Для упрощения предположим, что есть функция find_parent
        // В реальной реализации нужно отслеживать путь при спуске
        // Здесь требуется доработка для отслеживания пути к узлу

        // Временная заглушка: пропускаем логику из-за сложности без пути
        // Необходимо переработать метод erase с отслеживанием пути

        // Пример логики заимствования/слияния:
        if (parent) {
            // Попытка заимствования у левого брата
            if (child_idx > 0) {
                btree_node* left_sib = parent->_pointers[child_idx - 1];
                if (left_sib->_keys.size() > minimum_keys_in_node) {
                    // Заимствуем ключ
                    node->_keys.insert(node->_keys.begin(), parent->_keys[child_idx - 1]);
                    parent->_keys[child_idx - 1] = left_sib->_keys.back();
                    left_sib->_keys.pop_back();
                    if (!left_sib->_pointers.empty()) {
                        node->_pointers.insert(node->_pointers.begin(), left_sib->_pointers.back());
                        left_sib->_pointers.pop_back();
                    }
                    return;
                }
            }

            // Попытка заимствования у правого брата
            if (child_idx < parent->_pointers.size() - 1) {
                btree_node* right_sib = parent->_pointers[child_idx + 1];
                if (right_sib->_keys.size() > minimum_keys_in_node) {
                    node->_keys.push_back(parent->_keys[child_idx]);
                    parent->_keys[child_idx] = right_sib->_keys.front();
                    right_sib->_keys.erase(right_sib->_keys.begin());
                    if (!right_sib->_pointers.empty()) {
                        node->_pointers.push_back(right_sib->_pointers.front());
                        right_sib->_pointers.erase(right_sib->_pointers.begin());
                    }
                    return;
                }
            }

            // Слияние с братом
            if (child_idx > 0) {
                // Слияние с левым братом
                btree_node* left_sib = parent->_pointers[child_idx - 1];
                left_sib->_keys.push_back(parent->_keys[child_idx - 1]);
                left_sib->_keys.insert(left_sib->_keys.end(), node->_keys.begin(), node->_keys.end());
                left_sib->_pointers.insert(left_sib->_pointers.end(), node->_pointers.begin(), node->_pointers.end());
                parent->_keys.erase(parent->_keys.begin() + child_idx - 1);
                parent->_pointers.erase(parent->_pointers.begin() + child_idx);
                _allocator.deallocate_object(node);
                node = parent;
            } else {
                // Слияние с правым братом
                btree_node* right_sib = parent->_pointers[child_idx + 1];
                node->_keys.push_back(parent->_keys[child_idx]);
                node->_keys.insert(node->_keys.end(), right_sib->_keys.begin(), right_sib->_keys.end());
                node->_pointers.insert(node->_pointers.end(), right_sib->_pointers.begin(), right_sib->_pointers.end());
                parent->_keys.erase(parent->_keys.begin() + child_idx);
                parent->_pointers.erase(parent->_pointers.begin() + child_idx + 1);
                _allocator.deallocate_object(right_sib);
                node = parent;
            }
        }

        if (node == _root && node->_keys.empty() && !node->_pointers.empty()) {
            _root = node->_pointers[0];
            _allocator.deallocate_object(node);
            node = _root;
        }
    }
}

template <typename tkey, typename tvalue, typename compare, size_t t>
void print_tree(const B_tree<tkey, tvalue, compare, t>& tree) {
    using node = typename B_tree<tkey, tvalue, compare, t>::btree_node;

    if (!tree._root) {
        std::cout << "Tree is empty\n";
        return;
    }

    std::queue<node*> q;
    q.push(tree._root);
    int level = 0;

    while (!q.empty()) {
        int nodes_count = q.size();
        std::cout << "Level " << level << ":\t";

        for (int i = 0; i < nodes_count; ++i) {
            node* current = q.front();
            q.pop();

            // Вывод ключей
            std::cout << "[";
            for (size_t j = 0; j < current->_keys.size(); ++j) {
                std::cout << current->_keys[j].first;
                if (j < current->_keys.size() - 1) std::cout << ", ";
            }
            std::cout << "] ";

            // Добавление дочерних узлов
            for (auto&& child : current->_pointers) {
                if (child) q.push(child);
            }
        }

        std::cout << "\n";
        level++;
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_node*
B_tree<tkey, tvalue, compare, t>::allocate_node() {
    if (_allocator) {
        // Используем пользовательский аллокатор
        return _allocator.template allocate_object<btree_node>();
    } else {
        // Используем стандартный new
        return new btree_node();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::deallocate_node(btree_node* node) noexcept {
    if (!node) return;

    if (_allocator) {
        // Освобождаем через пользовательский аллокатор
        _allocator.deallocate_object(node);
    } else {
        // Используем стандартный delete
        delete node;
    }
}


#endif