#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_buddies_system.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <tuple>

allocator_buddies_system::~allocator_buddies_system()
{
    trace_with_guard("[ALLOC] Destructor called");
    debug_with_guard("~allocator_buddies_system");
    // Получаем размер выделенной памяти
    const size_t total_size = allocator_metadata_size +
                              (1ULL << get_total_space_power());

    // Используем дефолтный аллокатор для освобождения
    std::pmr::memory_resource* parent = std::pmr::new_delete_resource();

    // Освобождаем основную память аллокатора
    parent->deallocate(_trusted_memory, total_size);
    _trusted_memory = nullptr;
}

allocator_buddies_system::allocator_buddies_system(
    allocator_buddies_system &&other) noexcept
        : _trusted_memory(other._trusted_memory)
{
    debug_with_guard("allocator_buddies_system(allocator_buddies_system &&other)");
    other._trusted_memory = nullptr;
}

allocator_buddies_system &allocator_buddies_system::operator=(
    allocator_buddies_system &&other) noexcept
{
    if (this == &other) return *this;

    this->~allocator_buddies_system();
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
    return *this;
}

allocator_buddies_system::allocator_buddies_system(
        size_t space_size_bytes,
        std::pmr::memory_resource* parent_allocator,
        logger* log,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    size_t required_k = __detail::nearest_greater_k_of_2(space_size_bytes);

    try {
        if (required_k < min_k)
            throw std::invalid_argument("Space size is too small");

        if (!parent_allocator)
            parent_allocator = std::pmr::new_delete_resource();

        size_t total_size = allocator_metadata_size + (1ULL << required_k);

        _trusted_memory = parent_allocator->allocate(total_size);

        char *ptr = static_cast<char *>(_trusted_memory);

        *reinterpret_cast<logger **>(ptr) = log;
        ptr += sizeof(logger *);

        *reinterpret_cast<allocator_dbg_helper **>(ptr) = nullptr;
        ptr += sizeof(allocator_dbg_helper *);

        *reinterpret_cast<fit_mode *>(ptr) = allocate_fit_mode;
        ptr += sizeof(fit_mode);

        *reinterpret_cast<unsigned char *>(ptr) = static_cast<unsigned char>(required_k);
        ptr += sizeof(unsigned char);

        new(ptr) std::mutex();
        ptr += sizeof(std::mutex);

        block_metadata *initial_meta = reinterpret_cast<block_metadata *>(ptr);
        initial_meta->occupied = false;
        initial_meta->size = static_cast<unsigned char>(required_k);
    }
    catch (const std::bad_alloc& e)
    {
        error_with_guard("[ALLOC] Failed to allocator_buddies_system " + std::to_string(space_size_bytes));
        throw;
    }
    debug_with_guard("[ALLOC] Exiting allocator_buddies_system");
}

[[nodiscard]] void *allocator_buddies_system::do_allocate_sm(
    size_t size)
{
    if (size == 0)
        warning_with_guard("[ALLOC] Zero-size allocation request");

    debug_with_guard("[ALLOC] Entering do_allocate_sm, bytes: " + std::to_string(size));

    std::lock_guard<std::mutex> lock(*reinterpret_cast<std::mutex*>(
            static_cast<char*>(_trusted_memory) +
            sizeof(logger*) +
            sizeof(allocator_dbg_helper*) +
            sizeof(fit_mode) +
            sizeof(unsigned char)
    ));

    const size_t required_size = size + occupied_block_metadata_size;
    const size_t target_k_raw = std::max(
            __detail::nearest_greater_k_of_2(required_size),
            min_k
    );
    const unsigned char target_k = static_cast<unsigned char>(target_k_raw);

    try {


        void *target_block = nullptr;
        size_t best_size = 0;
        const fit_mode mode = get_fit_mode();

        for (auto it = begin(); it != end(); ++it) {
            if (!it.occupied() && it.size() >= target_k) {
                const size_t current_size = it.size();

                if (mode == fit_mode::first_fit) {
                    target_block = *it;
                    break;
                }

                if (mode == fit_mode::the_best_fit &&
                    (!target_block || current_size < best_size)) {
                    target_block = *it;
                    best_size = current_size;
                }

                if (mode == fit_mode::the_worst_fit &&
                    (!target_block || current_size > best_size)) {
                    target_block = *it;
                    best_size = current_size;
                }
            }
        }

        if (!target_block) {
            throw std::bad_alloc();
        }

        block_metadata *meta = reinterpret_cast<block_metadata *>(
                static_cast<char *>(target_block) - free_block_metadata_size
        );

        while (meta->size > target_k) {
            meta->size--;
            const size_t buddy_offset = 1ULL << meta->size;
            void *buddy = static_cast<char *>(target_block) + buddy_offset;

            block_metadata *buddy_meta = reinterpret_cast<block_metadata *>(
                    static_cast<char *>(buddy) - free_block_metadata_size
            );
            buddy_meta->occupied = false;
            buddy_meta->size = meta->size;
        }

        // Пометить как занятый
        meta->occupied = true;
        *reinterpret_cast<void **>(static_cast<char *>(target_block) + sizeof(block_metadata)) = _trusted_memory;

        debug_with_guard("After deallocation: " + format_blocks_info());

        return static_cast<char *>(target_block) + occupied_block_metadata_size;
    }
    catch (const std::bad_alloc& e)
    {
        error_with_guard("[ALLOC] Failed to do_allocate_sm in allocator_boundary_tags " + std::to_string(size));
        throw;
    }
}

void allocator_buddies_system::do_deallocate_sm(void *at)
{
    if (!at) return;
    debug_with_guard("[DEALLOC] Entering do_deallocate_sm");

    std::lock_guard<std::mutex> lock(*reinterpret_cast<std::mutex*>(
            static_cast<char*>(_trusted_memory) +
            sizeof(logger*) +
            sizeof(allocator_dbg_helper*) +
            sizeof(fit_mode) +
            sizeof(unsigned char)
    ));

    // Получаем метаданные блока
    void *block_start = static_cast<char *>(at) - occupied_block_metadata_size;
    block_metadata *meta = reinterpret_cast<block_metadata *>(
            static_cast<char *>(block_start) - free_block_metadata_size
    );

    if (!meta->occupied) return;
    meta->occupied = false;

    // Слияние с бадди
    bool merged;
    const size_t max_power = get_total_space_power();
    try {
        do {
            merged = false;
            const size_t current_size = 1ULL << meta->size;

            // Вычисляем адрес бадди через XOR
            const uintptr_t block_addr = reinterpret_cast<uintptr_t>(block_start);
            const uintptr_t buddy_addr = block_addr ^ current_size;
            void *buddy = reinterpret_cast<void *>(buddy_addr);

            // Проверяем границы памяти
            if (buddy >= (static_cast<char *>(_trusted_memory) + allocator_metadata_size) &&
                buddy < get_pool_end()) {
                block_metadata *buddy_meta = reinterpret_cast<block_metadata *>(
                        static_cast<char *>(buddy) - free_block_metadata_size
                );

                if (!buddy_meta->occupied && buddy_meta->size == meta->size && meta->size < max_power) {
                    // Сливаем блоки
                    meta->size++;
                    merged = true;

                    // Выбираем младший блок как новый базовый
                    if (buddy < block_start) {
                        meta = buddy_meta;
                        block_start = buddy;
                    }
                }
            }
        } while (merged);
        debug_with_guard("After deallocation: " + format_blocks_info());
    }
    catch (const std::bad_alloc& e)
    {
        error_with_guard("[ALLOC] Failed to allocator_boundary_tags " + std::to_string(max_power));
        throw;
    }
    debug_with_guard("[ALLOC] Exiting allocator_boundary_tags");
}

allocator_buddies_system::allocator_buddies_system(const allocator_buddies_system &other)
{
    // Блокируем исходный аллокатор на время копирования
    std::lock_guard<std::mutex> lock_other(
            *reinterpret_cast<std::mutex*>(
                    static_cast<char*>(other._trusted_memory) +
                    sizeof(logger*) +
                    sizeof(allocator_dbg_helper*) +
                    sizeof(fit_mode) +
                    sizeof(unsigned char)
            ));

    // Получаем параметры из исходного аллокатора
    size_t space_power = other.get_total_space_power();
    auto* parent_alloc = reinterpret_cast<std::pmr::memory_resource*>(
            static_cast<char*>(other._trusted_memory) +
            allocator_metadata_size -
            sizeof(std::pmr::memory_resource*));

    // Выделяем память такого же размера
    size_t total_size = allocator_metadata_size + (1ULL << space_power);
    _trusted_memory = parent_alloc->allocate(total_size);

    // Копируем метаданные побайтно
    memcpy(_trusted_memory, other._trusted_memory, total_size);

    // Инициализируем новый мьютекс
    new (static_cast<char*>(_trusted_memory) +
         sizeof(logger*) +
         sizeof(allocator_dbg_helper*) +
         sizeof(fit_mode) +
         sizeof(unsigned char)) std::mutex();

    // Копируем состояние блоков
    char* src = static_cast<char*>(other._trusted_memory) + allocator_metadata_size;
    char* dst = static_cast<char*>(_trusted_memory) + allocator_metadata_size;
    memcpy(dst, src, 1ULL << space_power);

    // Настраиваем указатели на аллокатор в блоках
    for (auto it = begin(); it != end(); ++it) {
        if (it.occupied()) {
            *reinterpret_cast<void**>(
                    static_cast<char*>(*it) -
                    occupied_block_metadata_size +
                    sizeof(block_metadata)) = _trusted_memory;
        }
    }
}

allocator_buddies_system &allocator_buddies_system::operator=(const allocator_buddies_system &other)
{
    if (this != &other) {
        // Удаляем текущие данные
        this->~allocator_buddies_system();

        // Используем конструктор копирования
        new (this) allocator_buddies_system(other);
    }
    return *this;
}

bool allocator_buddies_system::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return *this == other;
}

inline void allocator_buddies_system::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    *reinterpret_cast<fit_mode*>(
            static_cast<char*>(_trusted_memory) +
            sizeof(logger*) +
            sizeof(allocator_dbg_helper*)
    ) = mode;
}


std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    std::vector<allocator_test_utils::block_info> blocks;

    for (auto it = begin(); it != end(); ++it)
    {
        blocks.push_back({
                                 .block_size = it.size(),
                                 .is_block_occupied = it.occupied()
                         });
    }

    return blocks;
}

inline logger *allocator_buddies_system::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_buddies_system::get_typename() const
{
    return "allocator_buddies_system";
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info_inner() const
{
    return get_blocks_info();
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::begin() const noexcept
{
    void *pool_start = static_cast<char*>(_trusted_memory) + allocator_metadata_size;
    return buddy_iterator(pool_start);
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::end() const noexcept
{
    size_t space_power = *reinterpret_cast<unsigned char*>(
            static_cast<char*>(_trusted_memory) + sizeof(logger*) + sizeof(allocator_dbg_helper*) + sizeof(fit_mode)
    );
    void *pool_end = static_cast<char*>(_trusted_memory) + allocator_metadata_size + (1ULL << space_power);
    return buddy_iterator(pool_end);
}

bool allocator_buddies_system::buddy_iterator::operator==(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block == other._block;
}

bool allocator_buddies_system::buddy_iterator::operator!=(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block != other._block;
}

allocator_buddies_system::buddy_iterator &allocator_buddies_system::buddy_iterator::operator++() & noexcept
{
    if (!_block) return *this;

    block_metadata *meta = reinterpret_cast<block_metadata*>(_block);
    size_t block_size = 1ULL << meta->size;
    _block = static_cast<char*>(_block) + block_size;

    return *this;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::buddy_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_buddies_system::buddy_iterator::size() const noexcept
{
    if (!_block) return 0;

    block_metadata *meta = reinterpret_cast<block_metadata*>(_block);
    return 1ULL << meta->size;
}

bool allocator_buddies_system::buddy_iterator::occupied() const noexcept
{
    if (!_block) return false;

    block_metadata *meta = reinterpret_cast<block_metadata*>(_block);
    return meta->occupied;
}

void *allocator_buddies_system::buddy_iterator::operator*() const noexcept
{
    if (!_block) return nullptr;

    block_metadata *meta = reinterpret_cast<block_metadata*>(_block);
    if (meta->occupied)
    {
        return static_cast<char*>(_block) + occupied_block_metadata_size;
    }
    else
    {
        return static_cast<char*>(_block) + free_block_metadata_size;
    }
}

allocator_buddies_system::buddy_iterator::buddy_iterator(void *start)
        : _block(start)
{}


allocator_buddies_system::buddy_iterator::buddy_iterator()
        : _block(nullptr)
{}

allocator_with_fit_mode::fit_mode allocator_buddies_system::get_fit_mode() const noexcept
{
    // Смещение: logger* + allocator_dbg_helper*
    return *reinterpret_cast<fit_mode*>(
            static_cast<char*>(_trusted_memory) +
            sizeof(logger*) +
            sizeof(allocator_dbg_helper*)
    );
}

size_t allocator_buddies_system::get_total_space_power() const noexcept
{
    // Смещение: logger* + allocator_dbg_helper* + fit_mode
    return *reinterpret_cast<unsigned char*>(
            static_cast<char*>(_trusted_memory) +
            sizeof(logger*) +
            sizeof(allocator_dbg_helper*) +
            sizeof(fit_mode)
    );
}

void* allocator_buddies_system::get_pool_end() const noexcept
{
    // Конец пула = начало метаданных + их размер + размер самого пула
    return static_cast<char*>(_trusted_memory) +
           allocator_metadata_size +
           (1ULL << get_total_space_power());
}

std::vector<std::pair<void*, allocator_buddies_system::block_info>> allocator_buddies_system::get_blocks_with_addresses() const noexcept
{
    std::vector<std::pair<void*, block_info>> blocks;

    char* current = static_cast<char*>(_trusted_memory) + allocator_metadata_size;
    const char* end_ptr = static_cast<char*>(get_pool_end());

    while (current < end_ptr)
    {
        block_metadata* meta = reinterpret_cast<block_metadata*>(
                current - free_block_metadata_size);

        blocks.emplace_back(
                current,
                block_info{
                        1ULL << meta->size,
                        meta->occupied
                }
        );

        current += 1ULL << meta->size;
    }

    return blocks;
}

std::string allocator_buddies_system::format_blocks_info() const noexcept
{
    auto blocks = get_blocks_with_addresses();
    std::sort(blocks.begin(), blocks.end(),
              [](auto& a, auto& b) { return a.first < b.first; });

    std::stringstream ss;
    for (size_t i = 0; i < blocks.size(); ++i)
    {
        ss << (blocks[i].second.is_block_occupied ? "occup " : "avail ")
           << blocks[i].second.block_size;
        if (i != blocks.size() - 1) ss << "|";
    }
    return ss.str();
}