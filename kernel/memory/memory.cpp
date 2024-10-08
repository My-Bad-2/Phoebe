#include <memory/physical.hpp>
#include <memory/virtual.hpp>
#include <memory/memory.hpp>
#include <memory/heap.hpp>

namespace memory
{
void initialize()
{
	physical_initialize();
	virtual_initialize();
	heap_init();
}
} // namespace memory