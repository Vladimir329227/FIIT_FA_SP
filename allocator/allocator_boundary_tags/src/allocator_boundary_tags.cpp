#include <not_implemented.h>
#include <algorithm>
#include "../include/allocator_boundary_tags.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <tuple>

allocator_boundary_tags::~allocator_boundary_tags()
{
    trace_with_guard("[ALLOC] Destructor called");
    debug_with_guard("~allocator_boundary_tags");
    auto parent = get_parent_allocator();
    size_t total_size = allocator_metadata_size + get_space_size();
    if (parent)
        parent->deallocate(_trusted_memory, total_size);
    else
        ::operator delete(_trusted_memory);
}

allocator_boundary_tags::allocator_boundary_tags(
        allocator_boundary_tags &&other) noexcept
        : _trusted_memory(other._trusted_memory)
{
    debug_with_guard("allocator_boundary_tags(allocator_boundary_tags &&other)");
    other._trusted_memory = nullptr;
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
        allocator_boundary_tags &&other) noexcept
{
    if (this == &other) return *this;

    this->~allocator_boundary_tags();
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
    return *this;
}


/** If parent_allocator* == nullptr you should use std::pmr::get_default_resource()
 */
allocator_boundary_tags::allocator_boundary_tags(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *log,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    try {
        size_t total_size = allocator_metadata_size + space_size;
        void *memory;

        if (parent_allocator != nullptr)
        {
            memory = parent_allocator->allocate(total_size);
        }
        else
        {
            memory = ::operator new(total_size);
        }

        _trusted_memory = memory;

        char *ptr = static_cast<char*>(memory);

        // Инициализация метаданных
        *reinterpret_cast<logger**>(ptr) = log;
        ptr += sizeof(logger*);

        *reinterpret_cast<std::pmr::memory_resource**>(ptr) = parent_allocator;
        ptr += sizeof(std::pmr::memory_resource*);

        *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = allocate_fit_mode;
        ptr += sizeof(allocator_with_fit_mode::fit_mode);

        *reinterpret_cast<size_t*>(ptr) = space_size;
        ptr += sizeof(size_t);

        new (ptr) std::mutex();
        ptr += sizeof(std::mutex);

        *reinterpret_cast<void**>(ptr) = nullptr; // Первый блок
        debug_with_guard("allocator_boundary_tags");
    }
    catch (const std::bad_alloc& e)
    {
        error_with_guard("[ALLOC] Failed to allocator_boundary_tags " + std::to_string(space_size));
        throw;
    }
    debug_with_guard("[ALLOC] Exiting allocator_boundary_tags");
}

[[nodiscard]] void *allocator_boundary_tags::do_allocate_sm(
        size_t size)
{
    if (size == 0)
        warning_with_guard("[ALLOC] Zero-size allocation request");

    debug_with_guard("[ALLOC] Entering do_allocate_sm, bytes: " + std::to_string(size));
    std::lock_guard<std::mutex> lock(get_mutex());
    try {

        size_t required_size = occupied_block_metadata_size + size;
        char *alloc_start = static_cast<char *>(_trusted_memory) + allocator_metadata_size;
        char *alloc_end = alloc_start + get_space_size();
        std::vector<std::pair<void *, size_t>> free_blocks;

        // Собираем свободные блоки
        std::vector<void *> blocks;
        for (auto it = begin(); it != end(); ++it)
            blocks.push_back(*it);
        std::sort(blocks.begin(), blocks.end());

        if (!blocks.empty()) {
            char *first = static_cast<char *>(blocks.front());
            if (first > alloc_start) {
                size_t free_size = first - alloc_start;
                free_blocks.emplace_back(alloc_start, free_size);
            }
        } else {
            free_blocks.emplace_back(alloc_start, alloc_end - alloc_start);
        }

        // Проверяем промежутки между блоками
        for (size_t i = 0; i < blocks.size(); ++i) {
            void *current = blocks[i];
            size_t current_size = *reinterpret_cast<size_t *>(static_cast<char *>(current) + 3 * sizeof(void *));
            void *current_end = static_cast<char *>(current) + current_size;

            if (i < blocks.size() - 1) {
                void *next = blocks[i + 1];
                if (next > current_end)
                    free_blocks.emplace_back(current_end, static_cast<char *>(next) - static_cast<char *>(current_end));
            } else {
                if (current_end < alloc_end)
                    free_blocks.emplace_back(current_end,
                                             static_cast<char *>(alloc_end) - static_cast<char *>(current_end));
            }
        }

        allocator_with_fit_mode::fit_mode mode = get_fit_mode();
        std::pair<void *, size_t> selected{nullptr, 0};
        bool found = false;

        switch (mode) {
            case allocator_with_fit_mode::fit_mode::first_fit:
                for (auto &block: free_blocks)
                    if (block.second >= required_size) {
                        selected = block;
                        found = true;
                        break;
                    }
                break;
            case allocator_with_fit_mode::fit_mode::the_best_fit: {
                size_t min_diff = std::numeric_limits<size_t>::max();
                for (auto &block: free_blocks)
                    if (block.second >= required_size && (block.second - required_size) < min_diff) {
                        selected = block;
                        min_diff = block.second - required_size;
                        found = true;
                    }
                break;
            }
            case allocator_with_fit_mode::fit_mode::the_worst_fit: {
                size_t max_size = 0;
                for (auto &block: free_blocks)
                    if (block.second >= required_size && block.second > max_size) {
                        selected = block;
                        max_size = block.second;
                        found = true;
                    }
                break;
            }
        }

        if (!found) {
            error_with_guard("bad_alloc");
            throw std::bad_alloc();
        }

        void *new_block = selected.first;
        size_t block_size = selected.second;

        *reinterpret_cast<void **>(new_block) = nullptr; // prev
        *reinterpret_cast<void **>(static_cast<char *>(new_block) + sizeof(void *)) = nullptr; // next
        *reinterpret_cast<void **>(static_cast<char *>(new_block) + 2 * sizeof(void *)) = this; // allocator
        *reinterpret_cast<size_t *>(static_cast<char *>(new_block) + 3 * sizeof(void *)) = required_size; // size

        // Вставка в список блоков
        auto it = std::upper_bound(blocks.begin(), blocks.end(), new_block);
        size_t pos = it - blocks.begin();
        void *prev = pos > 0 ? blocks[pos - 1] : nullptr;
        void *next = pos < blocks.size() ? blocks[pos] : nullptr;

        *reinterpret_cast<void **>(new_block) = prev;
        *reinterpret_cast<void **>(static_cast<char *>(new_block) + sizeof(void *)) = next;

        if (prev)
            *reinterpret_cast<void **>(static_cast<char *>(prev) + sizeof(void *)) = new_block;
        else
            set_first_block(new_block);

        if (next)
            *reinterpret_cast<void **>(next) = new_block;

        information_with_guard("[ALLOC] Available memory: " + std::to_string(calculate_available_memory()));
        debug_with_guard("[ALLOC] Memory state:\n" + get_memory_state());


        return static_cast<char *>(new_block) + occupied_block_metadata_size;
    }
    catch (const std::bad_alloc& e)
    {
        error_with_guard("[ALLOC] Failed to allocate " + std::to_string(size) + " bytes: " + e.what());
        throw;
    }
}

void allocator_boundary_tags::do_deallocate_sm(
        void *at)
{
    debug_with_guard("[DEALLOC] Entering do_deallocate_sm");

    if (!at){
        error_with_guard("Err do_deallocate_sm");
        return;
    }



    std::lock_guard<std::mutex> lock(get_mutex());
    try {

        void *block = static_cast<char *>(at) - occupied_block_metadata_size;

        void *block_alloc = *reinterpret_cast<void **>(static_cast<char *>(block) + 2 * sizeof(void *));
        if (block_alloc != this)
        {
            error_with_guard("[DEALLOC] Block does not belong to this allocator");
            throw std::logic_error("Invalid deallocation");
        }

        void *prev = *reinterpret_cast<void **>(block);
        void *next = *reinterpret_cast<void **>(static_cast<char *>(block) + sizeof(void *));

        if (prev)
            *reinterpret_cast<void **>(static_cast<char *>(prev) + sizeof(void *)) = next;
        else
            set_first_block(next);

        if (next)
            *reinterpret_cast<void **>(next) = prev;
        information_with_guard("[DEALLOC] Available memory: " + std::to_string(calculate_available_memory()));
        debug_with_guard("[DEALLOC] Memory state:\n" + get_memory_state());

    }
    catch (const std::exception& e)
    {
        error_with_guard("[DEALLOC] Error: " + std::string(e.what()));
        throw;
    }
    debug_with_guard("[DEALLOC] Exiting do_deallocate_sm");

}

inline void allocator_boundary_tags::set_fit_mode(
        allocator_with_fit_mode::fit_mode mode)
{
    debug_with_guard("set_fit_mode");

    char *ptr = static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(std::pmr::memory_resource*);
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = mode;
}


std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const
{
    std::vector<allocator_test_utils::block_info> info;

    char *memory_start = static_cast<char*>(_trusted_memory) + allocator_metadata_size;
    size_t space_size = get_space_size();
    char *memory_end = memory_start + space_size;

    std::vector<char*> allocated_blocks;
    for (auto it = begin(); it != end(); ++it)
        allocated_blocks.push_back(static_cast<char*>(*it));
    std::sort(allocated_blocks.begin(), allocated_blocks.end());

    char *current = memory_start;

    // Обработка свободного пространства перед первым блоком
    if (!allocated_blocks.empty() && allocated_blocks.front() > memory_start)
    {
        size_t free_size = allocated_blocks.front() - memory_start;
        if (free_size >= occupied_block_metadata_size)
        {
            info.push_back({free_size, false});
        }
        else if (!info.empty())
        {
            info.back().block_size += free_size;
        }
    }

    // Обработка занятых блоков и промежутков между ними
    for (size_t i = 0; i < allocated_blocks.size(); ++i)
    {
        char *block = allocated_blocks[i];
        size_t block_size = *reinterpret_cast<size_t*>(block + 3 * sizeof(void*));
        char *block_end = block + block_size;

        info.push_back({block_size, true});

        if (i < allocated_blocks.size() - 1)
        {
            char *next_block = allocated_blocks[i + 1];
            size_t free_size = next_block - block_end;
            if (free_size >= occupied_block_metadata_size)
            {
                info.push_back({free_size, false});
            }
            else if (!info.empty())
            {
                info.back().block_size += free_size;
            }
        }
    }

    if (!allocated_blocks.empty())
    {
        char *last_block_end = allocated_blocks.back() +
                               *reinterpret_cast<size_t*>(allocated_blocks.back() + 3 * sizeof(void*));
        if (last_block_end < memory_end)
        {
            size_t free_size = memory_end - last_block_end;
            if (free_size >= occupied_block_metadata_size)
            {
                info.push_back({free_size, false});
            }
            else if (!info.empty())
            {
                info.back().block_size += free_size;
            }
        }
    }

    return info;
}

inline logger *allocator_boundary_tags::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "allocator_boundary_tags";
}


allocator_boundary_tags::boundary_iterator allocator_boundary_tags::begin() const noexcept
{
    return boundary_iterator(_trusted_memory);
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::end() const noexcept
{
    return boundary_iterator(nullptr);
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_inner() const
{
    return get_blocks_info();
}

allocator_boundary_tags::allocator_boundary_tags(const allocator_boundary_tags &other)
{
    std::lock_guard<std::mutex> lock(other.get_mutex());

    size_t space_size = other.get_space_size();
    size_t total_size = allocator_metadata_size + space_size;
    auto parent = other.get_parent_allocator();

    if (parent != nullptr)
    {
        _trusted_memory = parent->allocate(total_size);
    }
    else
    {
        _trusted_memory = ::operator new(total_size);
    }

    memcpy(_trusted_memory, other._trusted_memory, total_size);

    char* old_base = static_cast<char*>(other._trusted_memory);
    char* new_base = static_cast<char*>(_trusted_memory);

    void* current_block = get_first_block();
    while (current_block != nullptr)
    {
        void** allocator_ptr = reinterpret_cast<void**>(
                static_cast<char*>(current_block) + 2 * sizeof(void*));
        *allocator_ptr = this;

        void* old_prev = *reinterpret_cast<void**>(current_block);
        void* old_next = *reinterpret_cast<void**>(
                static_cast<char*>(current_block) + sizeof(void*));

        if (old_prev != nullptr)
        {
            ptrdiff_t prev_offset = static_cast<char*>(old_prev) - old_base;
            *reinterpret_cast<void**>(current_block) = new_base + prev_offset;
        }

        if (old_next != nullptr)
        {
            ptrdiff_t next_offset = static_cast<char*>(old_next) - old_base;
            *reinterpret_cast<void**>(
                    static_cast<char*>(current_block) + sizeof(void*)) = new_base + next_offset;
        }

        current_block = *reinterpret_cast<void**>(
                static_cast<char*>(current_block) + sizeof(void*));
    }
}

allocator_boundary_tags &allocator_boundary_tags::operator=(const allocator_boundary_tags &other)
{
    if (this == &other) return *this;

    this->~allocator_boundary_tags();
    new (this) allocator_boundary_tags(other);
    return *this;
}

bool allocator_boundary_tags::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}

bool allocator_boundary_tags::boundary_iterator::operator==(
        const allocator_boundary_tags::boundary_iterator &other) const noexcept
{
    return _occupied_ptr == other._occupied_ptr;
}

bool allocator_boundary_tags::boundary_iterator::operator!=(
        const allocator_boundary_tags::boundary_iterator & other) const noexcept
{
    return !(*this == other);
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator++() & noexcept
{
    if (_occupied_ptr)
        _occupied_ptr = *reinterpret_cast<void**>(static_cast<char*>(_occupied_ptr) + sizeof(void*));
    return *this;
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator--() & noexcept
{
    if (_trusted_memory)
    {
        if (!_occupied_ptr)
        {
            void *current = *reinterpret_cast<void**>(static_cast<char*>(_trusted_memory) + sizeof(logger*) +
                                                      sizeof(std::pmr::memory_resource*) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t) +
                                                      sizeof(std::mutex));
            void *last = nullptr;
            while (current)
            {
                last = current;
                current = *reinterpret_cast<void**>(static_cast<char*>(current) + sizeof(void*));
            }
            _occupied_ptr = last;
        }
        else
        {
            _occupied_ptr = *reinterpret_cast<void**>(_occupied_ptr);
        }
    }
    return *this;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator++(int n)
{
    boundary_iterator tmp = *this;
    ++(*this);
    return tmp;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator--(int n)
{
    boundary_iterator tmp = *this;
    --(*this);
    return tmp;
}

size_t allocator_boundary_tags::boundary_iterator::size() const noexcept
{
    if (_occupied_ptr == nullptr) return 0;

    return *reinterpret_cast<size_t*>(
            static_cast<char*>(_occupied_ptr) + 3 * sizeof(void*));
}

bool allocator_boundary_tags::boundary_iterator::occupied() const noexcept
{
    return true;
}

void* allocator_boundary_tags::boundary_iterator::operator*() const noexcept
{
    return _occupied_ptr;
}

allocator_boundary_tags::boundary_iterator::boundary_iterator()
        : _trusted_memory(nullptr), _occupied_ptr(nullptr) {}

allocator_boundary_tags::boundary_iterator::boundary_iterator(void *trusted)
        : _trusted_memory(trusted)
{
    if (trusted)
    {
        char *base = static_cast<char*>(trusted);
        void **first_block = reinterpret_cast<void**>(base + sizeof(logger*) + sizeof(std::pmr::memory_resource*) +
                                                      sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t) + sizeof(std::mutex));
        _occupied_ptr = *first_block;
    }
    else
        _occupied_ptr = nullptr;
}

void *allocator_boundary_tags::boundary_iterator::get_ptr() const noexcept
{
    return _occupied_ptr;
}

inline std::pmr::memory_resource* allocator_boundary_tags::get_parent_allocator() const
{
    return *reinterpret_cast<std::pmr::memory_resource**>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*));
}

inline allocator_with_fit_mode::fit_mode allocator_boundary_tags::get_fit_mode() const
{
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(std::pmr::memory_resource*));
}

inline size_t allocator_boundary_tags::get_space_size() const
{
    return *reinterpret_cast<size_t*>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(std::pmr::memory_resource*) +
            sizeof(allocator_with_fit_mode::fit_mode));
}

inline std::mutex& allocator_boundary_tags::get_mutex() const
{
    return *reinterpret_cast<std::mutex*>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(std::pmr::memory_resource*) +
            sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t));
}

inline void* allocator_boundary_tags::get_first_block() const
{
    return *reinterpret_cast<void**>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(std::pmr::memory_resource*) +
            sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t) + sizeof(std::mutex));
}

inline void allocator_boundary_tags::set_first_block(void *block)
{
    *reinterpret_cast<void**>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(std::pmr::memory_resource*) +
            sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t) + sizeof(std::mutex)) = block;
}

std::string allocator_boundary_tags::get_memory_state() const
{
    std::vector<std::tuple<char*, size_t, bool>> blocks;
    char* memory_start = static_cast<char*>(_trusted_memory) + allocator_metadata_size;
    char* memory_end = memory_start + get_space_size();
    char* current = memory_start;

    std::vector<char*> allocated_blocks;
    for (auto it = begin(); it != end(); ++it)
        allocated_blocks.push_back(static_cast<char*>(*it));
    std::sort(allocated_blocks.begin(), allocated_blocks.end());

    // Формируем список всех блоков (занятые + свободные)
    for (char* block : allocated_blocks)
    {
        if (block > current)
        {
            blocks.emplace_back(current, block - current, false);
        }
        size_t block_size = *reinterpret_cast<size_t*>(block + 3 * sizeof(void*));
        blocks.emplace_back(block, block_size, true);
        current = block + block_size;
    }

    if (current < memory_end)
    {
        blocks.emplace_back(current, memory_end - current, false);
    }

    // Сортируем по адресам (на всякий)
    std::sort(blocks.begin(), blocks.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    // Формируем строку
    std::stringstream ss;
    for (const auto& [addr, size, occupied] : blocks)
    {
        if (occupied)
        {
            ss << "occup " << (size - occupied_block_metadata_size) << " | ";
        }
        else
        {
            ss << "avail " << size << " | ";
        }
    }

    std::string result = ss.str();
    if (!result.empty())
    {
        result = result.substr(0, result.size() - 3);
    }
    return result;
}

size_t allocator_boundary_tags::calculate_available_memory() const
{
    size_t available = 0;
    char* memory_start = static_cast<char*>(_trusted_memory) + allocator_metadata_size;
    char* memory_end = memory_start + get_space_size();
    char* current = memory_start;

    for (auto it = begin(); it != end(); ++it)
    {
        char* block = static_cast<char*>(*it);
        if (block > current)
        {
            available += (block - current);
        }
        size_t block_size = *reinterpret_cast<size_t*>(block + 3 * sizeof(void*));
        current = block + block_size;
    }

    if (current < memory_end)
    {
        available += (memory_end - current);
    }

    return available;
}
