#include <not_implemented.h>
#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(
    logger *logger)
    : _logger(logger)
{
    debug_with_guard("init allocator_global_heap");
}

[[nodiscard]] void *allocator_global_heap::do_allocate_sm(
    size_t size)
{
    debug_with_guard("do_allocate_sm");

    try
    {
        void *ptr = ::operator new(size);
        trace_with_guard("Allocated " + std::to_string(size));
        return ptr;
    }
    catch (const std::bad_alloc &ex)
    {
        error_with_guard("Allocation failed: " + std::string(ex.what()));
        return nullptr;
    }
}

void allocator_global_heap::do_deallocate_sm(
    void *at)
{
    debug_with_guard("do_deallocate_sm");
    operator delete(at);
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const
{
    return "allocator_global_heap";
}

allocator_global_heap::~allocator_global_heap()
{
    debug_with_guard("end allocator_global_heap");
}

allocator_global_heap::allocator_global_heap(const allocator_global_heap &other)
    :_logger(other._logger)
{}

allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)
{
    if (this != &other)
        _logger = other._logger;
    return *this;
}

bool allocator_global_heap::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return dynamic_cast<const allocator_global_heap *>(&other) != nullptr;
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept
    :_logger(std::exchange(other._logger, nullptr))
{}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept
{
    if (this != &other)
        _logger = std::exchange(other._logger, nullptr);
    return *this;
}
