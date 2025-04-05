#ifndef MP_OS_WORKBENCH_HASH_TABLE_H
#define MP_OS_WORKBENCH_HASH_TABLE_H

#include <associative_container.h>
#include <not_implemented.h>
#include <vector>
#include <logger.h>
#include <map>
#include <pp_allocator.h>
#include <ranges>

// TODO: concept search_ds
template<typename T, typename tkey, typename tvalue>
concept search_ds_for = std::same_as<typename T::value_type, std::pair<const tkey, tvalue>> && std::copy_constructible<std::pair<const tkey, tvalue>> && std::copy_constructible<T> && std::ranges::forward_range<T> &&
            requires (T t, tkey key, tvalue val)
            {
                {t.insert(std::make_pair(key, val)).first} -> std::forward_iterator;
                {t.insert(std::make_pair(key, val)).second} -> std::same_as<bool>;
                {t.find(key)} -> std::forward_iterator;
                {t.erase(key)} -> std::forward_iterator;
            };

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds = std::map<tkey, tvalue, std::less<tkey>, pp_allocator<std::pair<const tkey, tvalue>>>, typename hash = std::hash<tkey>>
class hash_table final
{
    template< std::ranges::range R >
    using const_iterator_t = decltype(std::ranges::cbegin(std::declval<R&>()));
public:

    using value_type = std::pair<const tkey, tvalue>;
    using sds_it = std::ranges::iterator_t<sds>;
    using sds_cit = const_iterator_t<sds>;

private:

    using vector = std::vector<sds, pp_allocator<sds>>;

    vector _buckets;
    pp_allocator<value_type> _alloc;
    size_t _size;
    size_t _used_buckets;
    double _max_load_factor;
    logger* _logger;

public:

    // region constructors declaration

    explicit hash_table(pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // template<input_iterator_for_pair<tkey, tvalue> iterator>
    // explicit hash_table(iterator begin, iterator end, pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    hash_table(std::initializer_list<std::pair<tkey, tvalue>> data, pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    hash_table(const hash_table& other);

    hash_table(hash_table&& other) noexcept;

    hash_table& operator=(const hash_table& other);

    hash_table& operator=(hash_table&& other) noexcept;

    ~hash_table() noexcept;

    // endregion five declaration

    // region iterator

    class iterator final
    {
        vector::iterator _bucket_it;
        vector::iterator _bucket_sentinel;

        sds_it _data_it;
        sds_it _data_sentinel;

    public:

        using value_type = std::pair<const tkey, tvalue>;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type *;
        using iterator_category = std::forward_iterator_tag;
        using self = iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        explicit iterator(vector::iterator begin = vector::iterator(), vector::iterator end = vector::iterator());

        /** data_beg MUST be part of begin iterator, UB otherwise
         */
        explicit iterator(vector::iterator begin = vector::iterator(), vector::iterator end = vector::iterator(), sds_it data_beg = sds_it());

    };

    class const_iterator final
    {
        vector::const_iterator _bucket_it;
        vector::const_iterator _bucket_sentinel;

        sds_cit _data_it;
        sds_cit _data_sentinel;

    public:

        using value_type = std::pair<const tkey, tvalue>;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type *;
        using iterator_category = std::forward_iterator_tag;
        using self = const_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;


        const_iterator(const iterator&);
        explicit const_iterator(vector::const_iterator begin = vector::const_iterator(), vector::const_iterator end = vector::const_iterator());

        /** data_beg MUST be part of begin iterator, UB otherwise
         */
        explicit const_iterator(vector::const_iterator begin = vector::const_iterator(), vector::const_iterator end = vector::const_iterator(), sds_cit data_beg = sds_cit());
    };

    friend class iterator;
    friend class const_iterator;

    // endregion iterator

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

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    const_iterator cbegin();
    const_iterator cend();

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    iterator find(const tkey& key);
    const_iterator find(const tkey& key) const;

    iterator lower_bound(const tkey& key);
    const_iterator lower_bound(const tkey& key) const;

    iterator upper_bound(const tkey& key);
    const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<iterator, bool> insert(const value_type & data);
    std::pair<iterator, bool> insert(value_type && data);

    template <typename ...Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    iterator insert_or_assign(const value_type & data);
    iterator insert_or_assign(value_type && data);

    template <typename ...Args>
    iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);

    iterator erase(iterator beg, iterator en);
    iterator erase(const_iterator beg, const_iterator en);


    iterator erase(const tkey& key);

    // endregion modifiers declaration

    // region hash policy

    double max_load_factor() const;
    void max_load_factor(double ml);

    double load_factor() const;

    void rehash(size_t count);
    void reserve(size_t count);

    // endregion hash policy
};

// region constructors implementation
template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::hash_table(pp_allocator<value_type> allocator, logger* logger)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::hash_table(pp_allocator<value_type>, logger*)", "your code should be here...");
}

// template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
// template<input_iterator_for_pair<tkey, tvalue> iterator>
// hash_table<tkey, tvalue, sds, hash>::hash_table(iterator begin, iterator end, pp_allocator<value_type> allocator, logger* logger)
// {
//     throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> template<input_iterator_for_pair<tkey, tvalue> iterator> hash_table<tkey, tvalue, sds, hash>::hash_table(iterator , iterator , pp_allocator<value_type>, logger*)", "your code should be here...");
// }

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::hash_table(std::initializer_list<std::pair<tkey, tvalue>> data, pp_allocator<value_type> allocator, logger* logger)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::hash_table(std::initializer_list<std::pair<tkey, tvalue>>, pp_allocator<value_type>, logger* )", "your code should be here...");
}

// endregion constructors implementation

// region five implementation
template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::hash_table(const hash_table& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::hash_table(const hash_table&)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::hash_table(hash_table&& other) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::hash_table(hash_table&&) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>& hash_table<tkey, tvalue, sds, hash>::operator=(const hash_table& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>& hash_table<tkey, tvalue, sds, hash>::operator=(const hash_table&)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>& hash_table<tkey, tvalue, sds, hash>::operator=(hash_table&& other) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>& hash_table<tkey, tvalue, sds, hash>::operator=(hash_table&&) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::~hash_table() noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::~hash_table() noexcept", "your code should be here...");
}

// endregion five implementation

// region iterator implementation
template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator& hash_table<tkey, tvalue, sds, hash>::iterator::operator++()
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator& hash_table<tkey, tvalue, sds, hash>::iterator::operator++()", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::iterator::operator++(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::iterator::operator++(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator::reference hash_table<tkey, tvalue, sds, hash>::iterator::operator*() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator::reference hash_table<tkey, tvalue, sds, hash>::iterator::operator*() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator::pointer hash_table<tkey, tvalue, sds, hash>::iterator::operator->() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator::pointer hash_table<tkey, tvalue, sds, hash>::iterator::operator->() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
bool hash_table<tkey, tvalue, sds, hash>::iterator::operator==(const iterator& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> bool hash_table<tkey, tvalue, sds, hash>::iterator::operator==(const iterator& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
bool hash_table<tkey, tvalue, sds, hash>::iterator::operator!=(const iterator& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> bool hash_table<tkey, tvalue, sds, hash>::iterator::operator!=(const iterator& other) const noexcept", "your code should be here...");
}

// const iterator methods
template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator& hash_table<tkey, tvalue, sds, hash>::const_iterator::operator++()
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator& hash_table<tkey, tvalue, sds, hash>::const_iterator::operator++()", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::const_iterator::operator++(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::const_iterator::operator++(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator::reference hash_table<tkey, tvalue, sds, hash>::const_iterator::operator*() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator::reference hash_table<tkey, tvalue, sds, hash>::const_iterator::operator*() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator::pointer hash_table<tkey, tvalue, sds, hash>::const_iterator::operator->() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator::pointer hash_table<tkey, tvalue, sds, hash>::const_iterator::operator->() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
bool hash_table<tkey, tvalue, sds, hash>::const_iterator::operator==(const const_iterator& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> bool hash_table<tkey, tvalue, sds, hash>::const_iterator::operator==(const const_iterator& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
bool hash_table<tkey, tvalue, sds, hash>::const_iterator::operator!=(const const_iterator& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> bool hash_table<tkey, tvalue, sds, hash>::const_iterator::operator!=(const const_iterator& other) const noexcept", "your code should be here...");
}

// constructors iterators
template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::iterator::iterator(vector::iterator begin, vector::iterator end)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::iterator::iterator(vector::iterator , vector::iterator )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::iterator::iterator(vector::iterator begin, vector::iterator end, sds_it data_beg)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::iterator::iterator(vector::iterator begin, vector::iterator end, sds_it data_beg)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::const_iterator::const_iterator(vector::const_iterator begin, vector::const_iterator end)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::const_iterator::const_iterator(vector::const_iterator , vector::const_iterator )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::const_iterator::const_iterator(vector::const_iterator begin, vector::const_iterator end, sds_cit data_beg)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::const_iterator::const_iterator(vector::const_iterator begin, vector::const_iterator end, sds_cit data_beg)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
hash_table<tkey, tvalue, sds, hash>::const_iterator::const_iterator(const iterator& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> hash_table<tkey, tvalue, sds, hash>::const_iterator::const_iterator(const iterator&)", "your code should be here...");
}

// endregion iterator implementation

// region element access implementation

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
tvalue& hash_table<tkey, tvalue, sds, hash>::at(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> tvalue& hash_table<tkey, tvalue, sds, hash>::at(const tkey& )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
const tvalue& hash_table<tkey, tvalue, sds, hash>::at(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> const tvalue& hash_table<tkey, tvalue, sds, hash>::at(const tkey&) const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
tvalue& hash_table<tkey, tvalue, sds, hash>::operator[](const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> tvalue& hash_table<tkey, tvalue, sds, hash>::operator[](const tkey&)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
tvalue& hash_table<tkey, tvalue, sds, hash>::operator[](tkey&& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> tvalue& hash_table<tkey, tvalue, sds, hash>::operator[](tkey&&)", "your code should be here...");
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::begin()
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::begin()", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::end()
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::end()", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::begin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::begin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::end() const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::end() const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cbegin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cbegin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cend() const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cend() const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cbegin()
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cbegin()", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cend()
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::cend()", "your code should be here...");
}

// endregion iterator begins implementation

// region lookup implementation

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
size_t hash_table<tkey, tvalue, sds, hash>::size() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> size_t hash_table<tkey, tvalue, sds, hash>::size() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
bool hash_table<tkey, tvalue, sds, hash>::empty() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> bool hash_table<tkey, tvalue, sds, hash>::empty() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::find(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::find(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::find(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::find(const tkey& key) const", "your code should be here...");
}


template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::lower_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::lower_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::lower_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::lower_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::upper_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::upper_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::upper_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::const_iterator hash_table<tkey, tvalue, sds, hash>::upper_bound(const tkey&) const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
bool hash_table<tkey, tvalue, sds, hash>::contains(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> bool hash_table<tkey, tvalue, sds, hash>::contains(const tkey& ) const", "your code should be here...");
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
void hash_table<tkey, tvalue, sds, hash>::clear() noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> void hash_table<tkey, tvalue, sds, hash>::clear() noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
std::pair<typename hash_table<tkey, tvalue, sds, hash>::iterator, bool> hash_table<tkey, tvalue, sds, hash>::insert(const value_type& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> std::pair<typename hash_table<tkey, tvalue, sds, hash>::iterator, bool> hash_table<tkey, tvalue, sds, hash>::insert(const value_type& data)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
std::pair<typename hash_table<tkey, tvalue, sds, hash>::iterator, bool> hash_table<tkey, tvalue, sds, hash>::insert(value_type&& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> std::pair<typename hash_table<tkey, tvalue, sds, hash>::iterator, bool> hash_table<tkey, tvalue, sds, hash>::insert(value_type&&)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
template<typename ...Args>
std::pair<typename hash_table<tkey, tvalue, sds, hash>::iterator, bool> hash_table<tkey, tvalue, sds, hash>::emplace(Args&&... args)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> template<typename ...Args> std::pair<typename hash_table<tkey, tvalue, sds, hash>::iterator, bool> hash_table<tkey, tvalue, sds, hash>::emplace(Args&&... args)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::insert_or_assign(const value_type & data)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::insert_or_assign(const value_type &)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::insert_or_assign(value_type && data)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::insert_or_assign(value_type &&)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
template<typename ...Args>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::emplace_or_assign(Args&&... args)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> template<typename ...Args> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::emplace_or_assign(Args&&... args)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(iterator pos)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(iterator )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(const_iterator pos)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(const_iterator)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(iterator beg, iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(iterator , iterator )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(const_iterator beg, const_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(const_iterator , const_iterator )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(const tkey& key)
{
    throw not_implemented("emplate<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> typename hash_table<tkey, tvalue, sds, hash>::iterator hash_table<tkey, tvalue, sds, hash>::erase(const tkey&)", "your code should be here...");
}

// endregion modifiers implementation

// region hash policy implementation

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
double hash_table<tkey, tvalue, sds, hash>::max_load_factor() const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> double hash_table<tkey, tvalue, sds, hash>::max_load_factor() const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
void hash_table<tkey, tvalue, sds, hash>::max_load_factor(double ml)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> void hash_table<tkey, tvalue, sds, hash>::max_load_factor(double )", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
double hash_table<tkey, tvalue, sds, hash>::load_factor() const
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> double hash_table<tkey, tvalue, sds, hash>::load_factor() const", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
void hash_table<tkey, tvalue, sds, hash>::rehash(size_t count)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> void hash_table<tkey, tvalue, sds, hash>::rehash(size_t count)", "your code should be here...");
}

template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash>
void hash_table<tkey, tvalue, sds, hash>::reserve(size_t count)
{
    throw not_implemented("template<typename tkey, typename tvalue, search_ds_for<tkey, tvalue> sds, typename hash> void hash_table<tkey, tvalue, sds, hash>::reserve(size_t )", "your code should be here...");
}

// endregion hash policy implementation

#endif //MP_OS_WORKBENCH_HASH_TABLE_H