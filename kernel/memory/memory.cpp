#include <memory/physical.hpp>
#include <memory/virtual.hpp>
#include <memory/memory.hpp>

namespace memory
{
void initialize()
{
	physical_initialize();
	virtual_initialize();
}
} // namespace memory