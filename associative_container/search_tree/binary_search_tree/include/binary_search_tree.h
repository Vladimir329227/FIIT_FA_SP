#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H

#include <list>
#include <stack>
#include <vector>
#include <memory>
#include <logger.h>
#include <logger_guardant.h>
#include <not_implemented.h>
#include <search_tree.h>
#include <stack>
#include <ranges>
#include <pp_allocator.h>
#include <concepts>

namespace __detail
{
    template<typename tkey, typename tvalue, typename compare, typename tag>
    class bst_impl;

    class BST_TAG;
}


template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, typename tag = __detail::BST_TAG>
class binary_search_tree : private compare
{
public:

    using value_type = std::pair<const tkey, tvalue>;

    friend class __detail::bst_impl<tkey, tvalue, compare, tag>;

public:
    
    struct node
    {
    
    public:
        
        value_type data;

        node* parent;
        node* left_subtree;
        node* right_subtree;

        template<class ...Args>
        explicit node(node* parent, Args&& ...args);


        virtual ~node() =default;
    };

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;

    inline bool compare_pairs(const value_type & lhs, const value_type & rhs) const;

public:


    // region iterators definition

    class prefix_iterator;
    class prefix_const_iterator;
    class prefix_reverse_iterator;
    class prefix_const_reverse_iterator;

    class infix_iterator;
    class infix_const_iterator;
    class infix_reverse_iterator;
    class infix_const_reverse_iterator;

    class postfix_iterator;
    class postfix_const_iterator;
    class postfix_reverse_iterator;
    class postfix_const_reverse_iterator;

    /** @brief Watch about behavior of reverse iterators.
     *
     * @example Arrow is common iterator
     *  1 2 3 -> 4 5 6 7
     *  *it == 4.
     *
     *  @example But reverse:
     *  1 2 3 \<- 4 5 6 7
     *  *rit == 3
     */

    class prefix_iterator
    {
    public:

        node* _data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node* _backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        
        explicit prefix_iterator(node* data = nullptr);

        virtual ~prefix_iterator() =default;

        bool operator==(
            prefix_iterator const &other) const noexcept;
        
        bool operator!=(
            prefix_iterator const &other) const noexcept;
        
        prefix_iterator &operator++() & noexcept;
        
        prefix_iterator operator++(int not_used) noexcept;

        prefix_iterator &operator--() & noexcept;

        prefix_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

        node* get_node() const noexcept { return _data; }
    };
    
    class prefix_const_iterator
    {
    protected:

        prefix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_const_iterator(const node* data = nullptr);

        prefix_const_iterator(const prefix_iterator&) noexcept;

        virtual ~prefix_const_iterator() =default;

        bool operator==(
                prefix_iterator const &other) const noexcept;

        bool operator!=(
                prefix_iterator const &other) const noexcept;

        prefix_const_iterator &operator++() & noexcept;

        prefix_const_iterator operator++(int not_used) noexcept;

        prefix_const_iterator &operator--() & noexcept;

        prefix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };
    
    class prefix_reverse_iterator
    {
    protected:

        prefix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_reverse_iterator(node* data = nullptr);

        prefix_reverse_iterator(const prefix_iterator&) noexcept;
        operator prefix_iterator() const noexcept;

        prefix_iterator base() const noexcept;

        virtual ~prefix_reverse_iterator() =default;

        bool operator==(prefix_reverse_iterator const &other) const noexcept;

        bool operator!=(prefix_reverse_iterator const &other) const noexcept;

        prefix_reverse_iterator &operator++() & noexcept;

        prefix_reverse_iterator operator++(int not_used) noexcept;

        prefix_reverse_iterator &operator--() & noexcept;

        prefix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };
    
    class prefix_const_reverse_iterator
    {
    protected:

        prefix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_const_reverse_iterator(const node* data = nullptr);

        prefix_const_reverse_iterator(const prefix_const_iterator&) noexcept;

        operator prefix_const_iterator() const noexcept;
        prefix_const_iterator base() const noexcept;

        virtual ~prefix_const_reverse_iterator() =default;

        bool operator==(prefix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(prefix_const_reverse_iterator const &other) const noexcept;

        prefix_const_reverse_iterator &operator++() & noexcept;

        prefix_const_reverse_iterator operator++(int not_used) noexcept;

        prefix_const_reverse_iterator &operator--() & noexcept;

        prefix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };


    class infix_iterator
    {
    public:

        node* _data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node* _backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_iterator(node* data = nullptr);

        virtual ~infix_iterator() =default;

        bool operator==(
                infix_iterator const &other) const noexcept;

        bool operator!=(
                infix_iterator const &other) const noexcept;

        infix_iterator &operator++() & noexcept;

        infix_iterator operator++(int not_used) noexcept;

        infix_iterator &operator--() & noexcept;

        infix_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        node* get_node() const noexcept { return _data; }

    };

    class infix_const_iterator
    {
    public:

        infix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_const_iterator(const node* data = nullptr);

        infix_const_iterator(const infix_iterator&) noexcept;

        virtual ~infix_const_iterator() =default;

        bool operator==(
                infix_iterator const &other) const noexcept;

        bool operator!=(
                infix_iterator const &other) const noexcept;

        infix_const_iterator &operator++() & noexcept;

        infix_const_iterator operator++(int not_used) noexcept;

        infix_const_iterator &operator--() & noexcept;

        infix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class infix_reverse_iterator
    {
    public:

        infix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_reverse_iterator(node* data = nullptr);

        infix_reverse_iterator(const infix_iterator&) noexcept;
        operator infix_iterator() const noexcept;

        infix_iterator base() const noexcept;

        virtual ~infix_reverse_iterator() =default;

        bool operator==(infix_reverse_iterator const &other) const noexcept;

        bool operator!=(infix_reverse_iterator const &other) const noexcept;

        infix_reverse_iterator &operator++() & noexcept;

        infix_reverse_iterator operator++(int not_used) noexcept;

        infix_reverse_iterator &operator--() & noexcept;

        infix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class infix_const_reverse_iterator
    {
    public:

        infix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_const_reverse_iterator(const node* data = nullptr);

        infix_const_reverse_iterator(const infix_const_iterator&) noexcept;

        operator infix_const_iterator() const noexcept;
        infix_const_iterator base() const noexcept;

        virtual ~infix_const_reverse_iterator() =default;

        bool operator==(infix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(infix_const_reverse_iterator const &other) const noexcept;

        infix_const_reverse_iterator &operator++() & noexcept;

        infix_const_reverse_iterator operator++(int not_used) noexcept;

        infix_const_reverse_iterator &operator--() & noexcept;

        infix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };


    class postfix_iterator
    {
    public:

        node* _data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node* _backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_iterator(node* data = nullptr);

        virtual ~postfix_iterator() =default;

        bool operator==(
                postfix_iterator const &other) const noexcept;

        bool operator!=(
                postfix_iterator const &other) const noexcept;

        postfix_iterator &operator++() & noexcept;

        postfix_iterator operator++(int not_used) noexcept;

        postfix_iterator &operator--() & noexcept;

        postfix_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class postfix_const_iterator
    {
    public:

        postfix_iterator _base;
    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_const_iterator(const node* data = nullptr);

        postfix_const_iterator(const postfix_iterator&) noexcept;

        virtual ~postfix_const_iterator() =default;

        bool operator==(
                postfix_iterator const &other) const noexcept;

        bool operator!=(
                postfix_iterator const &other) const noexcept;

        postfix_const_iterator &operator++() & noexcept;

        postfix_const_iterator operator++(int not_used) noexcept;

        postfix_const_iterator &operator--() & noexcept;

        postfix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class postfix_reverse_iterator
    {
    protected:

        postfix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_reverse_iterator(node* data = nullptr);

        postfix_reverse_iterator(const postfix_iterator&) noexcept;
        operator postfix_iterator() const noexcept;

        postfix_iterator base() const noexcept;

        virtual ~postfix_reverse_iterator() =default;

        bool operator==(postfix_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_reverse_iterator const &other) const noexcept;

        postfix_reverse_iterator &operator++() & noexcept;

        postfix_reverse_iterator operator++(int not_used) noexcept;

        postfix_reverse_iterator &operator--() & noexcept;

        postfix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class postfix_const_reverse_iterator
    {
    protected:

        postfix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_const_reverse_iterator(const node* data = nullptr);

        postfix_const_reverse_iterator(const postfix_const_iterator&) noexcept;

        operator postfix_const_iterator() const noexcept;
        postfix_const_iterator base() const noexcept;

        virtual ~postfix_const_reverse_iterator() =default;

        bool operator==(postfix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_const_reverse_iterator const &other) const noexcept;

        postfix_const_reverse_iterator &operator++() & noexcept;

        postfix_const_reverse_iterator operator++(int not_used) noexcept;

        postfix_const_reverse_iterator &operator--() & noexcept;

        postfix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*();

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };



    friend class prefix_iterator;
    friend class prefix_const_iterator;
    friend class prefix_reverse_iterator;
    friend class prefix_const_reverse_iterator;

    friend class postfix_iterator;
    friend class postfix_const_iterator;
    friend class postfix_reverse_iterator;
    friend class postfix_const_reverse_iterator;

    friend class infix_iterator;
    friend class infix_const_iterator;
    friend class infix_reverse_iterator;
    friend class infix_const_reverse_iterator;

    // endregion iterators definition


protected:
    
    node *_root;
    logger* _logger;
    size_t _size;
    
    /** You should use coercion ctor or template methods of allocator
     */
    pp_allocator<value_type> _allocator;

public:
    explicit binary_search_tree(
            const compare& comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *logger = nullptr);

    explicit binary_search_tree(
            pp_allocator<value_type> alloc,
            const compare& comp = compare(),
            logger *logger = nullptr);


    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit binary_search_tree(iterator begin, iterator end, const compare& cmp = compare(), 
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(), 
                                logger* logger = nullptr);

    template<std::ranges::input_range Range>
    explicit binary_search_tree(Range&& range, const compare& cmp = compare(),
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                                logger* logger = nullptr);


    binary_search_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
                       pp_allocator<value_type> alloc = pp_allocator<value_type>(), 
                       logger* logger = nullptr);

public:
    
    binary_search_tree(const binary_search_tree &other);
    
    binary_search_tree(binary_search_tree &&other) noexcept;
    
    binary_search_tree &operator=(const binary_search_tree &other);
    
    binary_search_tree &operator=(binary_search_tree &&other) noexcept;
    
    virtual ~binary_search_tree();

public:

    tvalue& at(const tkey& key);
    const tvalue& at(const tkey& key) const;

    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    bool empty() const noexcept;

    size_t size() const noexcept;

    void clear() noexcept;

    std::pair<infix_iterator, bool> insert(const value_type&);
    std::pair<infix_iterator, bool> insert(value_type&&);

    template<std::input_iterator InputIt>
    void insert(InputIt first, InputIt last);

    template<std::ranges::input_range R>
    void insert_range( R&& rg );

    template<class ...Args>
    std::pair<infix_iterator, bool> emplace(Args&&...args);

    infix_iterator insert_or_assign(const value_type&);
    infix_iterator insert_or_assign(value_type&&);

    template<std::input_iterator InputIt >
    void insert_or_assign(InputIt first, InputIt last);


    template<class ...Args>
    infix_iterator emplace_or_assign(Args&&...args);

    virtual void swap(binary_search_tree& other) noexcept;

    bool contains(const tkey& key) const;

    infix_iterator find(const tkey&);
    infix_const_iterator find(const tkey&) const;


    infix_iterator lower_bound(const tkey&);
    infix_const_iterator lower_bound(const tkey&) const;

    infix_iterator upper_bound(const tkey&);
    infix_const_iterator upper_bound(const tkey&) const;

    infix_iterator erase(infix_iterator pos);
    infix_iterator erase(infix_const_iterator pos);

    infix_iterator erase(infix_iterator first, infix_iterator last);
    infix_iterator erase(infix_const_iterator first, infix_const_iterator last);

    size_t erase(const tkey& key);

public:
    
    // region iterators requests definition

    infix_iterator begin() noexcept;

    infix_iterator end() noexcept;

    infix_const_iterator begin() const noexcept;

    infix_const_iterator end() const noexcept;

    infix_const_iterator cbegin() const noexcept;

    infix_const_iterator cend() const noexcept;

    infix_reverse_iterator rbegin() noexcept;

    infix_reverse_iterator rend() noexcept;

    infix_const_reverse_iterator rbegin() const noexcept;

    infix_const_reverse_iterator rend() const noexcept;

    infix_const_reverse_iterator crbegin() const noexcept;

    infix_const_reverse_iterator crend() const noexcept;

    
    prefix_iterator begin_prefix() noexcept;
    
    prefix_iterator end_prefix() noexcept;

    prefix_const_iterator begin_prefix() const noexcept;

    prefix_const_iterator end_prefix() const noexcept;
    
    prefix_const_iterator cbegin_prefix() const noexcept;
    
    prefix_const_iterator cend_prefix() const noexcept;
    
    prefix_reverse_iterator rbegin_prefix() noexcept;
    
    prefix_reverse_iterator rend_prefix() noexcept;

    prefix_const_reverse_iterator rbegin_prefix() const noexcept;

    prefix_const_reverse_iterator rend_prefix() const noexcept;
    
    prefix_const_reverse_iterator crbegin_prefix() const noexcept;
    
    prefix_const_reverse_iterator crend_prefix() const noexcept;

    
    infix_iterator begin_infix() noexcept;
    
    infix_iterator end_infix() noexcept;

    infix_const_iterator begin_infix() const noexcept;

    infix_const_iterator end_infix() const noexcept;
    
    infix_const_iterator cbegin_infix() const noexcept;
    
    infix_const_iterator cend_infix() const noexcept;
    
    infix_reverse_iterator rbegin_infix() noexcept;
    
    infix_reverse_iterator rend_infix() noexcept;

    infix_const_reverse_iterator rbegin_infix() const noexcept;

    infix_const_reverse_iterator rend_infix() const noexcept;
    
    infix_const_reverse_iterator crbegin_infix() const noexcept;
    
    infix_const_reverse_iterator crend_infix() const noexcept;

    
    postfix_iterator begin_postfix() noexcept;
    
    postfix_iterator end_postfix() noexcept;

    postfix_const_iterator begin_postfix() const noexcept;

    postfix_const_iterator end_postfix() const noexcept;
    
    postfix_const_iterator cbegin_postfix() const noexcept;
    
    postfix_const_iterator cend_postfix() const noexcept;
    
    postfix_reverse_iterator rbegin_postfix() noexcept;
    
    postfix_reverse_iterator rend_postfix() noexcept;

    postfix_const_reverse_iterator rbegin_postfix() const noexcept;

    postfix_const_reverse_iterator rend_postfix() const noexcept;
    
    postfix_const_reverse_iterator crbegin_postfix() const noexcept;
    
    postfix_const_reverse_iterator crend_postfix() const noexcept;
    
    // endregion iterators requests definition

protected:



    // region subtree rotations definition
    
    static void small_left_rotation(node *&subtree_root) noexcept;

    static void small_right_rotation(node *&subtree_root) noexcept;

    static void big_left_rotation(node *&subtree_root) noexcept;

    static void big_right_rotation(node *&subtree_root) noexcept;

    static void double_left_rotation(node *&subtree_root) noexcept;

    static void double_right_rotation(node *&subtree_root) noexcept;
    
    // endregion subtree rotations definition

public:

    static typename binary_search_tree<tkey, tvalue, compare, tag>::node* find_last_node(node* root) noexcept;

};

namespace __detail
{
    template<typename tkey, typename tvalue, typename compare, typename tag>
    class bst_impl
    {
    public:
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, tag>::node* create_node(binary_search_tree<tkey, tvalue, compare, tag>& cont, Args&& ...args);

        static void delete_node(binary_search_tree<tkey, tvalue, compare, tag>& cont);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, tag>::node**){}

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, tag>& cont, binary_search_tree<tkey, tvalue, compare, tag>::node**){}

        static void erase(binary_search_tree<tkey, tvalue, compare, tag>& cont, binary_search_tree<tkey, tvalue, compare, tag>::node**);

        static void swap(binary_search_tree<tkey, tvalue, compare, tag>& lhs, binary_search_tree<tkey, tvalue, compare, tag>& rhs) noexcept;
    };
}

template<typename tkey, typename tvalue, typename compare, typename tag>
void __detail::bst_impl<tkey, tvalue, compare, tag>::swap(binary_search_tree<tkey, tvalue, compare, tag> &lhs,
                                                binary_search_tree<tkey, tvalue, compare, tag> &rhs) noexcept
{
    using std::swap;
    swap(lhs._root, rhs._root);
    swap(lhs._size, rhs._size);
    swap(lhs._logger, rhs._logger);
    lhs._allocator.swap(rhs._allocator);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<input_iterator_for_pair<tkey, tvalue> iterator>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(iterator begin, iterator end, const compare &cmp,
                                                                   pp_allocator<typename binary_search_tree<tkey, tvalue, compare, tag>::value_type> alloc, logger *logger)
        : compare(cmp),
          _root(nullptr),
          _logger(logger),
          _size(0),
          _allocator(alloc)
{
    for (auto it = begin; it != end; ++it)
    {
        insert(*it);
    }
}



template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::compare_pairs(const binary_search_tree::value_type &lhs,
                                                              const binary_search_tree::value_type &rhs) const
{
    // Сравниваем только ключи, значения игнорируем
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    // Используем функтор сравнения из базового класса
    return static_cast<const compare&>(*this)(lhs, rhs);
}

template<typename compare, typename U, typename iterator>
explicit binary_search_tree(iterator begin, iterator end, const compare& cmp = compare(),
                            pp_allocator<U> alloc = pp_allocator<U>(),
                            logger* logger = nullptr) -> binary_search_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit binary_search_tree(Range&& range, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* logger = nullptr) -> binary_search_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare> ;

template<typename tkey, typename tvalue, typename compare, typename U>
binary_search_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* logger = nullptr) -> binary_search_tree<tkey, tvalue, compare>;


// region node implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
binary_search_tree<tkey, tvalue, compare, tag>::node::node(node* parent, Args&& ...args)
        : data(std::forward<Args>(args)...),
          parent(parent),
          left_subtree(nullptr),
          right_subtree(nullptr)
{}

// endregion node implementation

// region prefix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::prefix_iterator(node* data)
        : _data(data)
{
    if (!_data) _backup = nullptr;
    else _backup = find_last_node(_data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator==(
        prefix_iterator const &other) const noexcept
{
    return _data == other._data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator!=(
        prefix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator++() & noexcept
{
    if (!_data) return *this;

    if (_data->left_subtree) {
        _data = _data->left_subtree;
    }
    else if (_data->right_subtree) {
        _data = _data->right_subtree;
    }
    else {
        node* parent = _data->parent;
        while (parent && (!parent->right_subtree || parent->right_subtree == _data)) {
            _data = parent;
            parent = parent->parent;
        }
        _data = parent ? parent->right_subtree : nullptr;
    }

    if (!_data) {
        _backup = find_last_node(_backup);
    }
    return *this;
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator++(int not_used) noexcept
{
    prefix_iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator--() & noexcept
{
    if (!_data) {
        _data = _backup;
        return *this;
    }

    if (_data->parent) {
        if (_data == _data->parent->right_subtree &&
            _data->parent->left_subtree)
            _data = find_last_node(_data->parent->left_subtree);
        else
            _data = _data->parent;
    } else {
        _data = nullptr;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator--(int not_used) const noexcept
{
    prefix_iterator temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator*()
{
    if (!_data) throw std::out_of_range("Dereferencing end iterator");
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator->() noexcept
{
    return &_data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::depth() const noexcept
{
    size_t depth = 0;
    node* current = _data;
    while (current->parent) {
        ++depth;
        current = current->parent;
    }
    return depth;
}

// endregion prefix_iterator implementation

// region prefix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::prefix_const_iterator(const node* data)
        : _base(data ? prefix_iterator(const_cast<node*>(data)) : prefix_iterator())
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::prefix_const_iterator(const prefix_iterator& other) noexcept
        : _base(other)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator==(
        prefix_iterator const &other) const noexcept
{
    return _base == other;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator!=(
        prefix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator++(int not_used) noexcept
{
    return prefix_const_iterator(_base++);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator--(int not_used) const noexcept
{
    return prefix_const_iterator(_base--);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator*()
{
    if (!_base._data) throw std::out_of_range("Dereferencing end iterator");
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion prefix_const_iterator implementation

// region prefix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::prefix_reverse_iterator(node* data)
    : _base(data) // Базовый итератор инициализируется прямым обходом
{
    if (data) {
        // Для обратного обхода начинаем с последнего узла префиксного порядка
        while (data->right_subtree) data = data->right_subtree;
        _base = prefix_iterator(data);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::prefix_reverse_iterator(const prefix_iterator& it) noexcept
        : _base(it)
{
    // Корректируем позицию для обратного обхода
    if (_base._data) {
        node* current = _base._data;
        while (current->right_subtree) current = current->right_subtree;
        _base = prefix_iterator(current);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator==(prefix_reverse_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator!=(prefix_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator++(int not_used) noexcept
{
    prefix_reverse_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator--(int not_used) const noexcept
{
    prefix_reverse_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator*()
{
    if (!_base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion prefix_reverse_iterator implementation

// region prefix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(const node* data)
        : _base(data ? prefix_reverse_iterator(const_cast<node*>(data)) : prefix_reverse_iterator())
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(const prefix_const_iterator& it) noexcept
        : _base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator() const noexcept
{
    return _base.base();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::base() const noexcept
{
    return _base.base();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator==(prefix_const_reverse_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator!=(prefix_const_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator++(int not_used) noexcept
{
    prefix_const_reverse_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator--(int not_used) const noexcept
{
    prefix_const_reverse_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator*()
{
    if (!_base._base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion prefix_const_reverse_iterator implementation

// region infix_iterator implementation
template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::infix_iterator(node* data)
        : _data(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator==(infix_iterator const &other) const noexcept
{
    return _data == other._data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator!=(infix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator++() & noexcept
{
    if (!_data) return *this;

    // Если есть правое поддерево - идем вправо, затем влево до конца
    if (_data->right_subtree)
    {
        _data = _data->right_subtree;
        while (_data->left_subtree)
        {
            _data = _data->left_subtree;
        }
    }
    else // Ищем первого родителя, для которого текущий узел - левый потомок
    {
        node* parent = _data->parent;
        while (parent && _data == parent->right_subtree)
        {
            _data = parent;
            parent = parent->parent;
        }
        _data = parent;
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator++(int not_used) noexcept
{
    infix_iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator--() & noexcept
{
    if (!_data)
    {
        _data = _backup; // Для end() возвращаемся к последнему узлу
        return *this;
    }

    // Если есть левое поддерево - идем влево, затем вправо до конца
    if (_data->left_subtree)
    {
        _data = _data->left_subtree;
        while (_data->right_subtree)
        {
            _data = _data->right_subtree;
        }
    }
    else // Ищем первого родителя, для которого текущий узел - правый потомок
    {
        node* parent = _data->parent;
        while (parent && _data == parent->left_subtree)
        {
            _data = parent;
            parent = parent->parent;
        }
        _data = parent;
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator--(int not_used) const noexcept
{
    infix_iterator temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator*()
{
    if (!_data) throw std::out_of_range("Dereferencing end iterator");
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator->() noexcept
{
    return &_data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::depth() const noexcept
{
    size_t depth = 0;
    node* current = _data;
    while (current && current->parent) {
        ++depth;
        current = current->parent;
    }
    return depth;
}

// endregion infix_iterator implementation

// region infix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::infix_const_iterator(const node* data)
        : _base(data ? infix_iterator(const_cast<node*>(data)) : infix_iterator())
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::infix_const_iterator(const infix_iterator& it) noexcept
        : _base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator==(infix_iterator const &other) const noexcept
{
    return _base == other;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator!=(infix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator++(int not_used) noexcept
{
    infix_const_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator--(int not_used) const noexcept
{
    infix_const_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator*()
{
    if (!_base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::depth() const noexcept
{
    size_t depth = 0;
    const node* current = _base.get_node();
    while (current && current->parent) {
        ++depth;
        current = current->parent;
    }
    return depth;
}

// endregion infix_const_iterator implementation

// region infix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::infix_reverse_iterator(node* data)
        : _base(data)
{
    if (data) {
        while (data->right_subtree) data = data->right_subtree;
        _base = infix_iterator(data);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::infix_reverse_iterator(const infix_iterator& it) noexcept
        : _base(it)
{
    node* current = _base._data;
    if (current) {
        while (current->right_subtree) current = current->right_subtree;
        _base = infix_iterator(current);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator==(infix_reverse_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator!=(infix_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator++(int not_used) noexcept
{
    infix_reverse_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator--(int not_used) const noexcept
{
    infix_reverse_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator*()
{
    if (!_base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::depth() const noexcept
{
    size_t depth = 0;
    node* current = _base._data;
    while (current && current->parent) {
        ++depth;
        current = current->parent;
    }
    return depth;
}

// endregion infix_reverse_iterator implementation

// region infix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::infix_const_reverse_iterator(const node* data)
        : _base(data ? infix_reverse_iterator(const_cast<node*>(data)) : infix_reverse_iterator())
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::infix_const_reverse_iterator(const infix_const_iterator& it) noexcept
        : _base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator() const noexcept
{
    return _base.base();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::base() const noexcept
{
    return _base.base();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator==(infix_const_reverse_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator!=(infix_const_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator++(int not_used) noexcept
{
    infix_const_reverse_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator--(int not_used) const noexcept
{
    infix_const_reverse_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator*()
{
    if (!_base._base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::depth() const noexcept
{
    size_t depth = 0;
    const node* current = _base._base._data;
    while (current && current->parent) {
        ++depth;
        current = current->parent;
    }
    return depth;
}

// endregion infix_const_reverse_iterator implementation

// region postfix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::postfix_iterator(node* data)
        : _data(data), _backup(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator==(postfix_iterator const &other) const noexcept
{
    return _data == other._data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator!=(postfix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator++() & noexcept
{
    if (_data == nullptr)
    {
        return *this;
    }

    node* parent = _data->parent;

    if (parent == nullptr)
    {
        _data = nullptr;
        return *this;
    }

    if (parent->right_subtree == _data)
    {
        _data = parent;
    }
    else
    {
        if (parent->right_subtree != nullptr)
        {
            _data = parent->right_subtree;
            while (_data->left_subtree != nullptr || _data->right_subtree != nullptr)
            {
                while (_data->left_subtree != nullptr)
                {
                    _data = _data->left_subtree;
                }
                if (_data->right_subtree != nullptr)
                {
                    _data = _data->right_subtree;
                }
            }
        }
        else
        {
            _data = parent;
        }
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator++(int not_used) noexcept
{
    postfix_iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator--() & noexcept
{
    if (_data == nullptr)
    {
        _data = _backup;
    }
    else
    {
        _data = _data->parent;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator--(int not_used) const noexcept
{
    postfix_iterator temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator*()
{
    if (!_data) throw std::out_of_range("Dereferencing end iterator");
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator->() noexcept
{
    return &_data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::depth() const noexcept
{
    size_t depth = 0;
    node* current = _data;
    while (current != nullptr)
    {
        current = current->parent;
        ++depth;
    }
    return depth - 1;
}

// endregion postfix_iterator implementation

// region postfix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::postfix_const_iterator(const node* data)
        : _base(const_cast<node*>(data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::postfix_const_iterator(const postfix_iterator& it) noexcept
        : _base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator==(postfix_iterator const &other) const noexcept
{
    return _base == other;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator!=(postfix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator++(int not_used) noexcept
{
    postfix_const_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator--(int not_used) const noexcept
{
    postfix_const_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator*()
{
    if (!_base._current) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion postfix_const_iterator implementation

// region postfix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::postfix_reverse_iterator(node* data)
        : _base(data)
{
    if (data) {
        _base.build_reverse_path(data);
        if (!_base._path.empty()) {
            _base._data = _base._path.top();
            _base._path.pop();
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::postfix_reverse_iterator(const postfix_iterator& it) noexcept
        : _base(it)
{
    if (_base._data) {
        _base.build_reverse_path(_base._data);
        if (!_base._path.empty()) {
            _base._data = _base._path.top();
            _base._path.pop();
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator==(postfix_reverse_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator!=(postfix_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator++(int not_used) noexcept
{
    postfix_reverse_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator--(int not_used) const noexcept
{
    postfix_reverse_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator*()
{
    if (!_base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion postfix_reverse_iterator implementation

// region postfix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(const node* data)
        : _base(data)
{
    if (data) {
        auto& mutable_base = const_cast<postfix_const_iterator&>(_base);
        mutable_base.build_reverse_path(const_cast<node*>(data));
        if (!mutable_base._path.empty()) {
            mutable_base._data = mutable_base._path.top();
            mutable_base._path.pop();
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(const postfix_const_iterator& it) noexcept
        : _base(it)
{
    auto& mutable_base = const_cast<postfix_const_iterator&>(_base);
    if (mutable_base._data) {
        mutable_base.build_reverse_path(const_cast<node*>(mutable_base._data));
        if (!mutable_base._path.empty()) {
            mutable_base._data = mutable_base._path.top();
            mutable_base._path.pop();
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator==(postfix_const_reverse_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator!=(postfix_const_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator++(int not_used) noexcept
{
    postfix_const_reverse_iterator temp = *this;
    --_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator--(int not_used) const noexcept
{
    postfix_const_reverse_iterator temp = *this;
    ++_base;
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator*()
{
    if (!_base._data) throw std::out_of_range("Dereferencing end iterator");
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion postfix_const_reverse_iterator implementation

// region binary_search_tree implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        const compare& comp,
        pp_allocator<value_type> alloc,
        logger *logger)
        : compare(comp),
          _root(nullptr),
          _logger(logger),
          _size(0),
          _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        pp_allocator<value_type> alloc,
        const compare& comp,
        logger *logger)
        : compare(comp),
          _root(nullptr),
          _logger(logger),
          _size(0),
          _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::ranges::input_range Range>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        Range&& range,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
        : compare(cmp),
          _root(nullptr),
          _logger(logger),
          _size(0),
          _allocator(alloc)
{
    for (auto&& elem : range)
    {
        insert(std::forward<decltype(elem)>(elem));
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
        : compare(cmp),
          _root(nullptr),
          _logger(logger),
          _size(0),
          _allocator(alloc)
{
    for (const auto& pair : data)
    {
        insert(pair); // Вставка элементов из списка инициализации
    }
}

// endregion binary_search_tree implementation

// region binary_search_tree 5_rules implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(const binary_search_tree &other)
        : compare(static_cast<const compare&>(other)),
          _logger(other._logger),
          _allocator(other._allocator.select_on_container_copy_construction()),
          _size(0),
          _root(nullptr)
{
    try
    {
        // Рекурсивное копирование узлов
        auto copy_subtree = [this](auto&& self, node* src, node* parent) -> node*
        {
            if (!src) return nullptr;

            node* new_node = _allocator.template new_object<node>(
                    parent,
                    src->data.first,
                    src->data.second);

            new_node->left_subtree = self(self, src->left_subtree, new_node);
            new_node->right_subtree = self(self, src->right_subtree, new_node);

            return new_node;
        };

        _root = copy_subtree(copy_subtree, other._root, nullptr);
        _size = other._size;
    }
    catch (...)
    {
        clear(); // Удаляем частично скопированные узлы
        throw;
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(binary_search_tree &&other) noexcept
        : compare(std::move(static_cast<compare&&>(other))),
          _logger(std::__exchange(other._logger, nullptr)),
          _allocator(std::move(other._allocator)),
          _root(std::__exchange(other._root, nullptr)),
          _size(std::__exchange(other._size, 0))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>&
binary_search_tree<tkey, tvalue, compare, tag>::operator=(const binary_search_tree &other)
{
    if (this == &other) return *this; // Защита от самоприсваивания

    // Создаем временную копию
    binary_search_tree temp(other);

    // Обмениваем ресурсы с временной копией
    swap(temp);

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>&
binary_search_tree<tkey, tvalue, compare, tag>::operator=(binary_search_tree &&other) noexcept
{
    if (this == &other) return *this;

    // Уничтожаем текущие данные
    clear();

    // Перемещаем ресурсы
    static_cast<compare&>(*this) = std::move(static_cast<compare&&>(other));
    _logger = std::__exchange(other._logger, nullptr);
    _allocator = std::move(other._allocator);
    _root = std::__exchange(other._root, nullptr);
    _size = std::__exchange(other._size, 0);

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::~binary_search_tree()
{
    clear();
}

// endregion binary_search_tree 5_rules implementation

// region binary_search_tree methods_access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue& binary_search_tree<tkey, tvalue, compare, tag>::at(const tkey& key)
{
    node* current = _root;
    while (current)
    {
        if (compare_keys(key, current->data.first))
        {
            current = current->left_subtree;
        }
        else if (compare_keys(current->data.first, key))
        {
            current = current->right_subtree;
        }
        else
        {
            return current->data.second;
        }
    }
    throw std::out_of_range("Key not found in binary_search_tree::at()");
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
const tvalue& binary_search_tree<tkey, tvalue, compare, tag>::at(const tkey& key) const
{
    return const_cast<binary_search_tree*>(this)->at(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue& binary_search_tree<tkey, tvalue, compare, tag>::operator[](const tkey& key)
{
    try
    {
        return at(key);
    }
    catch (const std::out_of_range&)
    {
        auto [it, inserted] = insert(value_type(key, tvalue()));
        return it->second;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue& binary_search_tree<tkey, tvalue, compare, tag>::operator[](tkey&& key)
{
    try
    {
        return at(key);
    }
    catch (const std::out_of_range&)
    {
        auto [it, inserted] = emplace(std::move(key), tvalue());
        return it->second;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::empty() const noexcept
{
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::clear() noexcept
{
    // Рекурсивное удаление узлов
    auto delete_subtree = [this](auto&& self, node* root) -> void
    {
        if (!root) return;

        self(self, root->left_subtree);
        self(self, root->right_subtree);
        _allocator.delete_object(root);
    };

    delete_subtree(delete_subtree, _root);
    _root = nullptr;
    _size = 0;
}

// endregion binary_search_tree methods_access implementation

// region binary_search_tree methods_insert and methods_emplace implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::insert(const value_type& value)
{
    return emplace(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::insert(value_type&& value)
{
    return emplace(std::move(value));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::input_iterator InputIt>
void binary_search_tree<tkey, tvalue, compare, tag>::insert(InputIt first, InputIt last)
{
    for (; first != last; ++first)
        emplace(*first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::ranges::input_range R>
void binary_search_tree<tkey, tvalue, compare, tag>::insert_range(R&& rg)
{
    for (auto&& elem : rg)
        emplace(std::forward<decltype(elem)>(elem));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::emplace(Args&&... args)
{
    node* current = _root;
    node* parent = nullptr;
    const tkey& key = value_type(std::forward<Args>(args)...).first;
    node* new_node;

    while (current)
    {
        parent = current;
        if (compare_keys(key, current->data.first))
            current = current->left_subtree;
        else if (compare_keys(current->data.first, key))
            current = current->right_subtree;
        else
            return {infix_iterator(current), false};
    }

    try
    {
        new_node = _allocator.template new_object<node>(
                parent,
                std::forward<Args>(args)...);

        if (!parent)
            _root = new_node;
        else if (compare_keys(key, parent->data.first))
            parent->left_subtree = new_node;
        else
            parent->right_subtree = new_node;
        ++_size;
        return {infix_iterator(new_node), true};
    }
    catch (...)
    {
        // Откат изменений при исключении
        if (parent)
            (parent->left_subtree == new_node ? parent->left_subtree : parent->right_subtree) = nullptr;
        _allocator.delete_object(new_node);
        throw;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(const value_type& value)
{
    return emplace_or_assign(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(value_type&& value)
{
    return emplace_or_assign(std::move(value));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::input_iterator InputIt>
void binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(InputIt first, InputIt last)
{
    for (; first != last; ++first)
        emplace_or_assign(*first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::emplace_or_assign(Args&&... args)
{
    value_type temp(std::forward<Args>(args)...);
    const tkey& key = temp.first;

    node* current = _root;
    node* parent = nullptr;
    node* new_node;

    while (current)
    {
        parent = current;
        if (compare_keys(key, current->data.first))
            current = current->left_subtree;
        else if (compare_keys(current->data.first, key))
            current = current->right_subtree;
        else
        {
            current->data.second = std::move(temp.second);
            return infix_iterator(current);
        }
    }

    try
    {
        new_node = _allocator.template new_object<node>(
                parent,
                std::move(temp));

        if (!parent)
            _root = new_node;
        else if (compare_keys(key, parent->data.first))
            parent->left_subtree = new_node;
        else
            parent->right_subtree = new_node;

        ++_size;
        return infix_iterator(new_node);
    }
    catch (...)
    {
        if (parent)
        {
            (parent->left_subtree == new_node ?
             parent->left_subtree : parent->right_subtree) = nullptr;
        }
        _allocator.delete_object(new_node);
        throw;
    }
}

// endregion binary_search_tree methods_insert and methods_emplace implementation

// region binary_search_tree swap_method implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::swap(binary_search_tree& other) noexcept
{
    using std::swap;
    swap(static_cast<compare&>(*this), static_cast<compare&>(other));
    swap(_root, other._root);
    swap(_size, other._size);
    swap(_logger, other._logger);
    swap(_allocator, other._allocator);
}

// endregion binary_search_tree swap_method implementation

// region binary_search_tree methods_search and methods_erase implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::contains(const tkey& key) const
{
    node* current = _root;
    while (current)
    {
        if (compare_keys(key, current->data.first))
            current = current->left_subtree;
        else if (compare_keys(current->data.first, key))
            current = current->right_subtree;
        else
            return true;
    }
    return false;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::find(const tkey& key)
{
    node* current = _root;

    while (current != nullptr)
    {
        if (compare_keys(key, current->data.first)) // key < current
        {
            current = current->left_subtree;
        }
        else if (compare_keys(current->data.first, key)) // current < key
        {
            current = current->right_subtree;
        }
        else // key == current
        {
            return infix_iterator(current);
        }
    }

    return end(); // key not found
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::find(const tkey& key) const
{
    return const_cast<binary_search_tree*>(this)->find(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::lower_bound(const tkey& key)
{
    node* current = _root;
    node* result = nullptr;

    while (current)
    {
        if (!compare_keys(current->data.first, key))
        {
            result = current;
            current = current->left_subtree;
        }
        else
            current = current->right_subtree;
    }
    return infix_iterator(result);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::lower_bound(const tkey& key) const
{
    node* current = _root;
    node* result = nullptr;

    while (current)
    {
        if (compare_keys(key, current->data.first))
        {
            result = current;
            current = current->left_subtree;
        }
        else
        {
            current = current->right_subtree;
        }
    }
    return infix_iterator(result);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::upper_bound(const tkey& key)
{
    node* current = _root;
    node* result = nullptr;

    while (current)
    {
        if (compare_keys(key, current->data.first))
        {
            result = current;
            current = current->left_subtree;
        }
        else
        {
            current = current->right_subtree;
        }
    }
    return infix_iterator(result);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::upper_bound(const tkey& key) const
{
    return infix_const_iterator(
            const_cast<binary_search_tree*>(this)->upper_bound(key)
    );
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_iterator pos)
{
    if (pos == end()) return end();

    node* node_to_delete = pos.get_node();
    infix_iterator next_it(pos);
    ++next_it;

    using allocator_traits = std::allocator_traits<pp_allocator<value_type>>;
    using node_allocator = typename allocator_traits::template rebind_alloc<node>;
    node_allocator node_alloc(_allocator);

    // Случай 1: Узел - лист
    if (!node_to_delete->left_subtree && !node_to_delete->right_subtree)
    {
        if (node_to_delete->parent)
        {
            if (node_to_delete->parent->left_subtree == node_to_delete)
                node_to_delete->parent->left_subtree = nullptr;
            else
                node_to_delete->parent->right_subtree = nullptr;
        }
        else
            _root = nullptr;

        allocator_traits::destroy(_allocator, &node_to_delete->data);
        node_alloc.deallocate(node_to_delete, 1);
        --_size;
    }
        // Случай 2: Один потомок
    else if (!node_to_delete->left_subtree || !node_to_delete->right_subtree)
    {
        node* child = node_to_delete->left_subtree ? node_to_delete->left_subtree : node_to_delete->right_subtree;

        if (node_to_delete->parent)
        {
            if (node_to_delete->parent->left_subtree == node_to_delete)
                node_to_delete->parent->left_subtree = child;
            else
                node_to_delete->parent->right_subtree = child;
        }
        else
            _root = child;

        if (child)
            child->parent = node_to_delete->parent;

        allocator_traits::destroy(_allocator, &node_to_delete->data);
        node_alloc.deallocate(node_to_delete, 1);
        --_size;
    }
        // Случай 3: Два потомка
    else
    {
        // Находим преемника (наибольший в левом поддереве)
        node* successor = node_to_delete->left_subtree;
        while (successor->right_subtree)
            successor = successor->right_subtree;

        // Копируем данные преемника в удаляемый узел
        const_cast<tkey&>(node_to_delete->data.first) = successor->data.first;
        node_to_delete->data.second = successor->data.second;

        // Удаляем преемника, который теперь имеет не более одного потомка
        node* parent = successor->parent;
        if (parent->left_subtree == successor)
        {
            parent->left_subtree = successor->left_subtree;
            if (successor->left_subtree)
                successor->left_subtree->parent = parent;
        }
        else
        {
            parent->right_subtree = successor->left_subtree;
            if (successor->left_subtree)
                successor->left_subtree->parent = parent;
        }

        allocator_traits::destroy(_allocator, &successor->data);
        node_alloc.deallocate(successor, 1);
        --_size;
    }

    return next_it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_const_iterator pos)
{
    return erase(infix_iterator(const_cast<node*>(pos._data)));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_iterator first, infix_iterator last)
{
    while (first != last)
        first = erase(first);
    return last;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_const_iterator first, infix_const_iterator last)
{
    infix_iterator next;
    while (first != last)
    {
        next = std::next(first);
        erase(first);
        first = next;
    }
    return last;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::erase(const tkey& key)
{
    infix_iterator it = find(key);
    if (it == end()) return 0;

    node* node_to_delete = it.get_node();

    // Находим node** через обход дерева
    node** node_ptr = &_root;
    node* parent = nullptr;

    while (*node_ptr != node_to_delete)
    {
        parent = *node_ptr;
        // Используем compare_keys вместо _comparator
        if (this->compare_keys(key, (*node_ptr)->data.first))
            node_ptr = &(*node_ptr)->left_subtree;
        else
            node_ptr = &(*node_ptr)->right_subtree;
    }

    // Вызываем тег-специфичную реализацию
    __detail::bst_impl<tkey, tvalue, compare, tag>::erase(*this, node_ptr);

    --_size;
    return 1;
}

// endregion binary_search_tree methods_search and methods_erase implementation

// region infix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin() noexcept
{
    node* current = _root;
    while (current && current->left_subtree) {
        current = current->left_subtree;
    }
    return infix_iterator(current);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end() noexcept
{
    return infix_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin() const noexcept
{
    return cbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end() const noexcept
{
    return cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin() const noexcept
{
    const node* current = _root;
    while (current && current->left_subtree) {
        current = current->left_subtree;
    }
    return infix_const_iterator(current);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend() const noexcept
{
    return infix_const_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin() noexcept
{
    node* current = _root;
    while (current && current->right_subtree) {
        current = current->right_subtree;
    }
    return infix_reverse_iterator(current);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend() noexcept
{
    return infix_reverse_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin() const noexcept
{
    return crbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend() const noexcept
{
    return crend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin() const noexcept
{
    const node* current = _root;
    while (current && current->right_subtree) {
        current = current->right_subtree;
    }
    return infix_const_reverse_iterator(current);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend() const noexcept
{
    return infix_const_reverse_iterator(nullptr);
}

// endregion infix_iterators requests implementation

// region prefix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_prefix() noexcept
{
    return prefix_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_prefix() noexcept
{
    return prefix_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_prefix() const noexcept
{
    return prefix_const_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_prefix() const noexcept
{
    return prefix_const_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_prefix() const noexcept
{
    return begin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_prefix() const noexcept
{
    return end_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_prefix() noexcept
{
    return prefix_reverse_iterator(end_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_prefix() noexcept
{
    return prefix_reverse_iterator(begin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_prefix() const noexcept
{
    return prefix_const_reverse_iterator(end_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_prefix() const noexcept
{
    return prefix_const_reverse_iterator(begin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_prefix() const noexcept
{
    return rbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_prefix() const noexcept
{
    return rend_prefix();
}

// endregion prefix_iterators requests implementation

// region infix_iterators methods implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_infix() noexcept
{
    return begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_infix() noexcept
{
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_infix() const noexcept
{
    return cbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_infix() const noexcept
{
    return cend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_infix() const noexcept
{
    return cbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_infix() const noexcept
{
    return cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_infix() noexcept
{
    return rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_infix() noexcept
{
    return rend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_infix() const noexcept
{
    return crbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_infix() const noexcept
{
    return crend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_infix() const noexcept
{
    return crbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_infix() const noexcept
{
    return crend();
}

// endregion infix_iterators methods implementation

// region postfix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_postfix() noexcept
{
    node* current = _root;
    std::stack<node*> stack;
    node* last_visited = nullptr;

    while (!stack.empty() || current) {
        if (current) {
            stack.push(current);
            current = current->left_subtree;
        } else {
            node* peek = stack.top();
            if (peek->right_subtree && last_visited != peek->right_subtree) {
                current = peek->right_subtree;
            } else {
                last_visited = stack.top();
                stack.pop();
                return postfix_iterator(last_visited);
            }
        }
    }
    return end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_postfix() noexcept
{
    return postfix_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_postfix() const noexcept
{
    return postfix_const_iterator(const_cast<binary_search_tree*>(this)->begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_postfix() const noexcept
{
    return postfix_const_iterator(nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_postfix() const noexcept
{
    return begin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_postfix() const noexcept
{
    return end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_postfix() noexcept
{
    return postfix_reverse_iterator(end_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_postfix() noexcept
{
    return postfix_reverse_iterator(begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_postfix() const noexcept
{
    return postfix_const_reverse_iterator(end_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_postfix() const noexcept
{
    return postfix_const_reverse_iterator(begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_postfix() const noexcept
{
    return rbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_postfix() const noexcept
{
    return rend_postfix();
}

// endregion postfix_iterators requests implementation

// endregion iterators requests implementation

//region subtree rotations implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::small_left_rotation(node *&subtree_root) noexcept
{
    node* right_child = subtree_root->right_subtree;
    subtree_root->right_subtree = right_child->left_subtree;

    if (subtree_root->right_subtree) {
        subtree_root->right_subtree->parent = subtree_root;
    }

    right_child->left_subtree = subtree_root;
    right_child->parent = subtree_root->parent;
    subtree_root->parent = right_child;

    subtree_root = right_child;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::small_right_rotation(node *&subtree_root) noexcept
{
    node* left_child = subtree_root->left_subtree;
    subtree_root->left_subtree = left_child->right_subtree;

    if (subtree_root->left_subtree) {
        subtree_root->left_subtree->parent = subtree_root;
    }

    left_child->right_subtree = subtree_root;
    left_child->parent = subtree_root->parent;
    subtree_root->parent = left_child;

    subtree_root = left_child;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::big_left_rotation(node *&subtree_root) noexcept
{
    small_right_rotation(subtree_root->right_subtree);
    small_left_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::big_right_rotation(node *&subtree_root) noexcept
{
    small_left_rotation(subtree_root->left_subtree);
    small_right_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::double_left_rotation(node *&subtree_root) noexcept
{
    small_right_rotation(subtree_root->right_subtree);
    small_left_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::double_right_rotation(node *&subtree_root) noexcept
{
    small_left_rotation(subtree_root->left_subtree);
    small_right_rotation(subtree_root);
}

//endregion subtree rotations implementation

namespace __detail {
    template<typename tkey, typename tvalue, typename compare, typename tag>
    template<typename ...Args>
    typename binary_search_tree<tkey, tvalue, compare, tag>::node*
    bst_impl<tkey, tvalue, compare, tag>::create_node(binary_search_tree<tkey, tvalue, compare, tag>& cont, Args&& ...args)
    {
        using node_type = typename binary_search_tree<tkey, tvalue, compare, tag>::node;
        return cont._allocator.template new_object<node_type>(
                nullptr,
                std::forward<Args>(args)...);
    }

    template<typename tkey, typename tvalue, typename compare, typename tag>
    void bst_impl<tkey, tvalue, compare, tag>::delete_node(binary_search_tree<tkey, tvalue, compare, tag>& cont)
    {
        cont._allocator.delete_object(cont);
    }

    template<typename tkey, typename tvalue, typename compare, typename tag>
    void bst_impl<tkey, tvalue, compare, tag>::erase(binary_search_tree<tkey, tvalue, compare, tag>& cont, typename binary_search_tree<tkey, tvalue, compare, tag>::node** node_ptr)
    {
        using node_type = typename binary_search_tree<tkey, tvalue, compare, tag>::node;
        node_type* node = *node_ptr;

        if (!node->left_subtree && !node->right_subtree) {
            // Удаление листа
            if (node->parent) {
                (node->parent->left_subtree == node ?
                 node->parent->left_subtree : node->parent->right_subtree) = nullptr;
            }
            *node_ptr = nullptr;
        } else if (!node->left_subtree || !node->right_subtree) {
            // Удаление узла с одним потомком
            node_type* child = node->left_subtree ? node->left_subtree : node->right_subtree;
            if (node->parent) {
                (node->parent->left_subtree == node ?
                 node->parent->left_subtree : node->parent->right_subtree) = child;
            }
            child->parent = node->parent;
            *node_ptr = child;
        } else {
            // Удаление узла с двумя потомками
            node_type* successor = node->left_subtree;
            while (successor->right_subtree) successor = successor->right_subtree;

            // Меняем и ключ, и значение через const_cast
            const_cast<tkey&>(node->data.first) = successor->data.first;
            const_cast<tvalue&>(node->data.second) = std::move(successor->data.second);

            erase(cont, &successor);
            return;
        }

        // Освобождение памяти
        using value_allocator = pp_allocator<typename binary_search_tree<tkey, tvalue, compare, tag>::value_type>;
        using node_allocator = typename std::allocator_traits<value_allocator>::template rebind_alloc<node_type>;

        node_allocator alloc(cont._allocator);
        std::allocator_traits<node_allocator>::destroy(alloc, node);
        std::allocator_traits<node_allocator>::deallocate(alloc, node, 1);

        cont._size--;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::node*
binary_search_tree<tkey, tvalue, compare, tag>::find_last_node(node* root) noexcept
{
    while (root && root->right_subtree)
        root = root->right_subtree;
    return root;
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H